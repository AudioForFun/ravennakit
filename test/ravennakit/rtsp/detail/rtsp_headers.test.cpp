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

#include "ravennakit/rtsp/detail/rtsp_request.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::rtsp::Headers") {
    SECTION("Find header") {
        rav::rtsp::Headers headers;
        REQUIRE(headers.get("CSeq") == nullptr);
        headers.push_back({"CSeq", "1"});
        auto* header = headers.get("CSeq");
        REQUIRE(header != nullptr);
        REQUIRE(header->value == "1");
    }

    SECTION("Get content length") {
        rav::rtsp::Headers headers;
        REQUIRE(headers.get_content_length() == std::nullopt);
        headers.emplace_back({"Content-Length", "10"});
        REQUIRE(headers.get_content_length() == 10);
    }

    SECTION("reset") {
        rav::rtsp::Headers headers;
        headers.push_back({"CSeq", "1"});
        headers.push_back({"Content-Length", "10"});
        headers.clear();
        REQUIRE(headers.empty());
    }

    SECTION("Add header, make sure existing header gets updated using emplace_back") {
        rav::rtsp::Headers headers;
        headers.emplace_back({"CSeq", "1"});
        headers.emplace_back({"CSeq", "2"});
        REQUIRE(headers.size() == 1);
        REQUIRE(headers.get_or_default("CSeq") == "2");
    }

    SECTION("Add header, make sure existing header gets updated using emplace_back case insensitive") {
        rav::rtsp::Headers headers;
        headers.emplace_back({"cseq", "1"});
        headers.emplace_back({"CSeq", "2"});
        REQUIRE(headers.size() == 1);
        REQUIRE(headers[0].value == "2");
    }

    SECTION("Add header, make sure existing header gets updated using push_back") {
        rav::rtsp::Headers headers;
        headers.push_back({"CSeq", "1"});
        headers.push_back({"CSeq", "2"});
        REQUIRE(headers.size() == 1);
        REQUIRE(headers[0].value == "2");
    }

    SECTION("Add header, make sure existing header gets updated using push_back case insensitive") {
        rav::rtsp::Headers headers;
        headers.push_back({"cseq", "1"});
        headers.push_back({"CSeq", "2"});
        REQUIRE(headers.size() == 1);
        REQUIRE(headers[0].value == "2");
    }
}
