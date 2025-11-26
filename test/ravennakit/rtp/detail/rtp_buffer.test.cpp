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

#include "ravennakit/rtp/detail/rtp_ringbuffer.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::rtp::Ringbuffer") {
    SECTION("Read with wraparound") {
        rav::rtp::Ringbuffer buffer;
        buffer.resize(10, 2);

        std::array<const uint8_t, 4> input = {0x0, 0x1, 0x2, 0x3};
        std::array<uint8_t, 4> output = {0x0, 0x1, 0x2, 0x3};

        const rav::BufferView buffer_view(input.data(), input.size());
        buffer.write(4, buffer_view);
        REQUIRE(buffer.get_next_ts().value() == 6);

        buffer.read(0, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x0, 0x0, 0x0, 0x0});

        buffer.read(2, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x0, 0x0, 0x0, 0x0});

        buffer.read(4, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x0, 0x1, 0x2, 0x3});

        buffer.read(6, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x0, 0x0, 0x0, 0x0});

        buffer.read(8, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x0, 0x0, 0x0, 0x0});

        // Here the wraparound happens
        buffer.read(10, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x0, 0x0, 0x0, 0x0});

        buffer.read(12, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x0, 0x0, 0x0, 0x0});

        // This timestamp matches 4
        buffer.read(14, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x0, 0x1, 0x2, 0x3});
    }

    SECTION("Fill buffer in one go") {
        rav::rtp::Ringbuffer buffer;
        buffer.resize(4, 2);

        std::array<const uint8_t, 8> input = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
        std::array<uint8_t, 4> output = {};

        const rav::BufferView buffer_view(input.data(), input.size());
        buffer.write(2, buffer_view);
        REQUIRE(buffer.get_next_ts().value() == 6);

        buffer.read(2, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x1, 0x2, 0x3, 0x4});
        buffer.read(0, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 4> {0x5, 0x6, 0x7, 0x8});
    }

    SECTION("Clear until") {
        rav::rtp::Ringbuffer buffer;
        buffer.resize(4, 2);

        std::array<const uint8_t, 8> input = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
        std::array<uint8_t, 8> output = {};

        const rav::BufferView buffer_view(input.data(), input.size());
        buffer.write(2, buffer_view);
        REQUIRE(buffer.get_next_ts().value() == 6);

        buffer.read(2, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 8> {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8});

        REQUIRE_FALSE(buffer.clear_until(6));
        REQUIRE(buffer.clear_until(8));
        buffer.read(2, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 8> {0x0, 0x0, 0x0, 0x0, 0x5, 0x6, 0x7, 0x8});

        buffer.read(4, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 8> {0x5, 0x6, 0x7, 0x8, 0x0, 0x0, 0x0, 0x0});

        buffer.set_ground_value(0xFF);
        REQUIRE(buffer.clear_until(10));

        buffer.read(4, output.data(), output.size());
        REQUIRE(output == std::array<uint8_t, 8> {0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0});
    }

    SECTION("Clear until some high timestamp") {
        rav::rtp::Ringbuffer buffer;
        buffer.resize(480, 2);
        buffer.clear_until(1000);
    }

    SECTION("Clear until some crazy high timestamp") {
        rav::rtp::Ringbuffer buffer;
        buffer.resize(480, 2);
        buffer.clear_until(253366016);
    }

    SECTION("Clear after reading") {
        rav::rtp::Ringbuffer buffer;
        buffer.resize(4, 2);

        std::array<const uint8_t, 8> input = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
        std::array<uint8_t, 8> output = {};

        const rav::BufferView buffer_view(input.data(), input.size());
        buffer.write(2, buffer_view);
        buffer.read(2, output.data(), output.size(), true);
        REQUIRE(output == std::array<uint8_t, 8> {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8});

        buffer.read(2, output.data(), output.size(), true);
        REQUIRE(output == std::array<uint8_t, 8> {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0});
    }
}
