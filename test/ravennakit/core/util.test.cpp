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

#include <catch2/catch_test_macros.hpp>

TEST_CASE("rav utils") {
    SECTION("Test int buffer") {
        int data[] = {1, 2, 3, 4, 5};
        REQUIRE(rav::num_elements_in_array(data) == 5);
    }

    SECTION("Test char buffer") {
        char data[] = {1, 2, 3, 4, 5};
        REQUIRE(rav::num_elements_in_array(data) == 5);
    }

    SECTION("is_within()") {
        REQUIRE(rav::is_within(1.0, 1.0, 0.0));
        REQUIRE_FALSE(rav::is_within(1.0, 1.000001, 0.0));

        REQUIRE(rav::is_within(1, 1, 0));
        REQUIRE_FALSE(rav::is_within(1, 2, 0));
    }
}
