/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/audio/audio_data.hpp"

#include <catch2/catch_all.hpp>

#include "ravennakit/containers/buffer_view.hpp"
#include "ravennakit/containers/vector_stream.hpp"
#include "ravennakit/core/util.hpp"

using namespace rav::audio_data;

namespace {
constexpr float f32_tolerance = 0.00004f;
constexpr double f64_tolerance = 0.00004;
}  // namespace

// MARK: - Interleaving conversions

TEST_CASE("audio_data | interleaving", "[audio_data]") {
    SECTION("Interleaved to interleaved int16") {
        constexpr std::array<int16_t, 4> src {1, 2, 3, 4};
        std::array<int16_t, 4> dst {};

        REQUIRE(rav::audio_data::convert<
                int16_t, byte_order::le, interleaving::interleaved, int16_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        ));

        REQUIRE(dst == std::array<int16_t, 4> {1, 2, 3, 4});
    }

    SECTION("Interleaved to interleaved int32") {
        constexpr std::array<int32_t, 4> src {1, 2, 3, 4};
        std::array<int32_t, 4> dst {};

        REQUIRE(rav::audio_data::convert<
                int32_t, byte_order::le, interleaving::interleaved, int32_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        ));

        REQUIRE(dst == std::array<int32_t, 4> {1, 2, 3, 4});
    }

    SECTION("Noninterleaved to noninterleaved int16") {
        constexpr std::array<int16_t, 4> src {1, 2, 3, 4};
        std::array<int16_t, 4> dst {};

        REQUIRE(rav::audio_data::convert<
                int16_t, byte_order::le, interleaving::noninterleaved, int16_t, byte_order::le,
                interleaving::noninterleaved>(src.data(), src.size(), dst.data(), dst.size(), 2));

        REQUIRE(dst == std::array<int16_t, 4> {1, 2, 3, 4});
    }

    SECTION("Noninterleaved to noninterleaved int32") {
        constexpr std::array<int32_t, 4> src {1, 2, 3, 4};
        std::array<int32_t, 4> dst {};

        auto result = rav::audio_data::convert<
            int32_t, byte_order::le, interleaving::noninterleaved, int32_t, byte_order::le,
            interleaving::noninterleaved>(src.data(), src.size(), dst.data(), dst.size(), 2);

        REQUIRE(result);
        REQUIRE(dst == std::array<int32_t, 4> {1, 2, 3, 4});
    }

    SECTION("Interleaved to non-interleaved int16") {
        constexpr std::array<int16_t, 4> src {1, 2, 3, 4};
        std::array<int16_t, 4> dst {};

        auto result = rav::audio_data::convert<
            int16_t, byte_order::le, interleaving::interleaved, int16_t, byte_order::le, interleaving::noninterleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst == std::array<int16_t, 4> {1, 3, 2, 4});
    }

    SECTION("Interleaved to non-interleaved int32") {
        constexpr std::array<int32_t, 4> src {1, 2, 3, 4};
        std::array<int32_t, 4> dst {};

        auto result = rav::audio_data::convert<
            int32_t, byte_order::le, interleaving::interleaved, int32_t, byte_order::le, interleaving::noninterleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst == std::array<int32_t, 4> {1, 3, 2, 4});
    }

    SECTION("Noninterleaved to interleaved int16") {
        constexpr std::array<int16_t, 4> src {1, 2, 3, 4};
        std::array<int16_t, 4> dst {};

        auto result = rav::audio_data::convert<
            int16_t, byte_order::le, interleaving::noninterleaved, int16_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst == std::array<int16_t, 4> {1, 3, 2, 4});
    }

    SECTION("Noninterleaved to interleaved int32") {
        constexpr std::array<int32_t, 4> src {1, 2, 3, 4};
        std::array<int32_t, 4> dst {};

        auto result = rav::audio_data::convert<
            int32_t, byte_order::le, interleaving::noninterleaved, int32_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst == std::array<int32_t, 4> {1, 3, 2, 4});
    }
}

// MARK: - Endian conversions

