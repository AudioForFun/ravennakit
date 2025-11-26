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

#include "ravennakit/core/json.hpp"
#include "ravennakit/core/net/http/http_client.hpp"

#include <iostream>
#include <catch2/catch_all.hpp>
#include <fmt/format.h>

TEST_CASE("rav::HttpClient") {
    SECTION("get_async") {
        boost::asio::io_context io_context;

        int counter = 0;
        auto token =
            [&counter](
                const boost::system::result<boost::beast::http::response<boost::beast::http::string_body>>& response
            ) {
                REQUIRE(response.has_value());
                REQUIRE(response->result() == boost::beast::http::status::ok);
                REQUIRE(!response->body().empty());

                auto json_body = boost::json::parse(response->body());
                REQUIRE(json_body.at("url") == "http://httpbin.cpp.al/get");
                counter++;
            };

        rav::HttpClient client(io_context, "http://httpbin.cpp.al");
        client.get_async("/get", token);
        io_context.run();

        REQUIRE(counter == 1);
    }

    SECTION("post_async") {
        boost::asio::io_context io_context;

        static constexpr auto num_requests = 5;
        int counter = 0;

        rav::HttpClient client(io_context, "http://httpbin.cpp.al");

        for (auto i = 0; i < num_requests; ++i) {
            boost::json::object json_body;
            json_body["test"] = i + 1;

            client.post_async(
                "/post", boost::json::serialize(json_body),
                [&counter, json_body](
                    const boost::system::result<boost::beast::http::response<boost::beast::http::string_body>>& response
                ) {
                    REQUIRE(response.has_value());
                    REQUIRE(response->result() == boost::beast::http::status::ok);
                    REQUIRE(!response->body().empty());

                    auto body = response->body();
                    auto json_body_returned = boost::json::parse(response->body());
                    REQUIRE(json_body_returned.at("json") == json_body);
                    REQUIRE(json_body_returned.at("url") == "http://httpbin.cpp.al/post");
                    counter++;
                },
                {}
            );
        }

        io_context.run();

        REQUIRE(counter == num_requests);
    }

    SECTION("cancel_outstanding_requests") {
        boost::asio::io_context io_context;

        int counter = 0;

        rav::HttpClient client(io_context, "http://httpbin.cpp.al");

        for (auto i = 0; i < 100; ++i) {
            boost::json::object json_body;
            json_body["test"] = i + 1;

            client.post_async(
                "/post", boost::json::serialize(json_body),
                [&counter, &client, json_body](
                    const boost::system::result<boost::beast::http::response<boost::beast::http::string_body>>& response
                ) {
                    REQUIRE(response.has_value());
                    REQUIRE(response->result() == boost::beast::http::status::ok);
                    REQUIRE(!response->body().empty());

                    auto body = response->body();
                    auto json_body_returned = boost::json::parse(response->body());
                    REQUIRE(json_body_returned.at("json") == json_body);
                    REQUIRE(json_body_returned.at("url") == "http://httpbin.cpp.al/post");
                    client.cancel_outstanding_requests();  // Cancel all requests after the first one
                    counter++;
                },
                {}
            );
        }

        io_context.run();

        REQUIRE(counter == 1);
    }
}
