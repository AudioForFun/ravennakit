#pragma once

#include "ravennakit/dnssd/bonjour/bonjour.hpp"

namespace dnssd
{

/**
 * RAII wrapper around DNSServiceRef.
 */
class scoped_dns_service_ref
{
public:
    scoped_dns_service_ref() = default;
    ~scoped_dns_service_ref();

    scoped_dns_service_ref (const scoped_dns_service_ref&) = delete;

    scoped_dns_service_ref (scoped_dns_service_ref&& other) noexcept;
    explicit scoped_dns_service_ref (const DNSServiceRef& serviceRef) noexcept;

    scoped_dns_service_ref& operator= (const scoped_dns_service_ref& other) = delete;
    scoped_dns_service_ref& operator= (scoped_dns_service_ref&& other) noexcept;

    /**
     * Assigns an existing DNSServiceRef to this instance. An existing DNSServiceRef will be deallocated, and this
     * object will take ownership over the given DNSServiceRef.
     * @param serviceRef The DNSServiceRef to assign to this instance.
     * @return A reference to this instance.
     */
    scoped_dns_service_ref& operator= (DNSServiceRef serviceRef);

    /**
     * @return Returns the contained DNSServiceRef.
     */
    [[nodiscard]] DNSServiceRef service_ref() const noexcept { return service_ref_; }

private:
    DNSServiceRef service_ref_ = nullptr;
};

} // namespace dnssd