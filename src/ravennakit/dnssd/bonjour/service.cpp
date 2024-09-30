#include "ravennakit/dnssd/bonjour/service.hpp"

#include "ravennakit/dnssd/log.hpp"
#include "ravennakit/dnssd/result.hpp"
#include "ravennakit/dnssd/bonjour/bonjour_txt_record.hpp"
#include "ravennakit/dnssd/bonjour/bonjour_browser.hpp"
#include "ravennakit/dnssd/bonjour/scoped_dns_service_ref.hpp"

#include <map>
#include <thread>

static void DNSSD_API resolveCallBack(
    DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
    const char* fullname, const char* hosttarget,
    uint16_t port,  // In network byte order
    uint16_t txtLen, const unsigned char* txtRecord, void* context
) {
    auto* service = static_cast<dnssd::service*>(context);
    service->resolve_callback(
        sdRef, flags, interfaceIndex, errorCode, fullname, hosttarget, ntohs(port), txtLen, txtRecord
    );
}

static void DNSSD_API getAddrInfoCallBack(
    DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
    const char* fullname, const struct sockaddr* address, uint32_t ttl, void* context
) {
    auto* service = static_cast<dnssd::service*>(context);
    service->get_addr_info_callback(sdRef, flags, interfaceIndex, errorCode, fullname, address, ttl);
}

dnssd::service::service(
    const char* fullname, const char* name, const char* type, const char* domain, bonjour_browser& owner
) :
    owner_(owner) {
    description_.fullname = fullname;
    description_.name = name;
    description_.type = type;
    description_.domain = domain;
}

void dnssd::service::resolve_on_interface(uint32_t index) {
    if (resolvers_.find(index) != resolvers_.end()) {
        // Already resolving on this interface
        return;
    }

    description_.interfaces.insert({index, {}});

    DNSServiceRef resolveServiceRef = owner_.connection().service_ref();

    if (owner_.report_if_error(result(DNSServiceResolve(
            &resolveServiceRef, kDNSServiceFlagsShareConnection, index, description_.name.c_str(),
            description_.type.c_str(), description_.domain.c_str(), ::resolveCallBack, this
        )))) {
        return;
    }

    resolvers_.insert({index, scoped_dns_service_ref(resolveServiceRef)});
}

void dnssd::service::resolve_callback(
    DNSServiceRef serviceRef, DNSServiceFlags flags, uint32_t interface_index, DNSServiceErrorType error_code,
    const char* fullname, const char* host_target, uint16_t port, uint16_t txt_len, const unsigned char* txt_record
) {
    DNSSD_LOG_DEBUG("> resolveCallBack enter (" << std::this_thread::get_id() << ") context=" << this << std::endl)
    DNSSD_LOG_DEBUG("- fullname=" << fullname << std::endl);

    if (owner_.report_if_error(result(error_code))) {
        return;
    }

    description_.host = host_target;
    description_.port = port;
    description_.txt = bonjour_txt_record::get_txt_record_from_raw_bytes(txt_record, txt_len);

    DNSSD_LOG_DEBUG("- resolveCallBack: " << mDescription.description() << std::endl)

    if (owner_.onServiceResolvedCallback)
        owner_.onServiceResolvedCallback(description_, interface_index);

    DNSServiceRef getAddrInfoServiceRef = owner_.connection().service_ref();

    if (owner_.report_if_error(result(DNSServiceGetAddrInfo(
            &getAddrInfoServiceRef, kDNSServiceFlagsShareConnection | kDNSServiceFlagsTimeout, interface_index,
            kDNSServiceProtocol_IPv4 | kDNSServiceProtocol_IPv6, host_target, ::getAddrInfoCallBack, this
        )))) {
        return;
    }

    get_addrs_.insert({interface_index, scoped_dns_service_ref(getAddrInfoServiceRef)});

    DNSSD_LOG_DEBUG("< resolveCallBack exit (" << std::this_thread::get_id() << ")" << std::endl)
}

void dnssd::service::get_addr_info_callback(
    DNSServiceRef sd_ref, DNSServiceFlags flags, uint32_t interface_index, DNSServiceErrorType error_code,
    const char* hostname, const struct sockaddr* address, uint32_t ttl
) {
    DNSSD_LOG_DEBUG("> getAddrInfoCallBack enter (" << std::this_thread::get_id() << ") context=" << this << std::endl)
    DNSSD_LOG_DEBUG("- getAddrInfoCallBack hostname: " << hostname << std::endl)

    if (error_code == kDNSServiceErr_Timeout) {
        get_addrs_.erase(interface_index);
        return;
    }

    if (owner_.report_if_error(result(error_code))) {
        return;
    }

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

    auto foundInterface = description_.interfaces.find(interface_index);
    if (foundInterface != description_.interfaces.end()) {
        auto result = foundInterface->second.insert(ip_addr);

        if (owner_.onAddressAddedCallback)
            owner_.onAddressAddedCallback(description_, *result.first, interface_index);
    } else {
        (void)owner_.report_if_error(
            result(std::string("Interface with id \"") + std::to_string(interface_index) + "\" not found")
        );
    }

    DNSSD_LOG_DEBUG("- Address: " << ip_addr << std::endl)
    DNSSD_LOG_DEBUG("< getAddrInfoCallBack exit (" << std::this_thread::get_id() << ")" << std::endl)
}

size_t dnssd::service::remove_interface(uint32_t index) {
    auto foundInterface = description_.interfaces.find(index);
    if (foundInterface == description_.interfaces.end()) {
        (void
        )owner_.report_if_error(result(std::string("interface with index \"") + std::to_string(index) + "\" not found")
        );
        return description_.interfaces.empty();
    }

    if (description_.interfaces.size() > 1) {
        for (auto& addr : foundInterface->second) {
            if (owner_.onAddressRemovedCallback)
                owner_.onAddressRemovedCallback(description_, addr, index);
        }
    }

    description_.interfaces.erase(foundInterface);
    resolvers_.erase(index);
    get_addrs_.erase(index);

    return description_.interfaces.size();
}

const dnssd::service_description& dnssd::service::description() const noexcept {
    return description_;
}
