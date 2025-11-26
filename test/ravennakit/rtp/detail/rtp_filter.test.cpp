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

#include "ravennakit/rtp/detail/rtp_filter.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::rtp::Filter") {
    const auto connection_address = boost::asio::ip::make_address("239.3.8.1");
    rav::rtp::Filter filter(connection_address);
    SECTION("matches") {
        REQUIRE(filter.connection_address() == connection_address);
    }

    SECTION("matches") {
        REQUIRE(filter.empty());
        filter.add_filter(boost::asio::ip::make_address("192.168.1.2"), rav::sdp::FilterMode::exclude);
        REQUIRE_FALSE(filter.empty());
    }

    SECTION("is_valid_source with no filters") {
        auto matches = filter.is_valid_source(connection_address, boost::asio::ip::make_address("127.0.0.1"));
        REQUIRE(matches);
    }

    SECTION("is_valid_source with invalid connection address") {
        auto matches = filter.is_valid_source(
            boost::asio::ip::make_address("192.168.1.2"), boost::asio::ip::make_address("127.0.0.1")
        );
        REQUIRE_FALSE(matches);
    }

    SECTION("is_valid_source with single exclude address") {
        filter.add_filter(boost::asio::ip::make_address("192.168.1.2"), rav::sdp::FilterMode::exclude);
        auto matches = filter.is_valid_source(connection_address, boost::asio::ip::make_address("127.0.0.1"));
        REQUIRE(matches);
        matches = filter.is_valid_source(connection_address, boost::asio::ip::make_address("192.168.1.2"));
        REQUIRE_FALSE(matches);
    }

    SECTION("is_valid_source with single include address") {
        filter.add_filter(boost::asio::ip::make_address("192.168.1.2"), rav::sdp::FilterMode::include);
        auto matches = filter.is_valid_source(connection_address, boost::asio::ip::make_address("127.0.0.1"));
        REQUIRE_FALSE(matches);
        matches = filter.is_valid_source(connection_address, boost::asio::ip::make_address("192.168.1.2"));
        REQUIRE(matches);
    }

    SECTION("add_filter with single include address") {
        auto src_filter = rav::sdp::parse_source_filter(" incl IN IP4 239.3.8.1 192.168.16.52");
        REQUIRE(src_filter);
        REQUIRE(filter.add_filter(*src_filter) == 1);
        REQUIRE_FALSE(filter.empty());
        REQUIRE(filter.connection_address() == boost::asio::ip::make_address("239.3.8.1"));
        REQUIRE(filter.is_valid_source(
            boost::asio::ip::make_address("239.3.8.1"), boost::asio::ip::make_address("192.168.16.52")
        ));
        REQUIRE_FALSE(filter.is_valid_source(
            boost::asio::ip::make_address("239.3.8.1"), boost::asio::ip::make_address("192.168.16.53")
        ));
    }

    SECTION("add_filter with single exclude address") {
        auto src_filter = rav::sdp::parse_source_filter(" excl IN IP4 239.3.8.1 192.168.16.52");
        REQUIRE(src_filter);
        REQUIRE(filter.add_filter(*src_filter) == 1);
        REQUIRE_FALSE(filter.empty());
        REQUIRE(filter.connection_address() == boost::asio::ip::make_address("239.3.8.1"));
        REQUIRE_FALSE(filter.is_valid_source(
            boost::asio::ip::make_address("239.3.8.1"), boost::asio::ip::make_address("192.168.16.52")
        ));
        REQUIRE(filter.is_valid_source(
            boost::asio::ip::make_address("239.3.8.1"), boost::asio::ip::make_address("192.168.16.53")
        ));
    }
}
