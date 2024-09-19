/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include <array>
#include <catch2/catch_all.hpp>

#include "ravennakit/containers/vector_stream.hpp"

TEST_CASE("vector_stream", "[vector_stream]") {
    rav::vector_stream stream;

    SECTION("write") {
        REQUIRE(stream.write<uint32_t>(0x12345678));
        REQUIRE(stream.write<uint32_t>(0x87654321));
        REQUIRE(stream.write<uint16_t>(0x1234));
        REQUIRE(stream.write<uint16_t>(0x5678));
        REQUIRE(stream.write<uint8_t>(0x12));
        REQUIRE(stream.write<uint8_t>(0x34));
        REQUIRE(stream.write<uint8_t>(0x56));
        REQUIRE(stream.write<uint8_t>(0x78));

        if constexpr (rav::little_endian) {
            std::array<uint8_t, 16> expected {
                0x78, 0x56, 0x34, 0x12,  // uint32_t
                0x21, 0x43, 0x65, 0x87,  // uint32_t
                0x34, 0x12,              // uint16_t
                0x78, 0x56,              // uint16_t
                0x12, 0x34, 0x56, 0x78,  // 4x uint8_t
            };
            REQUIRE(std::memcmp(stream.data(), expected.data(), stream.size()) == 0);
        } else {
            std::array<uint8_t, 16> expected {
                0x12, 0x34, 0x56, 0x78,  // uint32_t
                0x87, 0x65, 0x43, 0x21,  // uint32_t
                0x12, 0x34,              // uint16_t
                0x56, 0x78,              // uint16_t
                0x12, 0x34, 0x56, 0x78   // 4x uint8_t
            };
            REQUIRE(std::memcmp(stream.data(), expected.data(), stream.size()) == 0);
        }
    }

    SECTION("write_be") {
        REQUIRE(stream.write_be<uint32_t>(0x12345678));
        REQUIRE(stream.write_be<uint32_t>(0x87654321));
        REQUIRE(stream.write_be<uint16_t>(0x1234));
        REQUIRE(stream.write_be<uint16_t>(0x5678));
        REQUIRE(stream.write_be<uint8_t>(0x12));
        REQUIRE(stream.write_be<uint8_t>(0x34));
        REQUIRE(stream.write_be<uint8_t>(0x56));
        REQUIRE(stream.write_be<uint8_t>(0x78));

        std::array<uint8_t, 16> expected {
            0x12, 0x34, 0x56, 0x78,  // uint32_t
            0x87, 0x65, 0x43, 0x21,  // uint32_t
            0x12, 0x34,              // uint16_t
            0x56, 0x78,              // uint16_t
            0x12, 0x34, 0x56, 0x78   // 4x uint8_t
        };

        REQUIRE(std::memcmp(stream.data(), expected.data(), stream.size()) == 0);

    }

    SECTION("write_le") {
        REQUIRE(stream.write_le<uint32_t>(0x12345678));
        REQUIRE(stream.write_le<uint32_t>(0x87654321));
        REQUIRE(stream.write_le<uint16_t>(0x1234));
        REQUIRE(stream.write_le<uint16_t>(0x5678));
        REQUIRE(stream.write_le<uint8_t>(0x12));
        REQUIRE(stream.write_le<uint8_t>(0x34));
        REQUIRE(stream.write_le<uint8_t>(0x56));
        REQUIRE(stream.write_le<uint8_t>(0x78));

        std::array<uint8_t, 16> expected {
            0x78, 0x56, 0x34, 0x12,  // uint32_t
            0x21, 0x43, 0x65, 0x87,  // uint32_t
            0x34, 0x12,              // uint16_t
            0x78, 0x56,              // uint16_t
            0x12, 0x34, 0x56, 0x78,  // 4x uint8_t
        };

        REQUIRE(std::memcmp(stream.data(), expected.data(), stream.size()) == 0);
    }

    SECTION("read") {
        stream.write<uint32_t>(0x12345678);
        stream.write<uint32_t>(0x87654321);
        stream.write<uint16_t>(0x1234);
        stream.write<uint16_t>(0x5678);
        stream.write<uint8_t>(0x12);
        stream.write<uint8_t>(0x34);
        stream.write<uint8_t>(0x56);
        stream.write<uint8_t>(0x78);

        REQUIRE(stream.read<uint32_t>() == 0x12345678);
        REQUIRE(stream.read<uint32_t>() == 0x87654321);
        REQUIRE(stream.read<uint16_t>() == 0x1234);
        REQUIRE(stream.read<uint16_t>() == 0x5678);
        REQUIRE(stream.read<uint8_t>() == 0x12);
        REQUIRE(stream.read<uint8_t>() == 0x34);
        REQUIRE(stream.read<uint8_t>() == 0x56);
        REQUIRE(stream.read<uint8_t>() == 0x78);
        REQUIRE(stream.read<uint8_t>() == 0);
    }

    SECTION("read_le") {
        stream.write_le<uint32_t>(0x12345678);
        stream.write_le<uint32_t>(0x87654321);
        stream.write_le<uint16_t>(0x1234);
        stream.write_le<uint16_t>(0x5678);
        stream.write_le<uint8_t>(0x12);
        stream.write_le<uint8_t>(0x34);
        stream.write_le<uint8_t>(0x56);
        stream.write_le<uint8_t>(0x78);

        REQUIRE(stream.read_le<uint32_t>() == 0x12345678);
        REQUIRE(stream.read_le<uint32_t>() == 0x87654321);
        REQUIRE(stream.read_le<uint16_t>() == 0x1234);
        REQUIRE(stream.read_le<uint16_t>() == 0x5678);
        REQUIRE(stream.read_le<uint8_t>() == 0x12);
        REQUIRE(stream.read_le<uint8_t>() == 0x34);
        REQUIRE(stream.read_le<uint8_t>() == 0x56);
        REQUIRE(stream.read_le<uint8_t>() == 0x78);
        REQUIRE(stream.read_le<uint8_t>() == 0);
    }

    SECTION("read_be") {
        stream.write_be<uint32_t>(0x12345678);
        stream.write_be<uint32_t>(0x87654321);
        stream.write_be<uint16_t>(0x1234);
        stream.write_be<uint16_t>(0x5678);
        stream.write_be<uint8_t>(0x12);
        stream.write_be<uint8_t>(0x34);
        stream.write_be<uint8_t>(0x56);
        stream.write_be<uint8_t>(0x78);

        REQUIRE(stream.read<uint32_t>() == 0x78563412);
        REQUIRE(stream.read<uint32_t>() == 0x21436587);
        REQUIRE(stream.read<uint16_t>() == 0x3412);
        REQUIRE(stream.read<uint16_t>() == 0x7856);
        REQUIRE(stream.read<uint8_t>() == 0x12);
        REQUIRE(stream.read<uint8_t>() == 0x34);
        REQUIRE(stream.read<uint8_t>() == 0x56);
        REQUIRE(stream.read<uint8_t>() == 0x78);
        REQUIRE(stream.read<uint8_t>() == 0);
    }

    SECTION("write le read be") {
        stream.write_le<uint32_t>(0x12345678);
        REQUIRE(stream.read_be<uint32_t>() == 0x78563412);
    }

    SECTION("write be read le") {
        stream.write_be<uint32_t>(0x12345678);
        REQUIRE(stream.read_le<uint32_t>() == 0x78563412);
    }

    SECTION("reset") {
        stream.write_be<uint32_t>(0x12345678);
        stream.reset();
        REQUIRE(stream.size() == 0);
    }
}
