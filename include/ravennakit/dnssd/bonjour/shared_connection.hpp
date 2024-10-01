#pragma once

#include "scoped_dns_service_ref.hpp"
#include "bonjour.hpp"

namespace rav::dnssd
{

/**
 * Represents a shared connection to the mdns responder.
 */
class shared_connection
{
public:
    /**
     * Constructor which will create a connection and store the DNSServiceRef under RAII fashion.
     */
    shared_connection();

    /**
     * @return Returns the DNSServiceRef held by this instance. The DNSServiceRef will still be owned by this class.
     */
    [[nodiscard]] DNSServiceRef service_ref() const noexcept { return service_ref_.service_ref(); }

private:
    scoped_dns_service_ref service_ref_;
};

} // namespace dnssd
