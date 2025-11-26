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

#include "ravennakit/nmos/detail/nmos_timestamp.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::nmos::Version") {
    rav::nmos::Version version;

    SECTION("Default constructor") {
        CHECK_FALSE(version.is_valid());
    }

    SECTION("Valid version") {
        version = {1, 0};
        CHECK(version.is_valid());
    }

    SECTION("Invalid version") {
        version = {0, 0};
        CHECK_FALSE(version.is_valid());
    }

    SECTION("to_string") {
        version = {1439299836, 10};
        CHECK(version.to_string() == "1439299836:10");

        version = {0, 123456789};
        CHECK(version.to_string() == "0:123456789");
    }

    SECTION("From string") {
        auto v = rav::nmos::Version::from_string("1439299836:10");
        REQUIRE(v.has_value());
        CHECK(v->seconds == 1439299836);
        CHECK(v->nanoseconds == 10);

        // Leading whitespace
        v = rav::nmos::Version::from_string(" 1439299836:10");
        REQUIRE_FALSE(v.has_value());

        // Trailing whitespace
        v = rav::nmos::Version::from_string("1439299836:10 ");
        REQUIRE_FALSE(v.has_value());
    }
}
