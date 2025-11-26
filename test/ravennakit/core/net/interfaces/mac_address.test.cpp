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

#include "ravennakit/core/net/interfaces/mac_address.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::MacAddress") {
    SECTION("Construct from string 1") {
        rav::MacAddress mac("00:01:02:03:04:05");
        auto bytes = mac.bytes();
        REQUIRE(bytes[0] == 0x00);
        REQUIRE(bytes[1] == 0x01);
        REQUIRE(bytes[2] == 0x02);
        REQUIRE(bytes[3] == 0x03);
        REQUIRE(bytes[4] == 0x04);
        REQUIRE(bytes[5] == 0x05);
    }

    SECTION("Construct from string 2") {
        rav::MacAddress mac("1a:2b:3c:d4:e5:e6");
        auto bytes = mac.bytes();
        REQUIRE(bytes[0] == 0x1a);
        REQUIRE(bytes[1] == 0x2b);
        REQUIRE(bytes[2] == 0x3c);
        REQUIRE(bytes[3] == 0xd4);
        REQUIRE(bytes[4] == 0xe5);
        REQUIRE(bytes[5] == 0xe6);
    }
}
