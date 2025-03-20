#include "ravennakit/dnssd/bonjour/bonjour.hpp"

#if RAV_HAS_APPLE_DNSSD

    #include "ravennakit/dnssd/bonjour/bonjour_scoped_dns_service_ref.hpp"

    #include <utility>

rav::dnssd::BonjourScopedDnsServiceRef::~BonjourScopedDnsServiceRef() {
    reset();
}

rav::dnssd::BonjourScopedDnsServiceRef::BonjourScopedDnsServiceRef(BonjourScopedDnsServiceRef&& other
) noexcept {
    *this = std::move(other);
}

rav::dnssd::BonjourScopedDnsServiceRef::BonjourScopedDnsServiceRef(const DNSServiceRef& service_ref) noexcept :
    service_ref_(service_ref) {}

rav::dnssd::BonjourScopedDnsServiceRef&
rav::dnssd::BonjourScopedDnsServiceRef::operator=(BonjourScopedDnsServiceRef&& other) noexcept {
    reset();
    service_ref_ = other.service_ref_;
    other.service_ref_ = nullptr;
    return *this;
}

rav::dnssd::BonjourScopedDnsServiceRef&
rav::dnssd::BonjourScopedDnsServiceRef::operator=(DNSServiceRef service_ref) {
    reset();
    service_ref_ = service_ref;
    return *this;
}

DNSServiceRef rav::dnssd::BonjourScopedDnsServiceRef::service_ref() const noexcept {
    return service_ref_;
}

void rav::dnssd::BonjourScopedDnsServiceRef::reset() noexcept {
    if (service_ref_ != nullptr) {
        DNSServiceRefDeallocate(service_ref_);
        service_ref_ = nullptr;
    }
}

#endif
