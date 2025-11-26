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

#include "ravennakit/aes67/aes67_packet_time.hpp"
#include "ravennakit/core/util.hpp"
#include "aes67_packet_time.test.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::aes67::PacketTime") {
    constexpr float eps = 0.005f;
    SECTION("125 microseconds") {
        const auto pt = rav::aes67::PacketTime::us_125();
        REQUIRE_THAT(pt.signaled_ptime(44'100), Catch::Matchers::WithinRel(0.136f, eps));
        REQUIRE_THAT(pt.signaled_ptime(48'000), Catch::Matchers::WithinRel(0.125f, eps));
        REQUIRE_THAT(pt.signaled_ptime(88'200), Catch::Matchers::WithinRel(0.136f, eps));
        REQUIRE_THAT(pt.signaled_ptime(96'000), Catch::Matchers::WithinRel(0.125f, eps));
        REQUIRE_THAT(pt.signaled_ptime(192'000), Catch::Matchers::WithinRel(0.125f, eps));
        REQUIRE_THAT(pt.signaled_ptime(384'000), Catch::Matchers::WithinRel(0.125f, eps));

        REQUIRE(pt.framecount(44'100) == 6);
        REQUIRE(pt.framecount(48'000) == 6);
        REQUIRE(pt.framecount(88'200) == 12);
        REQUIRE(pt.framecount(96'000) == 12);
        REQUIRE(pt.framecount(192'000) == 24);
        REQUIRE(pt.framecount(384'000) == 48);
    }

    SECTION("250 microseconds") {
        const auto pt = rav::aes67::PacketTime::us_250();
        REQUIRE_THAT(pt.signaled_ptime(44'100), Catch::Matchers::WithinRel(0.272f, eps));
        REQUIRE_THAT(pt.signaled_ptime(48'000), Catch::Matchers::WithinRel(0.250f, eps));
        REQUIRE_THAT(pt.signaled_ptime(88'200), Catch::Matchers::WithinRel(0.272f, eps));
        REQUIRE_THAT(pt.signaled_ptime(96'000), Catch::Matchers::WithinRel(0.250f, eps));
        REQUIRE_THAT(pt.signaled_ptime(192'000), Catch::Matchers::WithinRel(0.250f, eps));
        REQUIRE_THAT(pt.signaled_ptime(384'000), Catch::Matchers::WithinRel(0.250f, eps));

        REQUIRE(pt.framecount(44'100) == 12);
        REQUIRE(pt.framecount(48'000) == 12);
        REQUIRE(pt.framecount(88'200) == 24);
        REQUIRE(pt.framecount(96'000) == 24);
        REQUIRE(pt.framecount(192'000) == 48);
        REQUIRE(pt.framecount(384'000) == 96);
    }

    SECTION("333 microseconds") {
        const auto pt = rav::aes67::PacketTime::us_333();
        REQUIRE_THAT(pt.signaled_ptime(44'100), Catch::Matchers::WithinRel(0.363f, eps));
        REQUIRE_THAT(pt.signaled_ptime(48'000), Catch::Matchers::WithinRel(0.333f, eps));
        REQUIRE_THAT(pt.signaled_ptime(88'200), Catch::Matchers::WithinRel(0.363f, eps));
        REQUIRE_THAT(pt.signaled_ptime(96'000), Catch::Matchers::WithinRel(0.333f, eps));
        REQUIRE_THAT(pt.signaled_ptime(192'000), Catch::Matchers::WithinRel(0.333f, eps));
        REQUIRE_THAT(pt.signaled_ptime(384'000), Catch::Matchers::WithinRel(0.333f, eps));

        REQUIRE(pt.framecount(44'100) == 16);
        REQUIRE(pt.framecount(48'000) == 16);
        REQUIRE(pt.framecount(88'200) == 32);
        REQUIRE(pt.framecount(96'000) == 32);
        REQUIRE(pt.framecount(192'000) == 64);
        REQUIRE(pt.framecount(384'000) == 128);
    }

    SECTION("1 millisecond") {
        const auto pt = rav::aes67::PacketTime::ms_1();
        REQUIRE_THAT(pt.signaled_ptime(44'100), Catch::Matchers::WithinRel(1.088435411f, eps));
        REQUIRE_THAT(pt.signaled_ptime(48'000), Catch::Matchers::WithinRel(1.0f, eps));
        REQUIRE_THAT(pt.signaled_ptime(88'200), Catch::Matchers::WithinRel(1.088435411f, eps));
        REQUIRE_THAT(pt.signaled_ptime(96'000), Catch::Matchers::WithinRel(1.0f, eps));
        REQUIRE_THAT(pt.signaled_ptime(192'000), Catch::Matchers::WithinRel(1.0f, eps));
        REQUIRE_THAT(pt.signaled_ptime(384'000), Catch::Matchers::WithinRel(1.0f, eps));

        REQUIRE(pt.framecount(44'100) == 48);
        REQUIRE(pt.framecount(48'000) == 48);
        REQUIRE(pt.framecount(88'200) == 96);
        REQUIRE(pt.framecount(96'000) == 96);
        REQUIRE(pt.framecount(192'000) == 192);
        REQUIRE(pt.framecount(384'000) == 384);
    }

    SECTION("4 milliseconds") {
        const auto pt = rav::aes67::PacketTime::ms_4();
        REQUIRE_THAT(pt.signaled_ptime(44'100), Catch::Matchers::WithinRel(4.354f, eps));
        REQUIRE_THAT(pt.signaled_ptime(48'000), Catch::Matchers::WithinRel(4.f, eps));
        REQUIRE_THAT(pt.signaled_ptime(88'200), Catch::Matchers::WithinRel(4.354f, eps));
        REQUIRE_THAT(pt.signaled_ptime(96'000), Catch::Matchers::WithinRel(4.f, eps));
        REQUIRE_THAT(pt.signaled_ptime(192'000), Catch::Matchers::WithinRel(4.f, eps));
        REQUIRE_THAT(pt.signaled_ptime(384'000), Catch::Matchers::WithinRel(4.f, eps));

        REQUIRE(pt.framecount(44'100) == 192);
        REQUIRE(pt.framecount(48'000) == 192);
        REQUIRE(pt.framecount(88'200) == 384);
        REQUIRE(pt.framecount(96'000) == 384);
        REQUIRE(pt.framecount(192'000) == 768);
        REQUIRE(pt.framecount(384'000) == 1536);
    }

    SECTION("To JSON") {
        auto test_packet_time = [](const rav::aes67::PacketTime& packet_time) {
            rav::aes67::test_packet_time_json(packet_time, boost::json::value_from(packet_time));
        };

        test_packet_time(rav::aes67::PacketTime::us_125());
        test_packet_time(rav::aes67::PacketTime::us_250());
        test_packet_time(rav::aes67::PacketTime::us_333());
        test_packet_time(rav::aes67::PacketTime::ms_1());
        test_packet_time(rav::aes67::PacketTime::ms_4());
    }
}

void rav::aes67::test_packet_time_json(const PacketTime& packet_time, const boost::json::value& json) {
    REQUIRE(json.at(0) == packet_time.fraction.numerator);
    REQUIRE(json.at(1) == packet_time.fraction.denominator);
}
