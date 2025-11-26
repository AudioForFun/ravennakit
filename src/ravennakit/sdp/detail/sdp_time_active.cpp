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

#include "ravennakit/sdp/detail/sdp_time_active.hpp"

#include "ravennakit/core/string_parser.hpp"

tl::expected<rav::sdp::TimeActiveField, std::string> rav::sdp::parse_time_active(std::string_view line) {
    StringParser parser(line);

    if (!parser.skip("t=")) {
        return tl::unexpected("time: expecting 't='");
    }

    TimeActiveField time;

    if (const auto start_time = parser.read_int<int64_t>()) {
        time.start_time = *start_time;
    } else {
        return tl::unexpected("time: failed to parse start time as integer");
    }

    if (!parser.skip(' ')) {
        return tl::unexpected("time: expecting space after start time");
    }

    if (const auto stop_time = parser.read_int<int64_t>()) {
        time.stop_time = *stop_time;
    } else {
        return tl::unexpected("time: failed to parse stop time as integer");
    }

    return time;
}

std::string rav::sdp::to_string(const TimeActiveField& time_active_field) {
    return fmt::format("t={} {}", time_active_field.start_time, time_active_field.stop_time);
}

tl::expected<void, std::string> rav::sdp::validate(const TimeActiveField& time_active_field) {
    if (time_active_field.start_time < 0) {
        return tl::unexpected("time: start time must be greater than or equal to 0");
    }
    if (time_active_field.stop_time < 0) {
        return tl::unexpected("time: stop time must be greater than or equal to 0");
    }
    return {};
}
