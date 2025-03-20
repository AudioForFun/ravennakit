#pragma once

#include "bonjour.hpp"

#if RAV_HAS_APPLE_DNSSD

namespace rav::dnssd {

/**
 * RAII wrapper around DNSServiceRef.
 */
class BonjourScopedDnsServiceRef {
  public:
    BonjourScopedDnsServiceRef() = default;
    ~BonjourScopedDnsServiceRef();

    explicit BonjourScopedDnsServiceRef(const DNSServiceRef& service_ref) noexcept;

    BonjourScopedDnsServiceRef(const BonjourScopedDnsServiceRef&) = delete;
    BonjourScopedDnsServiceRef& operator=(const BonjourScopedDnsServiceRef& other) = delete;

    BonjourScopedDnsServiceRef(BonjourScopedDnsServiceRef&& other) noexcept;
    BonjourScopedDnsServiceRef& operator=(BonjourScopedDnsServiceRef&& other) noexcept;

    /**
     * Assigns an existing DNSServiceRef to this instance. An existing DNSServiceRef will be deallocated, and this
     * object will take ownership over the given DNSServiceRef.
     * @param service_ref The DNSServiceRef to assign to this instance.
     * @return A reference to this instance.
     */
    BonjourScopedDnsServiceRef& operator=(DNSServiceRef service_ref);

    /**
     * @return Returns the contained DNSServiceRef.
     */
    [[nodiscard]] DNSServiceRef service_ref() const noexcept;

    /**
     * Resets the contained DNSServiceRef to nullptr.
     */
    void reset() noexcept;

  private:
    DNSServiceRef service_ref_ = nullptr;
};

}  // namespace rav::dnssd

#endif
