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

#include "ravennakit/core/math/fraction.hpp"
#include "ravennakit/core/expected.hpp"

#include <string_view>

namespace rav::sdp {

/**
 * The media clock source for a stream determines the timebase used to advance the RTP timestamps included in RTP
 * packets.
 * https://datatracker.ietf.org/doc/html/rfc7273#autoid-15
 */
class MediaClockSource {
  public:
    static constexpr auto k_attribute_name = "mediaclk";

    enum class ClockMode { undefined, direct };

    ClockMode mode {ClockMode::undefined};
    std::optional<int64_t> offset;
    std::optional<Fraction<int32_t>> rate;
};

/**
 * Converts the media clock source to a string.
 * @return The media clock source as a string.
 */
[[nodiscard]] std::string to_string(const MediaClockSource& mode);

/**
 * Converts the clock mode to a string.
 * @param mode The clock mode to convert.
 * @return The clock mode as a string.
 */
[[nodiscard]] const char* to_string(MediaClockSource::ClockMode mode);

/**
 * Validates the media clock source.
 * @throws rav::Exception if the media clock source is invalid.
 */
[[nodiscard]] tl::expected<void, std::string> validate(const MediaClockSource& clock_source);

/**
 * Create a MediaClockSource from a string from an SDP.
 * @param line The input text.
 * @return The MediaClockSource, or an error.
 */
[[nodiscard]] tl::expected<MediaClockSource, std::string> parse_media_clock_source(std::string_view line);

}  // namespace rav::sdp