TEST_CASE("audio_data | endian conversions", "[audio_data]") {
    SECTION("Big-endian to little-endian int16") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({1, 2, 3, 4});
        rav::vector_stream<int16_t> dst(4);

        REQUIRE(rav::audio_data::convert<
                int16_t, byte_order::be, interleaving::interleaved, int16_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        ));

        REQUIRE(dst.read_le() == 1);
        REQUIRE(dst.read_le() == 2);
        REQUIRE(dst.read_le() == 3);
        REQUIRE(dst.read_le() == 4);
    }

    SECTION("Big-endian to native-endian int16") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({1, 2, 3, 4});
        rav::vector_stream<int16_t> dst(4);

        REQUIRE(rav::audio_data::convert<
                int16_t, byte_order::be, interleaving::interleaved, int16_t, byte_order::ne, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        ));

        REQUIRE(dst.read() == 1);
        REQUIRE(dst.read() == 2);
        REQUIRE(dst.read() == 3);
        REQUIRE(dst.read() == 4);
    }

    SECTION("Big-endian to big-endian int16") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({1, 2, 3, 4});
        rav::vector_stream<int16_t> dst(4);

        REQUIRE(rav::audio_data::convert<
                int16_t, byte_order::be, interleaving::interleaved, int16_t, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        ));

        REQUIRE(dst.read_be() == 1);
        REQUIRE(dst.read_be() == 2);
        REQUIRE(dst.read_be() == 3);
        REQUIRE(dst.read_be() == 4);
    }

    SECTION("Little-endian to big-endian int16") {
        rav::vector_stream<int16_t> src;
        src.push_back_le({1, 2, 3, 4});
        rav::vector_stream<int16_t> dst(4);

        REQUIRE(rav::audio_data::convert<
                int16_t, byte_order::le, interleaving::interleaved, int16_t, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        ));

        REQUIRE(dst.read_be() == 1);
        REQUIRE(dst.read_be() == 2);
        REQUIRE(dst.read_be() == 3);
        REQUIRE(dst.read_be() == 4);
    }

    SECTION("Little-endian to native-endian int16") {
        rav::vector_stream<int16_t> src;
        src.push_back_le({1, 2, 3, 4});
        rav::vector_stream<int16_t> dst(4);

        REQUIRE(rav::audio_data::convert<
                int16_t, byte_order::le, interleaving::interleaved, int16_t, byte_order::ne, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        ));

        REQUIRE(dst.read() == 1);
        REQUIRE(dst.read() == 2);
        REQUIRE(dst.read() == 3);
        REQUIRE(dst.read() == 4);
    }
}

// MARK: - Sample conversions

TEST_CASE("audio_data | sample conversions", "[audio_data]") {
    SECTION("Minimum value") {
        constexpr int16_t src = std::numeric_limits<int16_t>::min();
        int16_t dst {};

        rav::audio_data::convert_sample<int16_t, byte_order::be, int16_t, byte_order::le>(&src, &dst);

        REQUIRE(reinterpret_cast<uint8_t*>(&dst)[0] == 0x80);
        REQUIRE(reinterpret_cast<uint8_t*>(&dst)[1] == 0x0);
    }

    SECTION("Max value") {
        constexpr int16_t src = std::numeric_limits<int16_t>::max();
        int16_t dst {};

        rav::audio_data::convert_sample<int16_t, byte_order::be, int16_t, byte_order::le>(&src, &dst);

        REQUIRE(reinterpret_cast<uint8_t*>(&dst)[0] == 0x7f);
        REQUIRE(reinterpret_cast<uint8_t*>(&dst)[1] == 0xff);
    }
}

// MARK: - Specific conversions

TEST_CASE("audio_data | uint8 to int8", "[audio_data]") {
    SECTION("Convert uint8 to int8 be to be") {
        rav::vector_stream<uint8_t> src({0, 255, 128, 0});
        std::array<int8_t, 4> dst {};

        auto result = rav::audio_data::convert<
            uint8_t, byte_order::be, interleaving::interleaved, int8_t, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst == std::array<int8_t, 4> {-128, 127, 0, -128});
    }

    SECTION("Convert uint8 to int8 be to le") {
        rav::vector_stream<uint8_t> src({0, 255, 128, 0});
        std::array<int8_t, 4> dst {};

        auto result = rav::audio_data::convert<
            uint8_t, byte_order::be, interleaving::interleaved, int8_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst == std::array<int8_t, 4> {-128, 127, 0, -128});
    }

    SECTION("Convert uint8 to int8 le to be") {
        rav::vector_stream<uint8_t> src({0, 255, 128, 0});
        std::array<int8_t, 4> dst {};

        auto result = rav::audio_data::convert<
            uint8_t, byte_order::be, interleaving::interleaved, int8_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst == std::array<int8_t, 4> {-128, 127, 0, -128});
    }
}

