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
#include <ravennakit/ntp/ntp_timestamp.hpp>

TEST_CASE("rav::ntp::Timestamp") {
    SECTION("timestamp::timestamp()") {
        const rav::ntp::Timestamp timestamp {0x01234567, 0x89abcdef};

        REQUIRE(timestamp.integer() == 0x01234567);
        REQUIRE(timestamp.fraction() == 0x89abcdef);
    }

    SECTION("timestamp::from_compact(uint16, uint16)") {
        const auto ts = rav::ntp::Timestamp::from_compact(0x0123, 0x4567);

        REQUIRE(ts.integer() == 0x0123);
        REQUIRE(ts.fraction() == 0x45670000);
    }

    SECTION("timestamp::from_compact(uint32)") {
        const auto ts = rav::ntp::Timestamp::from_compact(0x01234567);

        REQUIRE(ts.integer() == 0x0123);
        REQUIRE(ts.fraction() == 0x45670000);
    }

    SECTION("timestamp::operator==()") {
        SECTION("Equal") {
            const rav::ntp::Timestamp ts1 {0x01234567, 0x89abcdef};
            const rav::ntp::Timestamp ts2 {0x01234567, 0x89abcdef};
            REQUIRE(ts1 == ts2);
            REQUIRE_FALSE(ts1 != ts2);
        }

        SECTION("Not equal 1") {
            const rav::ntp::Timestamp ts1 {0x01234568, 0x89abcdef};
            const rav::ntp::Timestamp ts2 {0x01234567, 0x89abcdef};
            REQUIRE(ts1 != ts2);
            REQUIRE_FALSE(ts1 == ts2);
        }

        SECTION("Not equal 2") {
            const rav::ntp::Timestamp ts1 {0x01234567, 0x89abcdee};
            const rav::ntp::Timestamp ts2 {0x01234567, 0x89abcdef};
            REQUIRE(ts1 != ts2);
            REQUIRE_FALSE(ts1 == ts2);
        }
    }
}
