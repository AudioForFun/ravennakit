#pragma once

#include "bonjour.hpp"

#if RAV_HAS_APPLE_DNSSD

#include "ravennakit/dnssd/dnssd_advertiser.hpp"
#include "ravennakit/dnssd/result.hpp"
#include "ravennakit/dnssd/service_description.hpp"

#include <map>
#include <mutex>

namespace rav::dnssd
{

/**
 * Apple Bonjour implementation of IAdvertiser. Works on macOS and Windows.
 */
class bonjour_advertiser : public dnssd_advertiser
{
public:
    explicit bonjour_advertiser() = default;

    // MARK: IAdvertiser implementations -
    result register_service (
        const std::string& reg_type,
        const char* name,
        const char* domain,
        uint16_t port,
        const txt_record& txt_record) noexcept override;

    result update_txt_record (const txt_record& txt_record) override;
    void unregister_service() noexcept override;

private:
    bonjour_scoped_dns_service_ref service_ref_;

    static void DNSSD_API registerServiceCallBack (
        DNSServiceRef service_ref,
        DNSServiceFlags flags,
        DNSServiceErrorType error_code,
        const char* service_name,
        const char* reg_type,
        const char* reply_domain,
        void* context);
};

} // namespace dnssd

#endif
