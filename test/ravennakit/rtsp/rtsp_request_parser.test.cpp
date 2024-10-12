/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/rtsp/rtsp_request_parser.hpp"

#include <catch2/catch_all.hpp>

namespace {

/**
 * Returns a vector with copies of original, one with the original and one with \r\n replaced with \n.
 * @param original The original string.
 * @return Vector of strings.
 */
std::vector<std::string> replace_newlines(const std::string& original) {
    std::vector<std::string> texts;
    texts.emplace_back(original);
    texts.emplace_back(rav::string_replace(original, "\r\n", "\n"));
    return texts;
}

}  // namespace

TEST_CASE("rtsp_request_parser", "[rtsp_request_parser]") {
    SECTION("Parse without headers and without data") {
        auto texts = replace_newlines("DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\n\r\n");

        for (auto& txt : texts) {
            rav::rtsp_request request;
            rav::rtsp_request_parser parser(request);
            auto [result, consumed] = parser.parse(txt);
            REQUIRE(result == rav::rtsp_request_parser::result::good);
            REQUIRE(consumed == txt.size());
            REQUIRE(request.method == "DESCRIBE");
            REQUIRE(request.uri == "rtsp://server.example.com/fizzle/foo");
            REQUIRE(request.rtsp_version_major == 1);
            REQUIRE(request.rtsp_version_minor == 0);
            REQUIRE(request.headers.empty());
        }
    }

    SECTION("Parse with headers and without data") {
        auto texts = replace_newlines(
            "DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\nCSeq: 312\r\nAccept: application/sdp, application/rtsl, application/mheg\r\n\r\n"
        );

        for (auto& txt : texts) {
            rav::rtsp_request request;
            rav::rtsp_request_parser parser(request);
            auto [result, consumed] = parser.parse(txt);
            REQUIRE(result == rav::rtsp_request_parser::result::good);
            REQUIRE(consumed == txt.size());
            REQUIRE(request.method == "DESCRIBE");
            REQUIRE(request.uri == "rtsp://server.example.com/fizzle/foo");
            REQUIRE(request.rtsp_version_major == 1);
            REQUIRE(request.rtsp_version_minor == 0);
            REQUIRE_FALSE(request.headers.empty());
            REQUIRE(request.headers.size() == 2);
            REQUIRE(request.headers[0].name == "CSeq");
            REQUIRE(request.headers[0].value == "312");
            REQUIRE(request.headers[1].name == "Accept");
            REQUIRE(request.headers[1].value == "application/sdp, application/rtsl, application/mheg");
        }
    }

    SECTION("Parse folded headers") {
        auto texts = replace_newlines(
            "DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\nCSeq: 312\r\nAccept: application/sdp, \r\n application/rtsl, application/mheg\r\n\r\n"
        );

        auto tab_folder = replace_newlines(
            "DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\nCSeq: 312\r\nAccept: application/sdp, \r\n\tapplication/rtsl, application/mheg\r\n\r\n"
        );

        texts.insert(texts.end(), tab_folder.begin(), tab_folder.end());

        for (auto& txt : texts) {
            rav::rtsp_request request;
            rav::rtsp_request_parser parser(request);
            auto [result, consumed] = parser.parse(txt);
            REQUIRE(result == rav::rtsp_request_parser::result::good);
            REQUIRE(consumed == txt.size());
            REQUIRE(request.method == "DESCRIBE");
            REQUIRE(request.uri == "rtsp://server.example.com/fizzle/foo");
            REQUIRE(request.rtsp_version_major == 1);
            REQUIRE(request.rtsp_version_minor == 0);
            REQUIRE_FALSE(request.headers.empty());
            REQUIRE(request.headers.size() == 2);
            REQUIRE(request.headers[0].name == "CSeq");
            REQUIRE(request.headers[0].value == "312");
            REQUIRE(request.headers[1].name == "Accept");
            REQUIRE(request.headers[1].value == "application/sdp, application/rtsl, application/mheg");
        }
    }

    SECTION("Parse chunked") {
        auto texts = replace_newlines(
            "DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\nCSeq: 312\r\nAccept: application/sdp, application/rtsl, application/mheg\r\n\r\n"
        );

        for (auto& txt : texts) {
            rav::rtsp_request request;
            rav::rtsp_request_parser parser(request);
            constexpr size_t chunk_size = 3;
            size_t total_consumed = 0;
            for (size_t i = 0; i < txt.size(); i += chunk_size) {
                auto subview = txt.substr(i, chunk_size);
                auto [result, consumed] = parser.parse(subview);
                total_consumed += consumed;
                RAV_INFO("Consumed: {}, total consumed: {}", consumed, total_consumed);
                if (result == rav::rtsp_request_parser::result::good) {
                    REQUIRE(total_consumed == txt.size());
                    break;
                }

                REQUIRE(result == rav::rtsp_request_parser::result::indeterminate);
            }
            REQUIRE(request.method == "DESCRIBE");
            REQUIRE(request.uri == "rtsp://server.example.com/fizzle/foo");
            REQUIRE(request.rtsp_version_major == 1);
            REQUIRE(request.rtsp_version_minor == 0);
            REQUIRE_FALSE(request.headers.empty());
            REQUIRE(request.headers.size() == 2);
            REQUIRE(request.headers[0].name == "CSeq");
            REQUIRE(request.headers[0].value == "312");
            REQUIRE(request.headers[1].name == "Accept");
            REQUIRE(request.headers[1].value == "application/sdp, application/rtsl, application/mheg");
        }
    }

    SECTION("Parse with headers and with data") {
        auto texts = replace_newlines(
            "DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\nContent-Length: 28\r\n\r\nthis_is_the_part_called_data"
        );

        for (auto& txt : texts) {
            rav::rtsp_request request;
            rav::rtsp_request_parser parser(request);
            auto [result, consumed] = parser.parse(txt);
            REQUIRE(result == rav::rtsp_request_parser::result::good);
            REQUIRE(request.method == "DESCRIBE");
            REQUIRE(request.uri == "rtsp://server.example.com/fizzle/foo");
            REQUIRE(request.rtsp_version_major == 1);
            REQUIRE(request.rtsp_version_minor == 0);
            if (auto length = request.get_content_length()) {
                REQUIRE(length.value() == 28);
            } else {
                FAIL("Content-Length header not found");
            }
            REQUIRE(consumed == txt.size());
            REQUIRE(request.data == "this_is_the_part_called_data");
        }
    }

    SECTION("Parse with headers and with data in different chunks") {
        // TODO
    }
}
