#pragma once

#include "bonjour.hpp"

#if RAV_HAS_APPLE_DNSSD

    #include "bonjour_scoped_dns_service_ref.hpp"

namespace rav::dnssd {

/**
 * Represents a shared connection to the mdns responder.
 */
class BonjourSharedConnection {
  public:
    /**
     * Constructor which will create a connection and store the DNSServiceRef under RAII fashion.
     */
    BonjourSharedConnection();

    /**
     * @return Returns the DNSServiceRef held by this instance. The DNSServiceRef will still be owned by this class.
     */
    [[nodiscard]] DNSServiceRef service_ref() const noexcept {
        return service_ref_.service_ref();
    }

    /**
     * Resets the DNSServiceRef to nullptr.
     */
    void reset() noexcept {
        service_ref_.reset();
    }

  private:
    BonjourScopedDnsServiceRef service_ref_;
};

}  // namespace rav::dnssd

#endif
