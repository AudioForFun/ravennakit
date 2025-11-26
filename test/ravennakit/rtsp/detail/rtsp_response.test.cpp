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

#include "ravennakit/rtsp/detail/rtsp_response.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::rtsp::Response") {
    SECTION("reset") {
        rav::rtsp::Response request;
        request.status_code = 404;
        request.reason_phrase = "Error";
        request.rtsp_version_major = 1;
        request.rtsp_version_minor = 1;
        request.rtsp_headers.emplace_back({"CSeq", "1"});
        request.data = "Hello, World!";
        request.clear();
        REQUIRE(request.status_code == 0);
        REQUIRE(request.reason_phrase.empty());
        REQUIRE(request.rtsp_version_major == 0);
        REQUIRE(request.rtsp_version_minor == 0);
        REQUIRE(request.rtsp_headers.empty());
        REQUIRE(request.data.empty());
    }

    SECTION("encode") {
        rav::rtsp::Response res;
        res.rtsp_version_major = 1;
        res.rtsp_version_minor = 0;
        res.status_code = 200;
        res.reason_phrase = "OK";
        res.rtsp_headers.push_back({"CSeq", "1"});
        res.rtsp_headers.push_back({"Accept", "application/sdp"});
        res.data = "Hello, World!";

        REQUIRE(
            res.encode() == "RTSP/1.0 200 OK\r\nCSeq: 1\r\nAccept: application/sdp\r\ncontent-length: 13\r\n\r\nHello, World!"
        );

        res.rtsp_headers.push_back({"Content-Length", "555"});

        REQUIRE(
             res.encode() == "RTSP/1.0 200 OK\r\nCSeq: 1\r\nAccept: application/sdp\r\ncontent-length: 13\r\n\r\nHello, World!"
        );
    }
}