TEST_CASE("audio_data | int8 to int16", "[audio_data]") {
    SECTION("Convert int8 to int16 be to be") {
        rav::vector_stream<int8_t> src;
        src.push_back_be({-128, 127, 0, -127});
        rav::vector_stream<int16_t> dst(4);

        auto result = rav::audio_data::convert<
            int8_t, byte_order::be, interleaving::interleaved, int16_t, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst.read_be() == -32768);
        REQUIRE(dst.read_be() == 32512);
        REQUIRE(dst.read_be() == 0);
        REQUIRE(dst.read_be() == -32512);
    }

    SECTION("Convert int8 to int16 le to be") {
        rav::vector_stream<int8_t> src;
        src.push_back_le({-128, 127, 0, -127});
        rav::vector_stream<int16_t> dst(4);

        auto result = rav::audio_data::convert<
            int8_t, byte_order::le, interleaving::interleaved, int16_t, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst.read_be() == -32768);
        REQUIRE(dst.read_be() == 32512);
        REQUIRE(dst.read_be() == 0);
        REQUIRE(dst.read_be() == -32512);
    }

    SECTION("Convert int8 to int16 be to le") {
        rav::vector_stream<int8_t> src;
        src.push_back_le({-128, 127, 0, -127});
        rav::vector_stream<int16_t> dst(4);

        auto result = rav::audio_data::convert<
            int8_t, byte_order::be, interleaving::interleaved, int16_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst.read_le() == -32768);
        REQUIRE(dst.read_le() == 32512);
        REQUIRE(dst.read_le() == 0);
        REQUIRE(dst.read_le() == -32512);
    }
}

TEST_CASE("audio_data | int16 to int24", "[audio_data]") {
    SECTION("Convert int16 to int24 be to be") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({-32768, 32767, 0, -32767});
        rav::vector_stream<rav::int24_t> dst(4);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::be, interleaving::interleaved, rav::int24_t, byte_order::be,
            interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 2);

        REQUIRE(result);
        REQUIRE(dst.read_be() == -8388608);
        REQUIRE(dst.read_be() == 8388352);
        REQUIRE(dst.read_be() == 0);
        REQUIRE(dst.read_be() == -8388352);
    }

    SECTION("Convert int16 to int24 le to be") {
        rav::vector_stream<int16_t> src;
        src.push_back_le({-32768, 32767, 0, -32767});
        rav::vector_stream<rav::int24_t> dst(4);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::le, interleaving::interleaved, rav::int24_t, byte_order::be,
            interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 2);

        REQUIRE(result);
        REQUIRE(dst.read_be() == -8388608);
        REQUIRE(dst.read_be() == 8388352);
        REQUIRE(dst.read_be() == 0);
        REQUIRE(dst.read_be() == -8388352);
    }

    SECTION("Convert int16 to int24 be to le") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({-32768, 32767, 0, -32767});
        rav::vector_stream<rav::int24_t> dst(4);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::be, interleaving::interleaved, rav::int24_t, byte_order::le,
            interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 2);

        REQUIRE(result);
        REQUIRE(dst.read_le() == -8388608);
        REQUIRE(dst.read_le() == 8388352);
        REQUIRE(dst.read_le() == 0);
        REQUIRE(dst.read_le() == -8388352);
    }
}

TEST_CASE("audio_data | int16 to int32", "[audio_data]") {
    SECTION("Convert int16 to int32 be to be") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({-32768, 32767, 0, -32768});
        rav::vector_stream<int32_t> dst(4);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::be, interleaving::interleaved, int32_t, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst.read_be() == static_cast<int32_t>(-0x80000000));
        REQUIRE(dst.read_be() == 0x7fff0000);
        REQUIRE(dst.read_be() == 0);
        REQUIRE(dst.read_be() == static_cast<int32_t>(-0x80000000));
    }

    SECTION("Convert int16 to int32 be to le") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({-32768, 32767, 0, -32768});
        rav::vector_stream<int32_t> dst(4);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::be, interleaving::interleaved, int32_t, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst.read_le() == static_cast<int32_t>(-0x80000000));
        REQUIRE(dst.read_le() == 0x7fff0000);
        REQUIRE(dst.read_le() == 0);
        REQUIRE(dst.read_le() == static_cast<int32_t>(-0x80000000));
    }

    SECTION("Convert int16 to int32 le to be") {
        rav::vector_stream<int16_t> src;
        src.push_back_le({-32768, 32767, 0, -32768});
        rav::vector_stream<int32_t> dst(4);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::le, interleaving::interleaved, int32_t, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 2
        );

        REQUIRE(result);
        REQUIRE(dst.read_be() == static_cast<int32_t>(-0x80000000));
        REQUIRE(dst.read_be() == 0x7fff0000);
        REQUIRE(dst.read_be() == 0);
        REQUIRE(dst.read_be() == static_cast<int32_t>(-0x80000000));
    }
}

