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

#include <map>

TEST_CASE("std::multimap") {
    SECTION("Test order") {
        std::multimap<int, int> map;
        map.insert({1,4});
        map.insert({1,3});
        map.insert({0,2});
        map.insert({-1,1});

        std::vector<std::pair<int, int>> kvs;
        for (const auto& [k, v] : map) {
            kvs.emplace_back(k, v);
        }

        REQUIRE(kvs[0] == std::pair(-1,1));
        REQUIRE(kvs[1] == std::pair(0,2));
        REQUIRE(kvs[2] == std::pair(1,4));
        REQUIRE(kvs[3] == std::pair(1,3));
    }
}
