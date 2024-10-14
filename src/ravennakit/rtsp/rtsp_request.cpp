/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/rtsp/rtsp_request.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rtsp_request", "[rtsp_request]") {
    SECTION("Get header") {
        rav::rtsp_request request;
        request.headers.push_back({"Content-Length", "123"});
        request.headers.push_back({"Content-Type", "application/sdp"});

        if (const std::string* content_length = request.get_header_value("Content-Length"); content_length) {
            REQUIRE(*content_length == "123");
        } else {
            FAIL("Content-Length header not found");
        }

        if (const std::string* content_type = request.get_header_value("Content-Type"); content_type) {
            REQUIRE(*content_type == "application/sdp");
        } else {
            FAIL("Content-Type header not found");
        }

        REQUIRE(request.get_header_value("Content-Size") == nullptr);
    }

    SECTION("Get content length") {
        rav::rtsp_request request;
        request.headers.push_back({"Content-Length", "123"});

        if (auto content_length = request.get_content_length(); content_length) {
            REQUIRE(*content_length == 123);
        } else {
            FAIL("Content-Length header not found");
        }
    }

    SECTION("Get content length while there is no Content-Length header") {
        rav::rtsp_request request;
        REQUIRE(request.get_content_length() == std::nullopt);
    }
}