TEST_CASE("audio_data | int16 to float", "[audio_data]") {
    SECTION("Convert int16 to float be to be") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({-32768, 32767, 0});
        rav::vector_stream<float> dst(3);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::be, interleaving::interleaved, float, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 1
        );

        REQUIRE(result);
        REQUIRE(rav::util::is_within(dst.read_be(), -1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read_be(), +1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read_be(), +0.f, f32_tolerance));
    }

    SECTION("Convert int16 to float be to le") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({-32768, 32767, 0});
        rav::vector_stream<float> dst(3);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::be, interleaving::interleaved, float, byte_order::le, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 1
        );

        REQUIRE(result);
        REQUIRE(rav::util::is_within(dst.read_le(), -1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read_le(), +1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read_le(), +0.f, f32_tolerance));
    }

    SECTION("Convert int16 to float be to ne") {
        rav::vector_stream<int16_t> src;
        src.push_back_be({-32768, 32767, 0});
        rav::vector_stream<float> dst(3);

        auto result = rav::audio_data::convert<
            int16_t, byte_order::be, interleaving::interleaved, float, byte_order::ne, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 1
        );

        REQUIRE(result);
        REQUIRE(rav::util::is_within(dst.read(), -1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read(), +1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read(), +0.f, f32_tolerance));
    }
}

TEST_CASE("audio_data | int24 to float", "[audio_data]") {
    SECTION("Convert int24 to float be to be") {
        rav::vector_stream<rav::int24_t> src;
        src.push_back_be({-8388608, 8388607, 0});
        rav::vector_stream<float> dst(3);

        auto result = rav::audio_data::convert<
            rav::int24_t, byte_order::be, interleaving::interleaved, float, byte_order::be, interleaving::interleaved>(
            src.data(), src.size(), dst.data(), dst.size(), 1
        );

        REQUIRE(result);
        REQUIRE(rav::util::is_within(dst.read_be(), -1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read_be(), +1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read_be(), +0.f, f32_tolerance));
    }

    SECTION("Convert int24 to float be to le") {
        rav::vector_stream<rav::int24_t> src;
        src.push_back_be({-8388608, 8388607, 0});
        rav::vector_stream<float> dst(3);

        auto result = rav::audio_data::convert<
            rav::int24_t, byte_order::be, interleaving::interleaved, float, byte_order::le,
            interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

        REQUIRE(result);
        REQUIRE(rav::util::is_within(dst.read_le(), -1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read_le(), +1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read_le(), +0.f, f32_tolerance));
    }

    SECTION("Convert int24 to float be to ne") {
        rav::vector_stream<rav::int24_t> src;
        src.push_back_be({-8388608, 8388607, 0});
        rav::vector_stream<float> dst(3);

        auto result = rav::audio_data::convert<
            rav::int24_t, byte_order::be, interleaving::interleaved, float, byte_order::ne,
            interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

        REQUIRE(result);
        REQUIRE(rav::util::is_within(dst.read(), -1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read(), +1.f, f32_tolerance));
        REQUIRE(rav::util::is_within(dst.read(), +0.f, f32_tolerance));
    }
}

TEST_CASE("audio_data | int16 to double", "[audio_data]") {
     SECTION("Convert int16 to double be to be") {
         rav::vector_stream<int16_t> src;
         src.push_back_be({-32768, 32767, 0});
         rav::vector_stream<double> dst(3);

         auto result = rav::audio_data::convert<
             int16_t, byte_order::be, interleaving::interleaved, double, byte_order::be,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(rav::util::is_within(dst.read_be(), -1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read_be(), +1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read_be(), +0.0, f64_tolerance));
     }

     SECTION("Convert int16 to double be to le") {
         rav::vector_stream<int16_t> src;
         src.push_back_be({-32768, 32767, 0});
         rav::vector_stream<double> dst(3);

         auto result = rav::audio_data::convert<
             int16_t, byte_order::be, interleaving::interleaved, double, byte_order::le,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(rav::util::is_within(dst.read_le(), -1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read_le(), +1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read_le(), +0.0, f64_tolerance));
     }

     SECTION("Convert int16 to double be to ne") {
         rav::vector_stream<int16_t> src;
         src.push_back_be({-32768, 32767, 0});
         rav::vector_stream<double> dst(3);

         auto result = rav::audio_data::convert<
             int16_t, byte_order::be, interleaving::interleaved, double, byte_order::ne,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(rav::util::is_within(dst.read(), -1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read(), +1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read(), +0.0, f64_tolerance));
     }
}

