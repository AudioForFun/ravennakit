/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/dnssd/mock/mock_advertiser.hpp"

rav::util::id rav::dnssd::mock_advertiser::register_service(
    const std::string& reg_type, const char* name, const char* domain, uint16_t port, const txt_record& txt_record,
    bool auto_rename, bool local_only
) {
    return util::id {};
}

void rav::dnssd::mock_advertiser::update_txt_record(util::id id, const txt_record& txt_record) {}

void rav::dnssd::mock_advertiser::unregister_service(util::id id) {}

void rav::dnssd::mock_advertiser::subscribe(subscriber& s) {}
