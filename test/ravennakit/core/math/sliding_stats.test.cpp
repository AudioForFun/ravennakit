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

#include "ravennakit/core/util.hpp"
#include "ravennakit/core/math/sliding_stats.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::SlidingStats") {
    SECTION("average") {
        rav::SlidingStats avg(5);
        REQUIRE(avg.count() == 0);
        REQUIRE(rav::is_within(avg.mean(), 0.0, 0.0));
        avg.add(1);
        avg.add(2);
        avg.add(3);
        avg.add(4);
        avg.add(5);
        REQUIRE(avg.count() == 5);
        REQUIRE(rav::is_within(avg.mean(), 3.0, 0.0));
        avg.add(6);
        REQUIRE(avg.count() == 5);
        REQUIRE(rav::is_within(avg.mean(), 4.0, 0.0));
        avg.add(7);
        REQUIRE(avg.count() == 5);
        REQUIRE(rav::is_within(avg.mean(), 5.0, 0.0));
        avg.reset();
        REQUIRE(avg.count() == 0);
        REQUIRE(rav::is_within(avg.mean(), 0.0, 0.0));
    }

    SECTION("median") {
        rav::SlidingStats stats(5);
        REQUIRE(stats.count() == 0);
        REQUIRE(rav::is_within(stats.median(), 0.0, 0.0));
        stats.add(1);
        REQUIRE(stats.count() == 1);
        REQUIRE(rav::is_within(stats.median(), 1.0, 0.0));
        stats.add(500);
        stats.add(4);
        stats.add(3);
        REQUIRE(stats.count() == 4);
        REQUIRE(rav::is_within(stats.median(), 3.5, 0.0));
        stats.add(2);
        REQUIRE(stats.count() == 5);
        REQUIRE(rav::is_within(stats.median(), 3.0, 0.0));
    }
}
