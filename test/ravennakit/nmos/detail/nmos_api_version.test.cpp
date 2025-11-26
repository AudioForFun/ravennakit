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

#include "ravennakit/nmos/detail/nmos_api_version.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::nmos::ApiVersion") {
    rav::nmos::ApiVersion version;

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

    SECTION("To string") {
        version = {0, 0};
        CHECK_FALSE(version.is_valid());
    }

    SECTION("To string") {
        version = {1, 0};
        CHECK(version.to_string() == "v1.0");
    }

    SECTION("To string with invalid version") {
        version = {0, 0};
        CHECK(version.to_string() == "v0.0");
    }

    SECTION("To string with negative version") {
        version = {-1, -1};
        CHECK(version.to_string() == "v-1.-1");
    }

    SECTION("To string with large version") {
        version = {1000, 2000};
        CHECK(version.to_string() == "v1000.2000");
    }

    SECTION("From v1.2") {
        auto v = rav::nmos::ApiVersion::from_string("v1.2");
        REQUIRE(v.has_value());
        CHECK(v->major == 1);
        CHECK(v->minor == 2);
    }

    SECTION("From v1.2 with leading spaces") {
        auto v = rav::nmos::ApiVersion::from_string(" v1.2");
        REQUIRE_FALSE(v.has_value());
    }

    SECTION("From v1.2 with trailing spaces") {
        auto v = rav::nmos::ApiVersion::from_string("v1.2 ");
        REQUIRE_FALSE(v.has_value());
    }

    SECTION("From incomplete") {
        auto v = rav::nmos::ApiVersion::from_string("v1.");
        CHECK_FALSE(v.has_value());

        v = rav::nmos::ApiVersion::from_string("v12");
        CHECK_FALSE(v.has_value());

        v = rav::nmos::ApiVersion::from_string("v.2");
        CHECK_FALSE(v.has_value());
    }
}
