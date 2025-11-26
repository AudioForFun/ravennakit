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

#include "detail/sdp_connection_info.hpp"
#include "detail/sdp_constants.hpp"
#include "detail/sdp_format.hpp"
#include "detail/sdp_media_clock_source.hpp"
#include "detail/sdp_ravenna_clock_domain.hpp"
#include "detail/sdp_reference_clock.hpp"
#include "detail/sdp_source_filter.hpp"
#include "ravennakit/core/string_parser.hpp"

#include <cstdint>
#include <string>
#include <map>

namespace rav::sdp {

/**
 * A type representing a media description (m=*) as part of an SDP session description.
 */
struct MediaDescription {
    std::string media_type;
    uint16_t port {};
    uint16_t number_of_ports {1};
    std::string protocol;
    std::vector<Format> formats;
    std::vector<ConnectionInfoField> connection_infos;
    std::optional<float> ptime;
    std::optional<float> max_ptime;
    std::optional<MediaDirection> media_direction;
    std::optional<ReferenceClock> reference_clock;
    std::optional<MediaClockSource> media_clock;
    std::optional<std::string> session_information;
    std::optional<RavennaClockDomain> ravenna_clock_domain;     // RAVENNA-specific attribute
    std::optional<uint32_t> ravenna_sync_time;                  // RAVENNA-specific attribute
    std::optional<Fraction<uint32_t>> ravenna_clock_deviation;  // RAVENNA-specific attribute
    std::vector<SourceFilter> source_filters;
    std::optional<uint16_t> ravenna_framecount;  // Legacy RAVENNA attribute, replaced by ptime
    std::optional<std::string> mid;
    std::map<std::string, std::string> attributes;  // Remaining, unknown attributes

    /**
     * Adds a format to the media description. If a format with the same payload type already exists, it will be
     * replaced.
     * @param format_to_add The format to add.
     */
    void add_or_update_format(const Format& format_to_add);

    /**
     * Adds a source filter to the session description. If the filter already exists, it will be replaced.
     * @param filter The source filter to add.
     */
    void add_or_update_source_filter(const SourceFilter& filter);

    /**
     * Parse an attribute from a string.
     * @param line The string to parse.
     * @return A result indicating success or failure. When parsing fails, the error message will contain a
     * description of the error.
     */
    tl::expected<void, std::string> parse_attribute(std::string_view line);
};

/**
 * Validates the media description.
 * @return An error message if the media description is invalid.
 */
[[nodiscard]] tl::expected<void, std::string> validate(const MediaDescription& media);

/**
 * Converts the media description to a string.
 * @return The media description as a string.
 */
[[nodiscard]] std::string to_string(const MediaDescription& media_description, const char* newline = k_sdp_crlf);

/**
 * Parses a media description from a string (i.e. the line starting with m=*). Does not parse the connection
 * into or attributes.
 * @param line The string to parse.
 * @returns A result indicating success or failure. When parsing fails, the error message will contain a
 * description of the error.
 */
tl::expected<MediaDescription, std::string> parse_media_description(std::string_view line);

}  // namespace rav::sdp
