// SPDX-License-Identifier: AGPL-3.0-or-later
/*
 * Project: RAVENNAKIT (RAVENNA / AES67 / ST2110-30 SDK)
 * Copyright (c) 2024-2025 Sound on Digital
 *
 * This file is part of RAVENNAKIT.
 *
 * RAVENNAKIT is dual-licensed:
 *   1) Under the terms of the GNU Affero General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version (the "AGPL License"); and
 *   2) Under a commercial license from Sound on Digital, for customers who
 *      cannot (or do not wish to) comply with the AGPL License terms.
 *
 * If you obtained this file under the AGPL License, you may redistribute it
 * and/or modify it under the terms of the AGPL License. See the LICENSE
 * file in the project root for details.
 *
 * For commercial licensing, support, and other inquiries, please visit:
 *
 *     https://ravennakit.com
 *
 */

#include "ravennakit/dnssd/mock/dnssd_mock_browser.hpp"

#include "ravennakit/core/exception.hpp"
#include "ravennakit/core/string.hpp"

rav::dnssd::MockBrowser::MockBrowser(boost::asio::io_context& io_context) : io_context_(io_context) {}

void rav::dnssd::MockBrowser::mock_discovered_service(
    const std::string& fullname, const std::string& name, const std::string& reg_type, const std::string& domain
) {
    boost::asio::dispatch(io_context_, [=] {
        if (browsers_.find(reg_type) == browsers_.end()) {
            RAV_THROW_EXCEPTION("Not browsing for reg_type: {}", reg_type);
        }
        ServiceDescription service;
        service.fullname = fullname;
        service.name = name;
        service.reg_type = string_ends_with(reg_type, ".") ? reg_type : reg_type + ".";
        service.domain = string_ends_with(domain, ".") ? domain : domain + ".";
        const auto [it, inserted] = services_.emplace(fullname, service);
        on_service_discovered(it->second);
    });
}

void rav::dnssd::MockBrowser::mock_resolved_service(
    const std::string& fullname, const std::string& host_target, const uint16_t port, const TxtRecord& txt_record
) {
    boost::asio::dispatch(io_context_, [=] {
        const auto it = services_.find(fullname);
        if (it == services_.end()) {
            RAV_THROW_EXCEPTION("Service not discovered: {}", fullname);
        }
        it->second.host_target = host_target;
        it->second.port = port;
        it->second.txt = txt_record;
        on_service_resolved(it->second);
    });
}

void rav::dnssd::MockBrowser::mock_added_address(const std::string& fullname, const std::string& address, const uint32_t interface_index) {
    boost::asio::dispatch(io_context_, [=] {
        const auto it = services_.find(fullname);
        if (it == services_.end()) {
            RAV_THROW_EXCEPTION("Service not discovered: {}", fullname);
        }
        it->second.interfaces[interface_index].insert(address);
        on_address_added(it->second, address, interface_index);
    });
}

void rav::dnssd::MockBrowser::mock_removed_address(const std::string& fullname, const std::string& address, uint32_t interface_index) {
    boost::asio::dispatch(io_context_, [=] {
        const auto it = services_.find(fullname);
        if (it == services_.end()) {
            RAV_THROW_EXCEPTION("Service not discovered: {}", fullname);
        }
        const auto iface = it->second.interfaces.find(interface_index);
        if (iface == it->second.interfaces.end()) {
            RAV_THROW_EXCEPTION("Interface not found: {}", std::to_string(interface_index));
        }
        const auto addr = iface->second.find(address);
        if (addr == iface->second.end()) {
            RAV_THROW_EXCEPTION("Address not found: {}", address);
        }
        iface->second.erase(addr);
        if (iface->second.empty()) {
            it->second.interfaces.erase(iface);
        }
        on_address_removed(it->second, address, interface_index);
    });
}

void rav::dnssd::MockBrowser::mock_removed_service(const std::string& fullname) {
    boost::asio::dispatch(io_context_, [=] {
        const auto it = services_.find(fullname);
        if (it == services_.end()) {
            RAV_THROW_EXCEPTION("Service not discovered: {}", fullname);
        }
        on_service_removed(it->second);
        services_.erase(it);
    });
}

void rav::dnssd::MockBrowser::browse_for(const std::string& service_type) {
    auto [it, inserted] = browsers_.insert(service_type);
    if (!inserted) {
        RAV_THROW_EXCEPTION("Service type already being browsed for: {}", service_type);
    }
}

const rav::dnssd::ServiceDescription* rav::dnssd::MockBrowser::find_service(const std::string& service_name) const {
    for (auto& [_, service] : services_) {
        if (service.name == service_name) {
            return &service;
        }
    }
    return nullptr;
}

std::vector<rav::dnssd::ServiceDescription> rav::dnssd::MockBrowser::get_services() const {
    std::vector<ServiceDescription> result;
    result.reserve(services_.size());
    for (auto& [_, service] : services_) {
        result.push_back(service);
    }
    return result;
}
