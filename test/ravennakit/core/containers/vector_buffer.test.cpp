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

#include "ravennakit/core/containers/vector_buffer.hpp"

#include <array>
#include <catch2/catch_all.hpp>

TEST_CASE("rav::VectorBuffer") {
    rav::VectorBuffer<uint32_t> buffer;

    SECTION("write") {
        buffer.push_back(0x12345678);
        buffer.push_back(0x87654321);
        buffer.push_back(0x56);
        buffer.push_back(0x78);

        REQUIRE_FALSE(buffer == std::vector<uint32_t> {0x12345678});

        if constexpr (rav::little_endian) {
            REQUIRE(buffer == std::vector<uint32_t> {0x12345678, 0x87654321, 0x56, 0x78});
        } else {
            REQUIRE(buffer == std::vector<uint32_t> {0x78563412, 0x21436587, 0x56000000, 0x78000000});
        }
    }

    SECTION("write_be") {
        buffer.push_back_be(0x12345678);
        buffer.push_back_be(0x87654321);
        buffer.push_back_be({0x56, 0x78});

        REQUIRE(buffer == std::vector<uint32_t> {0x78563412, 0x21436587, 0x56000000, 0x78000000});
    }

    SECTION("write_le") {
        buffer.push_back_le(0x12345678);
        buffer.push_back_le(0x87654321);
        buffer.push_back_le(0x56);
        buffer.push_back_le(0x78);

        REQUIRE(buffer == std::vector<uint32_t> {0x12345678, 0x87654321, 0x56, 0x78});
    }

    SECTION("read") {
        buffer.push_back(0x12345678);
        buffer.push_back(0x87654321);
        buffer.push_back(0x56);
        buffer.push_back(0x78);

        REQUIRE(buffer.read() == 0x12345678);
        REQUIRE(buffer.read() == 0x87654321);
        REQUIRE(buffer.read() == 0x56);
        REQUIRE(buffer.read() == 0x78);
        REQUIRE(buffer.read() == 0);
    }

    SECTION("read_le") {
        buffer.push_back_le(0x12345678);
        buffer.push_back_le(0x87654321);
        buffer.push_back_le(0x56);
        buffer.push_back_le(0x78);

        REQUIRE(buffer.read_le() == 0x12345678);
        REQUIRE(buffer.read_le() == 0x87654321);
        REQUIRE(buffer.read_le() == 0x56);
        REQUIRE(buffer.read_le() == 0x78);
        REQUIRE(buffer.read_le() == 0);
    }

    SECTION("read_be") {
        buffer.push_back_be(0x12345678);
        buffer.push_back_be(0x87654321);
        buffer.push_back_be(0x56);
        buffer.push_back_be(0x78);

        REQUIRE(buffer.read_be() == 0x12345678);
        REQUIRE(buffer.read_be() == 0x87654321);
        REQUIRE(buffer.read_be() == 0x56);
        REQUIRE(buffer.read_be() == 0x78);
        REQUIRE(buffer.read_be() == 0);
    }

    SECTION("write le read be") {
        buffer.push_back_le(0x12345678);
        REQUIRE(buffer.read_be() == 0x78563412);
    }

    SECTION("write be read le") {
        buffer.push_back_be(0x12345678);
        REQUIRE(buffer.read_le() == 0x78563412);
    }

    SECTION("reset") {
        buffer.push_back_be(0x12345678);
        buffer.reset();
        REQUIRE(buffer.size() == 0); // NOLINT
        REQUIRE(buffer.empty());
    }
}
