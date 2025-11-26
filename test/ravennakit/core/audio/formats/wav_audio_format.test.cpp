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

#include "wav_audio_format.data.cpp"
#include "ravennakit/core/audio/formats/wav_audio_format.hpp"
#include "ravennakit/core/streams/byte_stream.hpp"
#include "ravennakit/core/util.hpp"

TEST_CASE("rav::WavAudioFormat") {
    {
        REQUIRE(sin_1ms_wav.size() == 1808);

        auto bytes = std::make_unique<rav::ByteStream>(sin_1ms_wav);
        REQUIRE(bytes->size().value() == 1808);

        rav::WavAudioFormat::Reader reader(std::move(bytes));
        REQUIRE(reader.num_channels() == 2);
        REQUIRE(rav::is_within(reader.sample_rate(), 44100.0, 0.00001));

        std::vector<uint8_t> read_audio_data(1764, 0);

        SECTION("Read all data in one go") {
            const auto read = reader.read_audio_data(read_audio_data.data(), read_audio_data.size());
            REQUIRE(read == 1764);

            // Compare whether the audio data is the same
            REQUIRE(
                std::equal(sin_1ms_wav.begin() + 44, sin_1ms_wav.end(), read_audio_data.begin(), read_audio_data.end())
            );
        }

        SECTION("Read in two parts") {
            const auto read1 = reader.read_audio_data(read_audio_data.data(), read_audio_data.size() / 2);
            REQUIRE(read1 == 1764 / 2);

            const auto read2 =
                reader.read_audio_data(read_audio_data.data() + read1.value(), read_audio_data.size() - read1.value());
            REQUIRE(read2 == read_audio_data.size() - read1.value());

            // Compare whether the audio data is the same
            REQUIRE(
                std::equal(sin_1ms_wav.begin() + 44, sin_1ms_wav.end(), read_audio_data.begin(), read_audio_data.end())
            );
        }
    }

    SECTION("Write wav file") {
        constexpr auto sin_1ms_wav_header_size = 44;
        const auto sin_1ms_wav_audio_data_size = sin_1ms_wav.size() - sin_1ms_wav_header_size;
        rav::ByteStream bytes;
        {
            rav::WavAudioFormat::Writer writer(bytes, rav::WavAudioFormat::FormatCode::pcm, 44100, 2, 16);
            REQUIRE(writer.write_audio_data(sin_1ms_wav.data() + sin_1ms_wav_header_size, sin_1ms_wav_audio_data_size));
            // Let writer go out of scope to let it finalize the file (in the destructor).
        }

        // 44 is the size of the header of the current implementation.
        constexpr auto header_size = 44;
        REQUIRE(bytes.size() == sin_1ms_wav_audio_data_size + header_size);

        REQUIRE(bytes.read_as_string(4) == "RIFF");
        REQUIRE(bytes.read_le<uint32_t>().value() == 1800);
        REQUIRE(bytes.read_as_string(4) == "WAVE");
        REQUIRE(bytes.read_as_string(4) == "fmt ");
        REQUIRE(bytes.read_le<uint32_t>().value() == 16);      // fmt chunk size
        REQUIRE(bytes.read_le<uint16_t>().value() == 0x1);     // Format code
        REQUIRE(bytes.read_le<uint16_t>().value() == 2);       // Num channels
        REQUIRE(bytes.read_le<uint32_t>().value() == 44100);   // Sample rate
        REQUIRE(bytes.read_le<uint32_t>().value() == 176400);  // Avg bytes per sec
        REQUIRE(bytes.read_le<uint16_t>().value() == 4);       // Block align
        REQUIRE(bytes.read_le<uint16_t>().value() == 16);      // Bits per sample
        REQUIRE(bytes.read_as_string(4) == "data");
        REQUIRE(bytes.read_le<uint32_t>().value() == 1764);  // Data size

        std::vector<uint8_t> read_audio_data(1764, 0);
        REQUIRE(bytes.read(read_audio_data.data(), read_audio_data.size()) == 1764);
        REQUIRE(
            std::equal(
                sin_1ms_wav.begin() + sin_1ms_wav_header_size, sin_1ms_wav.end(), read_audio_data.begin(),
                read_audio_data.end()
            )
        );
    }
}
