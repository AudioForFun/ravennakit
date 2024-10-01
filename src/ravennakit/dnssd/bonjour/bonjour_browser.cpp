#include "ravennakit/dnssd/bonjour/bonjour_browser.hpp"

#include "ravennakit/core/log.hpp"
#include "ravennakit/dnssd/bonjour/scoped_dns_service_ref.hpp"

#include <mutex>

static void DNSSD_API browseReply2(
    DNSServiceRef browseServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
    const char* name, const char* type, const char* domain, void* context
) {
    auto* browser = static_cast<rav::dnssd::bonjour_browser*>(context);
    browser->browse_reply(browseServiceRef, flags, interfaceIndex, errorCode, name, type, domain);
}

rav::dnssd::bonjour_browser::bonjour_browser() : thread_(std::thread(&bonjour_browser::thread, this)) {}

void rav::dnssd::bonjour_browser::browse_reply(
    [[maybe_unused]] DNSServiceRef browse_service_ref, DNSServiceFlags flags, uint32_t interface_index,
    DNSServiceErrorType error_code, const char* name, const char* type, const char* domain
) {
    if (report_if_error(result(error_code)))
        return;

    RAV_DEBUG(
        "browse_reply name={} type={} domain={} browse_service_ref={} interfaceIndex={}", name, type, domain,
        static_cast<void*>(browse_service_ref), interface_index
    );

    char fullname[kDNSServiceMaxDomainName] = {};
    if (report_if_error(result(DNSServiceConstructFullName(fullname, name, type, domain))))
        return;

    if (flags & kDNSServiceFlagsAdd) {
        // Insert a new service if not already present
        auto s = services_.find(fullname);
        if (s == services_.end()) {
            s = services_.insert({fullname, service(fullname, name, type, domain, *this)}).first;
            if (onServiceDiscoveredCallback)
                onServiceDiscoveredCallback(s->second.description());
        }

        s->second.resolve_on_interface(interface_index);
    } else {
        auto const foundService = services_.find(fullname);
        if (foundService == services_.end())
            if (report_if_error(result(std::string("service with fullname \"") + fullname + "\" not found")))
                return;

        if (foundService->second.remove_interface(interface_index) == 0) {
            // We just removed the last interface
            if (onServiceRemovedCallback)
                onServiceRemovedCallback(foundService->second.description());

            // Remove the BrowseResult (as there are not interfaces left)
            services_.erase(foundService);
        }
    }
}

bool rav::dnssd::bonjour_browser::report_if_error(const rav::dnssd::result& result) const noexcept {
    if (result.has_error()) {
        if (onBrowseErrorCallback)
            onBrowseErrorCallback(result);
        return true;
    }
    return false;
}

rav::dnssd::result rav::dnssd::bonjour_browser::browse_for(const std::string& service) {
    std::lock_guard<std::recursive_mutex> lg(lock_);

    // Initialize with the shared connection to pass it to DNSServiceBrowse
    DNSServiceRef browsingServiceRef = shared_connection_.service_ref();

    if (browsingServiceRef == nullptr)
        return result(kDNSServiceErr_ServiceNotRunning);

    if (browsers_.find(service) != browsers_.end()) {
        // Already browsing for this service
        return result("already browsing for service \"" + service + "\"");
    }

    auto result = rav::dnssd::result(DNSServiceBrowse(
        &browsingServiceRef, kDNSServiceFlagsShareConnection, kDNSServiceInterfaceIndexAny, service.c_str(), nullptr,
        ::browseReply2, this
    ));

    if (result.has_error())
        return result;

    browsers_.insert({service, scoped_dns_service_ref(browsingServiceRef)});
    // From here the serviceRef is under RAII inside the ScopedDnsServiceRef class

    return {};
}

void rav::dnssd::bonjour_browser::thread() {
    RAV_DEBUG("Start bonjour browser thread");

    struct timeval tv = {};
    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    const int fd = DNSServiceRefSockFD(shared_connection_.service_ref());

    if (fd < 0) {
        if (report_if_error(result("Invalid file descriptor"))) {
            RAV_ERROR("Invalid file descriptor");
            return;
        }
    }

    while (keep_going_.load()) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        int nfds = fd + 1;

        const int r = select(nfds, &readfds, nullptr, nullptr, &tv);

        {
            if (r < 0)  // Result
            {
                if (report_if_error(result("Select error: " + std::to_string(r)))) {
                    RAV_DEBUG("! Result (code={})", r);
                    break;
                }
            } else if (r == 0)  // Timeout
            {
            } else {
                if (FD_ISSET(fd, &readfds)) {
                    if (keep_going_.load() == false)
                        return;

                    // Locking here will make sure that all callbacks are synchronised because they are called in
                    // response to DNSServiceProcessResult.
                    std::lock_guard<std::recursive_mutex> lg(lock_);

                    RAV_DEBUG("Main loop (FD_ISSET == true)");
                    (void)report_if_error(result(DNSServiceProcessResult(shared_connection_.service_ref())));
                } else {
                    RAV_DEBUG("Main loop (FD_ISSET == false)");
                }
            }
        }
    }

    RAV_DEBUG("Stop bonjour browser thread");
}

rav::dnssd::bonjour_browser::~bonjour_browser() {
    keep_going_ = false;

    std::lock_guard guard(lock_);
    if (thread_.joinable())
        thread_.join();
}
