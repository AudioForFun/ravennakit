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

#include "ravennakit/core/audio/audio_format.hpp"
#include "audio_format.test.hpp"

#include <nanobench.h>
#include <catch2/catch_all.hpp>

TEST_CASE("rav::AudioFormat") {
    rav::AudioFormat audio_format;
    audio_format.byte_order = rav::AudioFormat::ByteOrder::be;
    audio_format.encoding = rav::AudioEncoding::pcm_s24;
    audio_format.sample_rate = 44100;
    audio_format.num_channels = 2;
    audio_format.ordering = rav::AudioFormat::ChannelOrdering::interleaved;

    test_audio_format_json(audio_format, boost::json::value_from(audio_format));
}

void rav::test_audio_format_json(const AudioFormat& audio_format, const boost::json::value& json) {
    REQUIRE(json.at("byte_order") == AudioFormat::to_string(audio_format.byte_order));
    REQUIRE(json.at("channel_ordering") == AudioFormat::to_string(audio_format.ordering));
    REQUIRE(json.at("encoding") == to_string(audio_format.encoding));
    REQUIRE(json.at("num_channels") == audio_format.num_channels);
    REQUIRE(json.at("sample_rate") == audio_format.sample_rate);
}
