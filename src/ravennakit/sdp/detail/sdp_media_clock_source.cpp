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

#include "ravennakit/core/log.hpp"
#include "ravennakit/core/string_parser.hpp"
#include "ravennakit/sdp/detail/sdp_media_clock_source.hpp"

const char* rav::sdp::to_string(const MediaClockSource::ClockMode mode) {
    switch (mode) {
        case MediaClockSource::ClockMode::undefined:
            return "undefined";
        case MediaClockSource::ClockMode::direct:
        default:
            return "direct";
    }
}

std::string rav::sdp::to_string(const MediaClockSource& mode) {
    auto txt = fmt::format("a={}:{}", MediaClockSource::k_attribute_name, to_string(mode.mode));
    if (mode.offset) {
        fmt::format_to(std::back_inserter(txt), "={}", *mode.offset);
    }
    if (mode.rate) {
        fmt::format_to(std::back_inserter(txt), " rate={}/{}", mode.rate->numerator, mode.rate->denominator);
    }
    return txt;
}

tl::expected<void, std::string> rav::sdp::validate(const MediaClockSource& clock_source) {
    if (clock_source.mode == MediaClockSource::ClockMode::undefined) {
        return tl::unexpected("media_clock: mode is undefined");
    }
    return {};
}

tl::expected<rav::sdp::MediaClockSource, std::string> rav::sdp::parse_media_clock_source(std::string_view line) {
    StringParser parser(line);

    MediaClockSource clock;

    if (const auto mode_part = parser.split(' ')) {
        StringParser mode_parser(*mode_part);

        if (const auto mode = mode_parser.split('=')) {
            if (mode == "direct") {
                clock.mode = MediaClockSource::ClockMode::direct;
            } else {
                RAV_LOG_WARNING("Unsupported media clock mode: {}", *mode);
                return tl::unexpected("media_clock: unsupported media clock mode");
            }
        } else {
            return tl::unexpected("media_clock: invalid media clock mode");
        }

        if (!mode_parser.exhausted()) {
            if (const auto offset = mode_parser.read_int<int64_t>()) {
                clock.offset = *offset;
            } else {
                return tl::unexpected("media_clock: invalid offset");
            }
        }
    }

    if (parser.exhausted()) {
        return clock;
    }

    if (const auto rate = parser.split('=')) {
        if (rate == "rate") {
            const auto numerator = parser.read_int<int32_t>();
            if (!numerator) {
                return tl::unexpected("media_clock: invalid rate numerator");
            }
            if (!parser.skip('/')) {
                return tl::unexpected("media_clock: invalid rate denominator");
            }
            const auto denominator = parser.read_int<int32_t>();
            if (!denominator) {
                return tl::unexpected("media_clock: invalid rate denominator");
            }
            clock.rate = Fraction<int32_t> {*numerator, *denominator};
        } else {
            return tl::unexpected("media_clock: unexpected token");
        }
    } else {
        return tl::unexpected("media_clock: expecting rate");
    }

    return clock;
}