TEST_CASE("audio_data | int24 to double", "[audio_data]") {
     SECTION("Convert int24 to double be to be") {
         rav::vector_stream<rav::int24_t> src;
         src.push_back_be({-8388608, 8388607, 0});
         rav::vector_stream<double> dst(3);

         auto result = rav::audio_data::convert<
             rav::int24_t, byte_order::be, interleaving::interleaved, double, byte_order::be,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(rav::util::is_within(dst.read_be(), -1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read_be(), +1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read_be(), +0.0, f64_tolerance));
     }

     SECTION("Convert int24 to double be to le") {
         rav::vector_stream<rav::int24_t> src;
         src.push_back_be({-8388608, 8388607, 0});
         rav::vector_stream<double> dst(3);

         auto result = rav::audio_data::convert<
             rav::int24_t, byte_order::be, interleaving::interleaved, double, byte_order::le,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(rav::util::is_within(dst.read_le(), -1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read_le(), +1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read_le(), +0.0, f64_tolerance));
     }

     SECTION("Convert int24 to double be to ne") {
         rav::vector_stream<rav::int24_t> src;
         src.push_back_be({-8388608, 8388607, 0});
         rav::vector_stream<double> dst(3);

         auto result = rav::audio_data::convert<
             rav::int24_t, byte_order::be, interleaving::interleaved, double, byte_order::ne,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(rav::util::is_within(dst.read(), -1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read(), +1.0, f64_tolerance));
         REQUIRE(rav::util::is_within(dst.read(), +0.0, f64_tolerance));
     }
}

TEST_CASE("audio_data | float to int16", "[audio_data]") {
     SECTION("Convert float to int16 be to be") {
         rav::vector_stream<float> src;
         src.push_back_be({-1.f, 1.f, 0.f});
         rav::vector_stream<int16_t> dst(3);

         auto result = rav::audio_data::convert<
             float, byte_order::be, interleaving::interleaved, int16_t, byte_order::be,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(dst.read_be() == -32767);
         REQUIRE(dst.read_be() == 32767);
         REQUIRE(dst.read_be() == 0);
     }

     SECTION("Convert float to int16 be to le") {
         rav::vector_stream<float> src;
         src.push_back_be({-1.f, 1.f, 0.f});
         rav::vector_stream<int16_t> dst(3);

         auto result = rav::audio_data::convert<
             float, byte_order::be, interleaving::interleaved, int16_t, byte_order::le,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(dst.read_le() == -32767);
         REQUIRE(dst.read_le() == 32767);
         REQUIRE(dst.read_le() == 0);
     }

     SECTION("Convert float to int16 le to be") {
         rav::vector_stream<float> src;
         src.push_back_le({-1.f, 1.f, 0.f});
         rav::vector_stream<int16_t> dst(3);

         auto result = rav::audio_data::convert<
             float, byte_order::le, interleaving::interleaved, int16_t, byte_order::be,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(dst.read_be() == -32767);
         REQUIRE(dst.read_be() == 32767);
         REQUIRE(dst.read_be() == 0);
     }
}

TEST_CASE("audio_data | float to int24", "[audio_data]") {
     SECTION("Convert float to int24 be to be") {
         rav::vector_stream<float> src;
         src.push_back_be({-1.f, 1.f, 0.f});
         rav::vector_stream<rav::int24_t> dst(3);

         auto result = rav::audio_data::convert<
             float, byte_order::be, interleaving::interleaved, rav::int24_t, byte_order::be,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(dst.read_be() == -8388607);
         REQUIRE(dst.read_be() == 8388607);
         REQUIRE(dst.read_be() == 0);
     }

     SECTION("Convert float to int24 be to le") {
         rav::vector_stream<float> src;
         src.push_back_be({-1.f, 1.f, 0.f});
         rav::vector_stream<rav::int24_t> dst(3);

         auto result = rav::audio_data::convert<
             float, byte_order::be, interleaving::interleaved, rav::int24_t, byte_order::le,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(dst.read_le() == -8388607);
         REQUIRE(dst.read_le() == 8388607);
         REQUIRE(dst.read_le() == 0);
     }

     SECTION("Convert float to int24 le to be") {
         rav::vector_stream<float> src;
         src.push_back_le({-1.f, 1.f, 0.f});
         rav::vector_stream<rav::int24_t> dst(3);

         auto result = rav::audio_data::convert<
             float, byte_order::le, interleaving::interleaved, rav::int24_t, byte_order::be,
             interleaving::interleaved>(src.data(), src.size(), dst.data(), dst.size(), 1);

         REQUIRE(result);
         REQUIRE(dst.read_be() == -8388607);
         REQUIRE(dst.read_be() == 8388607);
         REQUIRE(dst.read_be() == 0);
     }
}
