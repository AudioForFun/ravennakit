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

#include "ravennakit/sdp/detail/sdp_group.hpp"

#include "ravennakit/core/string_parser.hpp"
#include "ravennakit/core/support.hpp"

#include <fmt/ranges.h>

tl::expected<rav::sdp::Group, std::string> rav::sdp::parse_group(const std::string_view line) {
    StringParser parser(line);

    const auto type = parser.read_until(' ');
    if (!type) {
        return tl::unexpected("Invalid group type");
    }

    if (*type != "DUP") {
        return tl::unexpected(fmt::format("Unsupported group type ({})"));
    }

    Group group;
    group.type = Group::Type::dup;

    for (auto tag = parser.split(' '); tag.has_value(); tag = parser.split(' ')) {
        group.tags.emplace_back(*tag);
    }

    return group;
}

std::string rav::sdp::to_string(const Group& input) {
    return fmt::format("a=group:DUP {}", fmt::join(input.tags, " "));
}
