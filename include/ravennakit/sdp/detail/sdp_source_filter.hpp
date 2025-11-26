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

#include <string>
#include <vector>

namespace rav::sdp {

class SourceFilter {
  public:
    static constexpr auto k_attribute_name = "source-filter";

    FilterMode mode {FilterMode::undefined};
    NetwType net_type {NetwType::undefined};
    AddrType addr_type {AddrType::undefined};
    std::string dest_address;  // Must correspond to the address of a connection info field.
    std::vector<std::string> src_list;
};

/**
 * Converts the source filter to a string.
 * @returns The source filter as a string.
 */
[[nodiscard]] std::string to_string(const SourceFilter& filter);

/**
 * Parses a connection info field from a string.
 * @param line The string to parse.
 * @return A pair containing the parse result and the connection info. When parsing fails, the connection info
 * will be a default-constructed object.
 */
tl::expected<SourceFilter, std::string> parse_source_filter(std::string_view line);

/**
 * Validates the source filter.
 * @return An error message if the source filter is invalid.
 */
[[nodiscard]] tl::expected<void, std::string> validate(const SourceFilter& filter);

}  // namespace rav::sdp
