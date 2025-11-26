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

#include "ravennakit/sdp/detail/sdp_group.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::sdp::Group") {
    SECTION("Parse group line") {
        const auto group_line = "DUP primary secondary";
        auto group = rav::sdp::parse_group(group_line);
        REQUIRE(group);
        REQUIRE(group->type == rav::sdp::Group::Type::dup);
        REQUIRE(group->tags.size() == 2);
        REQUIRE(group->tags[0] == "primary");
        REQUIRE(group->tags[1] == "secondary");
    }

    SECTION("Parse group of three") {
        const auto group_line = "DUP primary secondary tertiary";
        auto group = rav::sdp::parse_group(group_line);
        REQUIRE(group);
        REQUIRE(group->type == rav::sdp::Group::Type::dup);
        auto tags = group->tags;
        REQUIRE(tags.size() == 3);
        REQUIRE(tags[0] == "primary");
        REQUIRE(tags[1] == "secondary");
        REQUIRE(tags[2] == "tertiary");
    }

    SECTION("To string") {
        rav::sdp::Group group;
        group.type = rav::sdp::Group::Type::dup;
        group.tags.push_back("primary");
        group.tags.push_back("secondary");
        REQUIRE(rav::sdp::to_string(group) == "a=group:DUP primary secondary");
    }
}
