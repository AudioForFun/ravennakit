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

#include "ravennakit/sdp/detail/sdp_connection_info.hpp"

#include "ravennakit/core/string_parser.hpp"
#include "ravennakit/sdp/detail/sdp_constants.hpp"

tl::expected<rav::sdp::ConnectionInfoField, std::string> rav::sdp::parse_connection_info(const std::string_view line) {
    StringParser parser(line);

    if (!parser.skip("c=")) {
        return tl::unexpected("connection: expecting 'c='");
    }

    ConnectionInfoField info;

    // Network type
    if (const auto network_type = parser.split(' ')) {
        if (*network_type == k_sdp_inet) {
            info.network_type = NetwType::internet;
        } else {
            return tl::unexpected("connection: invalid network type");
        }
    } else {
        return tl::unexpected("connection: failed to parse network type");
    }

    // Address type
    if (const auto address_type = parser.split(' ')) {
        if (*address_type == k_sdp_ipv4) {
            info.address_type = AddrType::ipv4;
        } else if (*address_type == k_sdp_ipv6) {
            info.address_type = AddrType::ipv6;
        } else {
            return tl::unexpected("connection: invalid address type");
        }
    } else {
        return tl::unexpected("connection: failed to parse address type");
    }

    // Address
    if (const auto address = parser.split('/')) {
        info.address = *address;
    }

    if (parser.exhausted()) {
        return info;
    }

    // Parse optional ttl and number of addresses
    if (info.address_type == AddrType::ipv4) {
        if (auto ttl = parser.read_int<int32_t>()) {
            info.ttl = *ttl;
        } else {
            return tl::unexpected("connection: failed to parse ttl for ipv4 address");
        }
        if (parser.skip('/')) {
            if (auto num_addresses = parser.read_int<int32_t>()) {
                info.number_of_addresses = *num_addresses;
            } else {
                return tl::unexpected("connection: failed to parse number of addresses for ipv4 address");
            }
        }
    } else if (info.address_type == sdp::AddrType::ipv6) {
        if (auto num_addresses = parser.read_int<int32_t>()) {
            info.number_of_addresses = *num_addresses;
        } else {
            return tl::unexpected("connection: failed to parse number of addresses for ipv4 address");
        }
    }

    if (!parser.exhausted()) {
        return tl::unexpected("connection: unexpected characters at end of line");
    }

    return info;
}

std::string rav::sdp::to_string(const ConnectionInfoField& field) {
    return fmt::format(
        "c={} {} {}{}{}", to_string(field.network_type), to_string(field.address_type), field.address,
        field.ttl.has_value() ? "/" + std::to_string(field.ttl.value()) : "",
        field.number_of_addresses.has_value() ? "/" + std::to_string(field.number_of_addresses.value()) : ""
    );
}

tl::expected<void, std::string> rav::sdp::validate(const ConnectionInfoField& field) {
    if (field.network_type == NetwType::undefined) {
        return tl::unexpected("connection: network type is undefined");
    }
    if (field.address_type == AddrType::undefined) {
        return tl::unexpected("connection: address type is undefined");
    }
    if (field.address.empty()) {
        return tl::unexpected("connection: address is empty");
    }
    if (field.address_type == AddrType::ipv4) {
        if (!field.ttl.has_value()) {
            return tl::unexpected("connection: ttl is required for ipv4 address");
        }
    } else if (field.address_type == AddrType::ipv6) {
        if (field.ttl.has_value()) {
            return tl::unexpected("connection: ttl is not allowed for ipv6 address");
        }
    }
    return {};
}
