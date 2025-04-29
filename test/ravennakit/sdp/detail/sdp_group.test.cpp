/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/sdp/detail/sdp_group.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("sdp group") {
    SECTION("Parse group line") {
        const auto group_line = "DUP primary secondary";
        auto group = rav::sdp::Group::parse_new(group_line);
        REQUIRE(group);
        REQUIRE(group->get_type() == rav::sdp::Group::Type::dup);
        auto tags = group->get_tags();
        REQUIRE(tags.size() == 2);
        REQUIRE(tags[0] == "primary");
        REQUIRE(tags[1] == "secondary");
    }

    SECTION("Parse group of three") {
        const auto group_line = "DUP primary secondary tertiary";
        auto group = rav::sdp::Group::parse_new(group_line);
        REQUIRE(group);
        REQUIRE(group->get_type() == rav::sdp::Group::Type::dup);
        auto tags = group->get_tags();
        REQUIRE(tags.size() == 3);
        REQUIRE(tags[0] == "primary");
        REQUIRE(tags[1] == "secondary");
        REQUIRE(tags[2] == "tertiary");
    }

    SECTION("To string") {
        rav::sdp::Group group;
        group.set_type(rav::sdp::Group::Type::dup);
        group.add_tag("primary");
        group.add_tag("secondary");
        auto group_str = group.to_string();
        REQUIRE(group_str.has_value());
        REQUIRE(group_str.value() == "a=group:DUP primary secondary");
    }
}
