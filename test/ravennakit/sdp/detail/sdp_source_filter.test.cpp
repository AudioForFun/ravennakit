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

#include "ravennakit/sdp/detail/sdp_source_filter.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::sdp::SourceFilter") {
    SECTION("Example 1") {
        auto filter = rav::sdp::parse_source_filter(" incl IN IP4 232.3.4.5 192.0.2.10");
        REQUIRE(filter);
        REQUIRE(filter->mode == rav::sdp::FilterMode::include);
        REQUIRE(filter->net_type == rav::sdp::NetwType::internet);
        REQUIRE(filter->addr_type == rav::sdp::AddrType::ipv4);
        REQUIRE(filter->dest_address == "232.3.4.5");
        const auto& src_list = filter->src_list;
        REQUIRE(src_list.size() == 1);
        REQUIRE(src_list.front() == "192.0.2.10");
    }

    SECTION("Example 2") {
        auto filter = rav::sdp::parse_source_filter(" excl IN IP4 192.0.2.11 192.0.2.10");
        REQUIRE(filter);
        REQUIRE(filter->mode == rav::sdp::FilterMode::exclude);
        REQUIRE(filter->net_type == rav::sdp::NetwType::internet);
        REQUIRE(filter->addr_type == rav::sdp::AddrType::ipv4);
        REQUIRE(filter->dest_address == "192.0.2.11");
        const auto& src_list = filter->src_list;
        REQUIRE(src_list.size() == 1);
        REQUIRE(src_list.front() == "192.0.2.10");
    }

    SECTION("Example 3") {
        auto filter = rav::sdp::parse_source_filter(" incl IN IP4 * 192.0.2.10");
        REQUIRE(filter);
        REQUIRE(filter->mode == rav::sdp::FilterMode::include);
        REQUIRE(filter->net_type == rav::sdp::NetwType::internet);
        REQUIRE(filter->addr_type == rav::sdp::AddrType::ipv4);
        REQUIRE(filter->dest_address == "*");
        const auto& src_list = filter->src_list;
        REQUIRE(src_list.size() == 1);
        REQUIRE(src_list.front() == "192.0.2.10");
    }

    SECTION("Example 4") {
        auto filter = rav::sdp::parse_source_filter(" incl IN IP6 FF0E::11A 2001:DB8:1:2:240:96FF:FE25:8EC9");
        REQUIRE(filter);
        REQUIRE(filter->mode == rav::sdp::FilterMode::include);
        REQUIRE(filter->net_type == rav::sdp::NetwType::internet);
        REQUIRE(filter->addr_type == rav::sdp::AddrType::ipv6);
        REQUIRE(filter->dest_address == "FF0E::11A");
        const auto& src_list = filter->src_list;
        REQUIRE(src_list.size() == 1);
        REQUIRE(src_list.front() == "2001:DB8:1:2:240:96FF:FE25:8EC9");
    }

    SECTION("Example 5") {
        auto filter = rav::sdp::parse_source_filter(" incl IN * dst-1.example.com src-1.example.com src-2.example.com");
        REQUIRE(filter);
        REQUIRE(filter->mode == rav::sdp::FilterMode::include);
        REQUIRE(filter->net_type == rav::sdp::NetwType::internet);
        REQUIRE(filter->addr_type == rav::sdp::AddrType::both);
        REQUIRE(filter->dest_address == "dst-1.example.com");
        const auto& src_list = filter->src_list;
        REQUIRE(src_list.size() == 2);
        REQUIRE(src_list.at(0) == "src-1.example.com");
        REQUIRE(src_list.at(1) == "src-2.example.com");
    }
}
