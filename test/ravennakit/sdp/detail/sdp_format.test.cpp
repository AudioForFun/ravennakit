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

#include "ravennakit/sdp/detail/sdp_format.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::sdp::Format") {
    SECTION("98/L16/48000/2") {
        auto fmt = rav::sdp::parse_format("98 L16/48000/2");
        REQUIRE(fmt);
        REQUIRE(fmt->payload_type == 98);
        REQUIRE(fmt->encoding_name == "L16");
        REQUIRE(fmt->clock_rate == 48000);
        REQUIRE(fmt->num_channels == 2);
        auto audio_format = rav::sdp::make_audio_format(*fmt);
        REQUIRE(audio_format.has_value());
        auto expected_audio_format = rav::AudioFormat {
            rav::AudioFormat::ByteOrder::be, rav::AudioEncoding::pcm_s16,
            rav::AudioFormat::ChannelOrdering::interleaved, 48000, 2
        };
        REQUIRE(*audio_format == expected_audio_format);
    }

    SECTION("98/L16/48000/4") {
        auto fmt = rav::sdp::parse_format("98 L16/48000/4");
        REQUIRE(fmt);
        REQUIRE(fmt->payload_type == 98);
        REQUIRE(fmt->encoding_name == "L16");
        REQUIRE(fmt->clock_rate == 48000);
        REQUIRE(fmt->num_channels == 4);
        auto audio_format = rav::sdp::make_audio_format(*fmt);
        REQUIRE(audio_format.has_value());
        auto expected_audio_format = rav::AudioFormat {
            rav::AudioFormat::ByteOrder::be, rav::AudioEncoding::pcm_s16,
            rav::AudioFormat::ChannelOrdering::interleaved, 48000, 4
        };
        REQUIRE(*audio_format == expected_audio_format);
    }

    SECTION("98/L24/48000/2") {
        auto fmt = rav::sdp::parse_format("98 L24/48000/2");
        REQUIRE(fmt);
        REQUIRE(fmt->payload_type == 98);
        REQUIRE(fmt->encoding_name == "L24");
        REQUIRE(fmt->clock_rate == 48000);
        REQUIRE(fmt->num_channels == 2);
        auto audio_format = rav::sdp::make_audio_format(*fmt);
        REQUIRE(audio_format.has_value());
        auto expected_audio_format = rav::AudioFormat {
            rav::AudioFormat::ByteOrder::be, rav::AudioEncoding::pcm_s24,
            rav::AudioFormat::ChannelOrdering::interleaved, 48000, 2
        };
        REQUIRE(*audio_format == expected_audio_format);
    }

    SECTION("98/L32/48000/2") {
        auto fmt = rav::sdp::parse_format("98 L32/48000/2");
        REQUIRE(fmt);
        REQUIRE(fmt->payload_type == 98);
        REQUIRE(fmt->encoding_name == "L32");
        REQUIRE(fmt->clock_rate == 48000);
        REQUIRE(fmt->num_channels == 2);
        auto audio_format = rav::sdp::make_audio_format(*fmt);
        REQUIRE(audio_format.has_value());
        auto expected_audio_format = rav::AudioFormat {
            rav::AudioFormat::ByteOrder::be, rav::AudioEncoding::pcm_s32,
            rav::AudioFormat::ChannelOrdering::interleaved, 48000, 2
        };
        REQUIRE(*audio_format == expected_audio_format);
    }

    SECTION("98/NA/48000/2") {
        auto fmt = rav::sdp::parse_format("98 NA/48000/2");
        REQUIRE(fmt);
        REQUIRE(fmt->payload_type == 98);
        REQUIRE(fmt->encoding_name == "NA");
        REQUIRE(fmt->clock_rate == 48000);
        REQUIRE(fmt->num_channels == 2);
        auto audio_format = rav::sdp::make_audio_format(*fmt);
        REQUIRE_FALSE(audio_format.has_value());
    }
}
