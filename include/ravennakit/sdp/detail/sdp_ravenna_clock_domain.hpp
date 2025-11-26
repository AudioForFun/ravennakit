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
 * Defines a clock source and domain. This is a RAVENNA-specific attribute extension to the SDP specification.
 */
struct RavennaClockDomain {
    static constexpr auto k_attribute_name = "clock-domain";
    enum class SyncSource { undefined, ptp_v2 };

    SyncSource source {SyncSource::undefined};
    int32_t domain {};
};

/**
 * Parses a new instance of this structure from a string.
 * @param line The string to parse.
 * @return A parse result.
 */
[[nodiscard]] tl::expected<RavennaClockDomain, std::string> parse_ravenna_clock_domain(std::string_view line);

/**
 * @param source The sync source to convert.
 * @returns A string representation of the sync source.
 */
[[nodiscard]] const char* to_string(RavennaClockDomain::SyncSource source);

/**
 * @param ravenna_clock_domain The ravenna clock domain.
 * @return The string representation of this structure.
 */
[[nodiscard]] std::string to_string(const RavennaClockDomain& ravenna_clock_domain);

/**
 * Validates the values of this structure.
 * @returns An error message if the values are invalid.
 */
[[nodiscard]] tl::expected<void, std::string> validate(const RavennaClockDomain& ravenna_clock_domain);

}  // namespace rav::sdp
