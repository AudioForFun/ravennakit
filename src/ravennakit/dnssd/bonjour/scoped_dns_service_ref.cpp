#include "ravennakit/dnssd/bonjour/scoped_dns_service_ref.hpp"
#include "ravennakit/dnssd/bonjour/bonjour.hpp"

#include <utility>

rav::dnssd::scoped_dns_service_ref::~scoped_dns_service_ref() {
    if (service_ref_ != nullptr)
        DNSServiceRefDeallocate(service_ref_);
}

rav::dnssd::scoped_dns_service_ref::scoped_dns_service_ref(scoped_dns_service_ref&& other) noexcept {
    *this = std::move(other);
}

rav::dnssd::scoped_dns_service_ref::scoped_dns_service_ref(const DNSServiceRef& serviceRef) noexcept :
    service_ref_(serviceRef) {}

rav::dnssd::scoped_dns_service_ref& rav::dnssd::scoped_dns_service_ref::operator=(rav::dnssd::scoped_dns_service_ref&& other
) noexcept {
    if (service_ref_ != nullptr)
        DNSServiceRefDeallocate(service_ref_);

    service_ref_ = other.service_ref_;
    other.service_ref_ = nullptr;
    return *this;
}

rav::dnssd::scoped_dns_service_ref& rav::dnssd::scoped_dns_service_ref::operator=(DNSServiceRef serviceRef) {
    if (service_ref_ != nullptr)
        DNSServiceRefDeallocate(service_ref_);

    service_ref_ = serviceRef;
    return *this;
}
