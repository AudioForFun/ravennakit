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

#include <catch2/catch_all.hpp>

#include "ravennakit/rtsp/detail/rtsp_request.hpp"

TEST_CASE("rav::rtsp::Request") {
    SECTION("Get header") {
        rav::rtsp::Request request;
        request.rtsp_headers.push_back(rav::rtsp::Headers::Header {"Content-Length", "123"});
        request.rtsp_headers.push_back({"Content-Type", "application/sdp"});

        if (const auto* header = request.rtsp_headers.get("Content-Length"); header) {
            REQUIRE(header->value == "123");
        } else {
            FAIL("Content-Length header not found");
        }

        if (const auto* header = request.rtsp_headers.get("Content-Type"); header) {
            REQUIRE(header->value == "application/sdp");
        } else {
            FAIL("Content-Type header not found");
        }

        REQUIRE(request.rtsp_headers.get("Content-Size") == nullptr);
    }

    SECTION("Get content length") {
        rav::rtsp::Request request;
        request.rtsp_headers.push_back({"Content-Length", "123"});

        if (auto content_length = request.rtsp_headers.get_content_length(); content_length) {
            REQUIRE(*content_length == 123);
        } else {
            FAIL("Content-Length header not found");
        }
    }

    SECTION("Get content length while there is no Content-Length header") {
        rav::rtsp::Request request;
        REQUIRE(request.rtsp_headers.get_content_length() == std::nullopt);
    }

    SECTION("reset") {
        rav::rtsp::Request request;
        request.method = "GET";
        request.uri = "/index.html";
        request.rtsp_version_major = 1;
        request.rtsp_version_minor = 1;
        request.rtsp_headers.emplace_back({"CSeq", "1"});
        request.data = "Hello, World!";
        request.clear();
        REQUIRE(request.method.empty());
        REQUIRE(request.uri.empty());
        REQUIRE(request.rtsp_version_major == 0);
        REQUIRE(request.rtsp_version_minor == 0);
        REQUIRE(request.rtsp_headers.empty());
        REQUIRE(request.data.empty());
    }
    SECTION("encode") {
        rav::rtsp::Request req;
        req.rtsp_version_major = 1;
        req.rtsp_version_minor = 0;
        req.method = "OPTIONS";
        req.uri = "*";
        req.rtsp_headers.push_back({"CSeq", "1"});
        req.rtsp_headers.push_back({"Accept", "application/sdp"});
        req.data = "Hello, World!";

        auto encoded = req.encode();
        REQUIRE(
            encoded
            == "OPTIONS * RTSP/1.0\r\nCSeq: 1\r\nAccept: application/sdp\r\ncontent-length: 13\r\n\r\nHello, World!"
        );
    }
}
