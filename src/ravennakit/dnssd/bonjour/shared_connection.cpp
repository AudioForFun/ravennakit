#include "ravennakit/dnssd/bonjour/shared_connection.hpp"
#include "ravennakit/dnssd/log.hpp"
#include "ravennakit/dnssd/result.hpp"
#include "ravennakit/dnssd/bonjour/bonjour.hpp"

dnssd::shared_connection::shared_connection() {
    DNSServiceRef ref = nullptr;
    auto result = dnssd::result(DNSServiceCreateConnection(&ref));
    if (result.has_error())
        DNSSD_LOG_DEBUG(error.description())
    else
        service_ref_ = ref;  // From here on the ref is under RAII inside a ScopedDnsServiceRef class
}
