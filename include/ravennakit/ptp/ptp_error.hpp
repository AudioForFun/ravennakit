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

#pragma once

namespace rav::ptp {

enum class Error {
    invalid_data,
    invalid_header_length,
    invalid_message_length,
    only_ordinary_clock_supported,
    only_slave_supported,
    failed_to_get_network_interfaces,
    network_interface_not_found,
    no_mac_address_available,
    invalid_clock_identity,
    port_invalid,
    port_already_exists,
    too_many_ports,
};

inline const char* to_string(const Error error) {
    switch (error) {
        case Error::invalid_data:
            return "invalid data";
        case Error::invalid_header_length:
            return "invalid header length";
        case Error::invalid_message_length:
            return "invalid message length";
        case Error::only_ordinary_clock_supported:
            return "only ordinary clock supported";
        case Error::only_slave_supported:
            return "only slave supported";
        case Error::failed_to_get_network_interfaces:
            return "failed to get network interfaces";
        case Error::network_interface_not_found:
            return "network interface not found";
        case Error::no_mac_address_available:
            return "no MAC address available";
        case Error::invalid_clock_identity:
            return "invalid clock identity";
        case Error::port_invalid:
            return "port invalid";
        case Error::port_already_exists:
            return "port already exists";
        case Error::too_many_ports:
            return "too many ports";
        default:
            return "unknown error";
    }
}

}  // namespace rav::ptp
