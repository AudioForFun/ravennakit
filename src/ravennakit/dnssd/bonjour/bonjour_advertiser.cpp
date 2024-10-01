#include "ravennakit/dnssd/bonjour/bonjour_advertiser.hpp"
#include "ravennakit/dnssd/result.hpp"
#include "ravennakit/dnssd/service_description.hpp"
#include "ravennakit/dnssd/bonjour/bonjour_txt_record.hpp"

#include <iostream>
#include <thread>

rav::dnssd::result rav::dnssd::bonjour_advertiser::register_service(
    const std::string& reg_type, const char* name, const char* domain, uint16_t port, const txt_record& txt_record
) noexcept {
    DNSServiceRef serviceRef = nullptr;
    auto record = bonjour_txt_record(txt_record);

    auto r = result(DNSServiceRegister(
        &serviceRef, 0, 0, name, reg_type.c_str(), domain, nullptr, htons(port), record.length(), record.bytesPtr(),
        registerServiceCallBack, this
    ));

    if (r.has_error())
        return r;

    service_ref_ = serviceRef;

    return result(DNSServiceProcessResult(service_ref_.service_ref()));
}

void rav::dnssd::bonjour_advertiser::unregister_service() noexcept {
    service_ref_ = nullptr;
}

void rav::dnssd::bonjour_advertiser::registerServiceCallBack(
    DNSServiceRef service_ref, DNSServiceFlags flags, DNSServiceErrorType error_code, const char* service_name,
    const char* reg_type, const char* reply_domain, void* context
) {
    (void)service_ref;
    (void)flags;
    (void)service_name;
    (void)reg_type;
    (void)reply_domain;

    result const result(error_code);

    if (result.has_error()) {
        auto* owner = static_cast<bonjour_advertiser*>(context);
        if (owner->onAdvertiserErrorCallback)
            owner->onAdvertiserErrorCallback(result);
        owner->unregister_service();
    }
}

rav::dnssd::result rav::dnssd::bonjour_advertiser::update_txt_record(const txt_record& txt_record) {
    auto const record = bonjour_txt_record(txt_record);

    // Second argument's nullptr tells us that we are updating the primary record.
    return result(DNSServiceUpdateRecord(service_ref_.service_ref(), nullptr, 0, record.length(), record.bytesPtr(), 0)
    );
}
