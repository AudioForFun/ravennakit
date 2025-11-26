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

#include "ravennakit/ptp/types/ptp_clock_identity.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::ptp::ClockIdentity") {
    SECTION("Construct from MAC address") {
        const rav::MacAddress mac_address("a1:b2:c3:d4:e5:f6");
        const auto clock_identity = rav::ptp::ClockIdentity::from_mac_address(mac_address);

        REQUIRE(clock_identity.has_value());
        REQUIRE(clock_identity->data[0] == 0xa1);
        REQUIRE(clock_identity->data[1] == 0xb2);
        REQUIRE(clock_identity->data[2] == 0xc3);
        REQUIRE(clock_identity->data[3] == 0xd4);
        REQUIRE(clock_identity->data[4] == 0xe5);
        REQUIRE(clock_identity->data[5] == 0xf6);
        REQUIRE(clock_identity->data[6] == rav::ptp::ClockIdentity::k_implementer_specific_octets[0]);
        REQUIRE(clock_identity->data[7] == rav::ptp::ClockIdentity::k_implementer_specific_octets[1]);
    }

    SECTION("Default constructor") {
        constexpr rav::ptp::ClockIdentity clock_identity;
        REQUIRE(clock_identity.all_zero());
    }

    SECTION("Empty") {
        rav::ptp::ClockIdentity clock_identity;
        REQUIRE(clock_identity.all_zero());

        for (unsigned char& i : clock_identity.data) {
            SECTION("Test every byte") {
                i = 1;
                REQUIRE_FALSE(clock_identity.all_zero());
            }
        }
    }

    SECTION("Comparison") {
        rav::ptp::ClockIdentity a;
        rav::ptp::ClockIdentity b;

        SECTION("Equal") {
            REQUIRE(a == b);
            REQUIRE_FALSE(a < b);
            REQUIRE_FALSE(a > b);
        }

        SECTION("a < b") {
            b.data[0] = 1;
            REQUIRE(a < b);
            REQUIRE(a != b);
        }

        SECTION("a > b") {
            a.data[0] = 1;
            REQUIRE(a > b);
            REQUIRE(a != b);
        }

        SECTION("a < b") {
            b.data[7] = 1;
            REQUIRE(a < b);
            REQUIRE(a != b);
        }

        SECTION("a > b") {
            a.data[7] = 1;
            REQUIRE(a > b);
            REQUIRE(a != b);
        }
    }
}
