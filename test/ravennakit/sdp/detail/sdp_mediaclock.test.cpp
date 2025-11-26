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

#include <catch2/catch_all.hpp>

#include "ravennakit/sdp/detail/sdp_media_clock_source.hpp"

TEST_CASE("rav::sdp::MediaClockSource", "[media_clock]") {
    SECTION("Parse direct only") {
        constexpr auto line = "direct";
        const auto clock = rav::sdp::parse_media_clock_source(line);
        REQUIRE(clock);
        REQUIRE(clock->mode == rav::sdp::MediaClockSource::ClockMode::direct);
        REQUIRE_FALSE(clock->offset.has_value());
        REQUIRE_FALSE(clock->rate.has_value());
    }

    SECTION("Parse direct with offset") {
        constexpr auto line = "direct=555";
        const auto clock = rav::sdp::parse_media_clock_source(line);
        REQUIRE(clock);
        REQUIRE(clock->mode == rav::sdp::MediaClockSource::ClockMode::direct);
        REQUIRE(clock->offset.value() == 555);
        REQUIRE_FALSE(clock->rate.has_value());
    }

    SECTION("Parse direct with offset and rate") {
        constexpr auto line = "direct=555 rate=48000/1";
        const auto clock = rav::sdp::parse_media_clock_source(line);
        REQUIRE(clock);
        REQUIRE(clock->mode == rav::sdp::MediaClockSource::ClockMode::direct);
        REQUIRE(clock->offset.value() == 555);
        REQUIRE(clock->rate.value().numerator == 48000);
        REQUIRE(clock->rate.value().denominator == 1);
    }

    SECTION("Parse direct without offset and rate") {
        constexpr auto line = "direct rate=48000/1";
        const auto clock = rav::sdp::parse_media_clock_source(line);
        REQUIRE(clock);
        REQUIRE(clock->mode == rav::sdp::MediaClockSource::ClockMode::direct);
        REQUIRE_FALSE(clock->offset.has_value());
        REQUIRE(clock->rate.value().numerator == 48000);
        REQUIRE(clock->rate.value().denominator == 1);
    }
}
