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

#include "ravennakit/sdp/detail/sdp_time_active.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::sdp::TimeActiveField") {
    SECTION("Test time field") {
        auto time = rav::sdp::parse_time_active("t=123456789 987654321");
        REQUIRE(time);
        REQUIRE(time->start_time == 123456789);
        REQUIRE(time->stop_time == 987654321);
    }

    SECTION("Test invalid time field") {
        auto result = rav::sdp::parse_time_active("t=123456789 ");
        REQUIRE_FALSE(result);
    }

    SECTION("Test invalid time field") {
        auto result = rav::sdp::parse_time_active("t=");
        REQUIRE_FALSE(result);
    }
}
