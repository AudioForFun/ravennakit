#include "ravennakit/dnssd/bonjour/bonjour.hpp"

#if RAV_HAS_APPLE_DNSSD

    #include "ravennakit/dnssd/bonjour/bonjour_browser.hpp"
    #include "ravennakit/core/log.hpp"
    #include "ravennakit/dnssd/bonjour/bonjour_scoped_dns_service_ref.hpp"
    #include "ravennakit/dnssd/bonjour/bonjour_txt_record.hpp"

    #include <mutex>

rav::dnssd::bonjour_browser::service::service(
    const char* fullname, const char* name, const char* type, const char* domain, rav::dnssd::bonjour_browser& owner
) :
    owner_(owner) {
    description_.fullname = fullname;
    description_.name = name;
    description_.type = type;
    description_.domain = domain;
}

void rav::dnssd::bonjour_browser::service::resolve_on_interface(uint32_t index) {
    if (resolvers_.find(index) != resolvers_.end()) {
        // Already resolving on this interface
        return;
    }

    description_.interfaces.insert({index, {}});

    DNSServiceRef resolveServiceRef = owner_.shared_connection_.service_ref();

    DNSSD_THROW_IF_ERROR(
        "Failed to resolve service",
        DNSServiceResolve(
            &resolveServiceRef, kDNSServiceFlagsShareConnection, index, description_.name.c_str(),
            description_.type.c_str(), description_.domain.c_str(), resolve_callback, this
        )
    );

    resolvers_.insert({index, bonjour_scoped_dns_service_ref(resolveServiceRef)});
}

void rav::dnssd::bonjour_browser::service::resolve_callback(
    [[maybe_unused]] DNSServiceRef serviceRef, [[maybe_unused]] DNSServiceFlags flags, uint32_t interface_index,
    DNSServiceErrorType error_code, [[maybe_unused]] const char* fullname, const char* host_target, uint16_t port,
    uint16_t txt_len, const unsigned char* txt_record, void* context
) {
    DNSSD_THROW_IF_ERROR("Resolve failed", error_code);

    auto* browser_service = static_cast<service*>(context);

    browser_service->description_.host = host_target;
    browser_service->description_.port = port;
    browser_service->description_.txt = bonjour_txt_record::get_txt_record_from_raw_bytes(txt_record, txt_len);

    browser_service->owner_.emit(events::service_resolved {browser_service->description_, interface_index});

    DNSServiceRef getAddrInfoServiceRef = browser_service->owner_.shared_connection_.service_ref();

    DNSSD_THROW_IF_ERROR(
        "Failed to get addr info",
        DNSServiceGetAddrInfo(
            &getAddrInfoServiceRef, kDNSServiceFlagsShareConnection | kDNSServiceFlagsTimeout, interface_index,
            kDNSServiceProtocol_IPv4 | kDNSServiceProtocol_IPv6, host_target, get_addr_info_callback, browser_service
        )
    );

    browser_service->get_addrs_.insert({interface_index, bonjour_scoped_dns_service_ref(getAddrInfoServiceRef)});
}

void rav::dnssd::bonjour_browser::service::get_addr_info_callback(
    [[maybe_unused]] DNSServiceRef sd_ref, [[maybe_unused]] DNSServiceFlags flags, const uint32_t interface_index,
    const DNSServiceErrorType error_code, [[maybe_unused]] const char* hostname, const struct sockaddr* address,
    [[maybe_unused]] uint32_t ttl, void* context
) {
    auto* browser_service = static_cast<service*>(context);

    if (error_code == kDNSServiceErr_Timeout) {
        browser_service->get_addrs_.erase(interface_index);
        return;
    }

    DNSSD_THROW_IF_ERROR("Get addr info callback error", error_code);

    char ip_addr[INET6_ADDRSTRLEN] = {};

    const void* ip_addr_data = nullptr;

    if (address->sa_family == AF_INET) {
        ip_addr_data = &reinterpret_cast<const sockaddr_in*>(address)->sin_addr;
    } else if (address->sa_family == AF_INET6) {
        ip_addr_data = &reinterpret_cast<const sockaddr_in6*>(address)->sin6_addr;
    } else {
        return;  // Don't know how to handle this case
    }

    // Winsock version requires the const cast because Microsoft.
    inet_ntop(address->sa_family, const_cast<void*>(ip_addr_data), ip_addr, INET6_ADDRSTRLEN);

    const auto found_interface = browser_service->description_.interfaces.find(interface_index);
    if (found_interface != browser_service->description_.interfaces.end()) {
        const auto result = found_interface->second.insert(ip_addr);
        browser_service->owner_.emit(
            events::address_added {browser_service->description_, *result.first, interface_index}
        );
    } else {
        RAV_THROW_EXCEPTION(std::string("Interface with id \"") + std::to_string(interface_index) + "\" not found");
    }
}

