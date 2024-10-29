/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/dnssd/mock/mock_browser.hpp"

void rav::dnssd::mock_browser::browse_for(const std::string& service_type) {}

const rav::dnssd::service_description* rav::dnssd::mock_browser::find_service(const std::string& service_name) const {
    return nullptr;
}

std::vector<rav::dnssd::service_description> rav::dnssd::mock_browser::get_services() const {
    return {};
}

void rav::dnssd::mock_browser::subscribe(subscriber& s) {}
