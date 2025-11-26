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

#include "ravennakit/core/util/uri.hpp"

#include <catch2/catch_all.hpp>

// https://datatracker.ietf.org/doc/html/rfc3986/

TEST_CASE("rav::Uri") {
    SECTION("uri parse") {
        SECTION("Full URI") {
            auto uri = rav::Uri::parse(
                "foo://user:pass@example.com:8042/some/path%20with%20space?key=value+space&key2=value2#fragment"
            );
            REQUIRE(uri.scheme == "foo");
            REQUIRE(uri.user == "user");
            REQUIRE(uri.password == "pass");
            REQUIRE(uri.host == "example.com");
            REQUIRE(uri.port == 8042);
            REQUIRE(uri.path == "/some/path with space");
            REQUIRE(uri.query == std::map<std::string, std::string> {{"key", "value space"}, {"key2", "value2"}});
            REQUIRE(uri.fragment == "fragment");
        }

        SECTION("Minimal URI") {
            auto uri = rav::Uri::parse("foo://");
            REQUIRE(uri.scheme == "foo");
            REQUIRE(uri.user.empty());
            REQUIRE(uri.password.empty());
            REQUIRE(uri.host.empty());
            REQUIRE_FALSE(uri.port.has_value());
            REQUIRE(uri.path.empty());
            REQUIRE(uri.query.empty());
            REQUIRE(uri.fragment.empty());
        }

        SECTION("Only host") {
            auto uri = rav::Uri::parse("foo://example.com");
            REQUIRE(uri.scheme == "foo");
            REQUIRE(uri.user.empty());
            REQUIRE(uri.password.empty());
            REQUIRE(uri.host == "example.com");
            REQUIRE_FALSE(uri.port.has_value());
            REQUIRE(uri.path.empty());
            REQUIRE(uri.query.empty());
            REQUIRE(uri.fragment.empty());
        }

        SECTION("With port") {
            auto uri = rav::Uri::parse("foo://example.com:1234");
            REQUIRE(uri.scheme == "foo");
            REQUIRE(uri.user.empty());
            REQUIRE(uri.password.empty());
            REQUIRE(uri.host == "example.com");
            REQUIRE(uri.port == 1234);
            REQUIRE(uri.path.empty());
            REQUIRE(uri.query.empty());
            REQUIRE(uri.fragment.empty());
        }

        SECTION("With path") {
            auto uri = rav::Uri::parse("foo://example.com:1234/some/path");
            REQUIRE(uri.scheme == "foo");
            REQUIRE(uri.user.empty());
            REQUIRE(uri.password.empty());
            REQUIRE(uri.host == "example.com");
            REQUIRE(uri.port == 1234);
            REQUIRE(uri.path == "/some/path");
            REQUIRE(uri.query.empty());
            REQUIRE(uri.fragment.empty());
        }

        SECTION("With query") {
            auto uri = rav::Uri::parse("foo://example.com:1234/some/path?key1=value1&key2=value2");
            REQUIRE(uri.scheme == "foo");
            REQUIRE(uri.user.empty());
            REQUIRE(uri.password.empty());
            REQUIRE(uri.host == "example.com");
            REQUIRE(uri.port == 1234);
            REQUIRE(uri.path == "/some/path");
            REQUIRE(uri.query == std::map<std::string, std::string> {{"key1", "value1"}, {"key2", "value2"}});
            REQUIRE(uri.fragment.empty());
        }

        SECTION("With fragment") {
            auto uri = rav::Uri::parse("foo://example.com:1234/some/path#fragment");
            REQUIRE(uri.scheme == "foo");
            REQUIRE(uri.user.empty());
            REQUIRE(uri.password.empty());
            REQUIRE(uri.host == "example.com");
            REQUIRE(uri.port == 1234);
            REQUIRE(uri.path == "/some/path");
            REQUIRE(uri.query.empty());
            REQUIRE(uri.fragment == "fragment");
        }
    }

    SECTION("uri to_string") {
        const rav::Uri uri {
            "foo",
            "user",
            "pass",
            "example.com",
            8042,
            "/some/path with space",
            {{"key1", "value with space"}, {"key2", "value2"}},
            "fragment"
        };
        auto result = uri.to_string();
        REQUIRE(
            result == "foo://user:pass@example.com:8042/some/path%20with%20space?key1=value+with+space&key2=value2#fragment"
        );
    }

    SECTION("uri decode") {
        auto result = rav::Uri::decode("foo%20bar%21+");
        REQUIRE(result == "foo bar!+");

        result = rav::Uri::decode("foo%20bar%21+", true);
        REQUIRE(result == "foo bar! ");

        result = rav::Uri::decode(
            "%20%21%22%23%24%25%26%27%28%29%2A%2B%2C%2D%2E%2F%3A%3B%3C%3D%3E%3F%40%5B%5C%5D%5E%5F%60%7B%7C%7D%7E"
        );
        REQUIRE(result == " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");  // Includes all unreserved characters
    }

    SECTION("uri encode") {
        auto result = rav::Uri::encode(" !\"#$%&'()*+,/:;<=>?@[\\]^`{|}", false, true);
        REQUIRE(result == "%20%21%22%23%24%25%26%27%28%29%2A%2B%2C%2F%3A%3B%3C%3D%3E%3F%40%5B%5C%5D%5E%60%7B%7C%7D");

        // Unreserved characters
        result = rav::Uri::encode("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~");
        REQUIRE(result == "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~");

        result = rav::Uri::encode(" ", true);
        REQUIRE(result == "+");

        result = rav::Uri::encode(" ", false);
        REQUIRE(result == "%20");

        result = rav::Uri::encode("/", true, true);
        REQUIRE(result == "%2F");

        result = rav::Uri::encode("/", true, false);
        REQUIRE(result == "/");
    }
}
