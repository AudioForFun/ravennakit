#include "ravennakit/dnssd/bonjour/bonjour_browser.hpp"

#include "ravennakit/dnssd/log.hpp"
#include "ravennakit/dnssd/bonjour/scoped_dns_service_ref.hpp"

#include <mutex>

static void DNSSD_API browseReply2(
    DNSServiceRef browseServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
    const char* name, const char* type, const char* domain, void* context
) {
    auto* browser = static_cast<dnssd::bonjour_browser*>(context);
    browser->browse_reply(browseServiceRef, flags, interfaceIndex, errorCode, name, type, domain);
}

dnssd::bonjour_browser::bonjour_browser() : thread_(std::thread(&bonjour_browser::thread, this)) {}

void dnssd::bonjour_browser::browse_reply(
    [[maybe_unused]] DNSServiceRef browse_service_ref, DNSServiceFlags flags, uint32_t interface_index,
    DNSServiceErrorType error_code, const char* name, const char* type, const char* domain
) {
    DNSSD_LOG_DEBUG("> browseReply enter (context=" << this << ")" << std::endl)

    if (report_if_error(result(error_code)))
        return;

    DNSSD_LOG_DEBUG(
        "> browseReply"
        << " name=" << name << " type=" << type << " domain=" << domain << " browseServiceRef=" << browseServiceRef
        << " interfaceIndex=" << interfaceIndex << std::endl
    )

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

    DNSSD_LOG_DEBUG("< browseReply exit (" << std::this_thread::get_id() << ")" << std::endl)
}

bool dnssd::bonjour_browser::report_if_error(const dnssd::result& result) const noexcept {
    if (result.has_error()) {
        if (onBrowseErrorCallback)
            onBrowseErrorCallback(result);
        return true;
    }
    return false;
}

dnssd::result dnssd::bonjour_browser::browse_for(const std::string& service) {
    std::lock_guard<std::recursive_mutex> lg(lock_);

    // Initialize with the shared connection to pass it to DNSServiceBrowse
    DNSServiceRef browsingServiceRef = shared_connection_.service_ref();

    if (browsingServiceRef == nullptr)
        return result(kDNSServiceErr_ServiceNotRunning);

    if (browsers_.find(service) != browsers_.end()) {
        // Already browsing for this service
        return result("already browsing for service \"" + service + "\"");
    }

    auto result = dnssd::result(DNSServiceBrowse(
        &browsingServiceRef, kDNSServiceFlagsShareConnection, kDNSServiceInterfaceIndexAny, service.c_str(), nullptr,
        ::browseReply2, this
    ));

    if (result.has_error())
        return result;

    browsers_.insert({service, scoped_dns_service_ref(browsingServiceRef)});
    // From here the serviceRef is under RAII inside the ScopedDnsServiceRef class

    return {};
}

void dnssd::bonjour_browser::thread() {
    DNSSD_LOG_DEBUG("> Start browse thread" << std::endl)

    struct timeval tv = {};
    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    int fd = -1;
    int nfds = -1;

    fd = DNSServiceRefSockFD(shared_connection_.service_ref());

    if (fd < 0) {
        if (report_if_error(result("Invalid file descriptor"))) {
            DNSSD_LOG_DEBUG("< Invalid file descriptor" << std::endl)
            return;
        }
    }

    while (keep_going_.load()) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        nfds = fd + 1;

        const int r = select(nfds, &readfds, (fd_set*)nullptr, (fd_set*)nullptr, &tv);

        {
            if (r < 0)  // Result
            {
                if (report_if_error(result("Select error: " + std::to_string(r)))) {
                    DNSSD_LOG_DEBUG("! Result (code=" << result << ")" << std::endl)
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

                    DNSSD_LOG_DEBUG("> Main loop (FD_ISSET == true)" << std::endl)
                    (void)report_if_error(result(DNSServiceProcessResult(shared_connection_.service_ref())));
                } else {
                    DNSSD_LOG_DEBUG("> Main loop (FD_ISSET == false)" << std::endl)
                }
            }
        }
    }

    DNSSD_LOG_DEBUG("< Stop browse thread" << std::endl)
}

dnssd::bonjour_browser::~bonjour_browser() {
    keep_going_ = false;

    std::lock_guard<std::recursive_mutex> lg(lock_);
    if (thread_.joinable())
        thread_.join();
}