size_t rav::dnssd::bonjour_browser::service::remove_interface(uint32_t index) {
    const auto found_interface = description_.interfaces.find(index);
    if (found_interface == description_.interfaces.end()) {
        RAV_ERROR("Interface with id \"{}\" not found", index);
        return description_.interfaces.size();
    }

    if (description_.interfaces.size() > 1) {
        for (auto& addr : found_interface->second) {
            owner_.emit(events::address_removed {description_, addr, index});
        }
    }

    description_.interfaces.erase(found_interface);
    resolvers_.erase(index);
    get_addrs_.erase(index);

    return description_.interfaces.size();
}

const rav::dnssd::service_description& rav::dnssd::bonjour_browser::service::description() const noexcept {
    return description_;
}

rav::dnssd::bonjour_browser::bonjour_browser() {
    process_results_thread_.start(shared_connection_.service_ref());
}

rav::dnssd::bonjour_browser::~bonjour_browser() {
    process_results_thread_.stop();
}

void rav::dnssd::bonjour_browser::browse_reply(
    [[maybe_unused]] DNSServiceRef browse_service_ref, DNSServiceFlags flags, uint32_t interface_index,
    const DNSServiceErrorType error_code, const char* name, const char* type, const char* domain, void* context
) {
    auto* browser = static_cast<bonjour_browser*>(context);

    DNSSD_THROW_IF_ERROR("Browse reply error", error_code);

    RAV_DEBUG(
        "browse_reply name={} type={} domain={} browse_service_ref={} interfaceIndex={}", name, type, domain,
        static_cast<void*>(browse_service_ref), interface_index
    );

    char fullname[kDNSServiceMaxDomainName] = {};
    DNSSD_THROW_IF_ERROR("Failed to construct full name", DNSServiceConstructFullName(fullname, name, type, domain));

    if (flags & kDNSServiceFlagsAdd) {
        // Insert a new service if not already present
        auto s = browser->services_.find(fullname);
        if (s == browser->services_.end()) {
            s = browser->services_.insert({fullname, service(fullname, name, type, domain, *browser)}).first;

            browser->emit(events::service_discovered {s->second.description()});
        }

        s->second.resolve_on_interface(interface_index);
    } else {
        auto const foundService = browser->services_.find(fullname);
        if (foundService == browser->services_.end()) {
            RAV_THROW_EXCEPTION(std::string("Service with fullname \"") + fullname + "\" not found");
        }

        if (foundService->second.remove_interface(interface_index) == 0) {
            // We just removed the last interface
            browser->emit(events::service_removed {foundService->second.description()});

            // Remove the BrowseResult (as there are not interfaces left)
            browser->services_.erase(foundService);
        }
    }
}

void rav::dnssd::bonjour_browser::browse_for(const std::string& service) {
    auto guard = process_results_thread_.lock();

    // Initialize with the shared connection to pass it to DNSServiceBrowse
    DNSServiceRef browsingServiceRef = shared_connection_.service_ref();

    if (browsingServiceRef == nullptr) {
        RAV_THROW_EXCEPTION("DNSSD Service not running");
    }

    if (browsers_.find(service) != browsers_.end()) {
        // Already browsing for this service
        RAV_THROW_EXCEPTION("Already browsing for service \"" + service + "\"");
    }

    DNSSD_THROW_IF_ERROR(
        "Browse error: ",
        DNSServiceBrowse(
            &browsingServiceRef, kDNSServiceFlagsShareConnection, kDNSServiceInterfaceIndexAny, service.c_str(),
            nullptr, browse_reply, this
        )
    );

    browsers_.insert({service, bonjour_scoped_dns_service_ref(browsingServiceRef)});
    // From here the serviceRef is under RAII inside the ScopedDnsServiceRef class
}

#endif
