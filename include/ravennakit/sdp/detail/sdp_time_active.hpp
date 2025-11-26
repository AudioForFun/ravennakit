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

#include <string_view>

namespace rav::sdp {

/**
 * A type representing the time field (t=*) of an SDP session description.
 * Defined as seconds since January 1, 1900, UTC.
 */
struct TimeActiveField {
    /// The start time of the session.
    int64_t start_time {0};
    /// The stop time of the session.
    int64_t stop_time {0};
};

/**
 * Parses a time field from a string.
 * @param line The string to parse.
 * @return A pair containing the parse result and the time field.
 */
[[nodiscard]] tl::expected<TimeActiveField, std::string> parse_time_active(std::string_view line);

/**
 * Converts the time field to an SDP compatible string.
 * @return The time field as a string.
 */
[[nodiscard]] std::string to_string(const TimeActiveField& time_active_field);

/**
 * Validates the values of this structure.
 * @return A result indicating success or failure. When validation fails, the error message will contain a
 * description.
 */
[[nodiscard]] tl::expected<void, std::string> validate(const TimeActiveField& time_active_field);

}  // namespace rav::sdp
