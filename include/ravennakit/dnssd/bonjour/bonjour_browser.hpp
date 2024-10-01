#pragma once

#include "scoped_dns_service_ref.hpp"
#include "shared_connection.hpp"
#include "ravennakit/dnssd/dnssd_browser.hpp"
#include "ravennakit/dnssd/result.hpp"

#include <atomic>
#include <map>
#include <mutex>
#include <thread>

namespace rav::dnssd {

class service;

/**
 * Apple Bonjour implementation of IBrowser. Works on macOS and Windows.
 */
class bonjour_browser: public dnssd_browser {
  public:
    explicit bonjour_browser();
    ~bonjour_browser() override;

    // MARK: IBrowser implementations -
    result browse_for(const std::string& service) override;
    bool report_if_error(const result& result) const noexcept;

    /**
     * Called by dns_sd logic in response to a browse reply.
     * @param browse_service_ref The DNSServiceRef.
     * @param flags Possible values are kDNSServiceFlagsMoreComing and kDNSServiceFlagsAdd.
     *                See flag definitions for details.
     * @param interface_index The interface on which the service is advertised. This index should be passed to
     *                       DNSServiceResolve() when resolving the service.
     * @param error_code Will be kDNSServiceErr_NoError (0) on success, otherwise will indicate the failure that
     *                  occurred. Other parameters are undefined if the errorCode is nonzero.
     * @param name The discovered service name. This name should be displayed to the user, and stored for subsequent use
     *             in the DNSServiceResolve() call.
     * @param type The service type.
     * @param domain The domain of the discovered service instance.
     */
    void browse_reply(
        DNSServiceRef browse_service_ref, DNSServiceFlags flags, uint32_t interface_index,
        DNSServiceErrorType error_code, const char* name, const char* type, const char* domain
    );

    /**
     * @return Returns the SharedConnection this instance is using for communicating with the mdns responder.
     */
    const shared_connection& connection() const noexcept {
        return shared_connection_;
    }

  private:
    shared_connection shared_connection_;
    std::map<std::string, scoped_dns_service_ref> browsers_;
    std::map<std::string, service> services_;
    std::atomic<bool> keep_going_ = {true};
    std::thread thread_;
    std::recursive_mutex lock_;

    void thread();
};

}  // namespace dnssd
