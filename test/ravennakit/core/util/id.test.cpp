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

#include "ravennakit/core/util/id.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::Id", "[id]") {
    SECTION("Invalid id") {
        constexpr rav::Id invalid_id;
        REQUIRE_FALSE(invalid_id.is_valid());
    }

    SECTION("Invalid id 2") {
        rav::Id invalid_id(0);
        REQUIRE_FALSE(invalid_id.is_valid());
    }

    SECTION("Generator") {
        rav::Id::Generator gen;
        REQUIRE(gen.next() == 1);
        REQUIRE(gen.next() == 2);
        REQUIRE(gen.next() == 3);
    }

    SECTION("Process-wide id") {
        auto id = rav::Id::get_next_process_wide_unique_id();
        REQUIRE(rav::Id::get_next_process_wide_unique_id() == id.value() + 1);
        REQUIRE(rav::Id::get_next_process_wide_unique_id() == id.value() + 2);
        REQUIRE(rav::Id::get_next_process_wide_unique_id() == id.value() + 3);
    }
}
