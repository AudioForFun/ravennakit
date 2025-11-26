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

#include "sdp_types.hpp"
#include "ravennakit/core/expected.hpp"

namespace rav::sdp {

/**
 * A type representing the connection information (c=*) of an SDP session description.
 */
struct ConnectionInfoField {
    /// Specifies the type of network.
    NetwType network_type {NetwType::undefined};
    /// Specifies the type of address.
    AddrType address_type {AddrType::undefined};
    /// The address at which the media can be found.
    std::string address;
    /// Optional ttl
    std::optional<int32_t> ttl;
    /// Optional number of addresses
    std::optional<int32_t> number_of_addresses;
};

/**
 * Parses a connection info field from a string.
 * @param line The string to parse.
 * @return A result object containing either the newly parsed value, or an error string.
 */
tl::expected<ConnectionInfoField, std::string> parse_connection_info(std::string_view line);

/**
 * Converts the connection info to a string.
 * @return The connection info as a string.
 */
std::string to_string(const ConnectionInfoField& field);

/**
 * Validates the connection info.
 * @return An error message if the connection info is invalid.
 */
[[nodiscard]] tl::expected<void, std::string> validate(const ConnectionInfoField& field);

}  // namespace rav::sdp
