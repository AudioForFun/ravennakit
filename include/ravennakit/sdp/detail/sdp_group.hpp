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

#include "ravennakit/core/expected.hpp"

#include <string>

namespace rav::sdp {

/**
 * Represents a group of media descriptions in an SDP message.
 * RFC 5888: https://datatracker.ietf.org/doc/html/rfc5888
 * RFC 7104: https://datatracker.ietf.org/doc/html/rfc7104
 */
class Group {
  public:
    enum class Type {
        undefined = 0,
        dup,
    };

    Type type = Type::undefined;
    std::vector<std::string> tags;
};

/**
 * Parses a group line from an SDP message.
 * @param line The line to parse.
 * @return A tl::expected object containing the parsed group or an error message.
 */
[[nodiscard]] tl::expected<Group, std::string> parse_group(std::string_view line);

/**
 * @return The group encoded as string.
 */
[[nodiscard]] std::string to_string(const Group& input);

}  // namespace rav::sdp
