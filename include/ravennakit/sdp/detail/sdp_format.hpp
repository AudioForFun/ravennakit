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

#include "ravennakit/core/audio/audio_format.hpp"

#include <string>

namespace rav::sdp {

/**
 * Holds the information of an RTP map.
 */
struct Format {
    uint8_t payload_type {};
    std::string encoding_name;
    uint32_t clock_rate {};
    uint32_t num_channels {};
};

/**
 * Parses a format from a string.
 * @param line The string to parse.
 * @return A result indicating success or failure. When parsing fails, the error message will contain a
 * description of what went wrong.
 */
[[nodiscard]] tl::expected<Format, std::string> parse_format(std::string_view line);

/**
 * @return The format as audio_format, or nullopt if the format is not supported or cannot be converted.
 */
[[nodiscard]] std::optional<Format> make_audio_format(const AudioFormat& input_format);

/**
 * @return The format as audio_format, or nullopt if the format is not supported or cannot be converted.
 */
[[nodiscard]] std::optional<AudioFormat> make_audio_format(const Format& input_format);

/**
 * Converts a Format to an SDP compatible string.
 * @param input_format The input format.
 * @return The encoded string.
 */
[[nodiscard]] std::string to_string(const Format& input_format);

}  // namespace rav::sdp
