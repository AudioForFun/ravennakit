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

#include "ravennakit/core/streams/byte_stream.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::ByteStream") {
    SECTION("Read") {
        rav::ByteStream stream;

        REQUIRE(stream.write_ne<uint32_t>(1));
        REQUIRE(stream.write_ne<uint16_t>(2));
        REQUIRE(stream.write_ne<uint8_t>(3));
        REQUIRE(stream.write_ne<int64_t>(4));

        REQUIRE(stream.get_read_position() == 0);

        REQUIRE(stream.read_ne<uint32_t>() == 1);
        REQUIRE(stream.read_ne<uint16_t>() == 2);
        REQUIRE(stream.read_ne<uint8_t>() == 3);
        REQUIRE(stream.read_ne<int64_t>() == 4);

        REQUIRE(stream.read_ne<int64_t>().has_value() == false);
    }

    SECTION("Set read position") {
        rav::ByteStream stream;
        REQUIRE(stream.write_ne<uint32_t>(1));

        REQUIRE(stream.read_ne<uint32_t>() == 1);
        REQUIRE(stream.set_read_position(0) == true);
        REQUIRE(stream.read_ne<uint32_t>() == 1);
        REQUIRE(stream.set_read_position(5) == false);
    }

    SECTION("Get read position") {
        rav::ByteStream stream;
        REQUIRE(stream.write_ne<uint32_t>(1));
        REQUIRE(stream.get_read_position() == 0);
        REQUIRE(stream.read_ne<uint32_t>());
        REQUIRE(stream.get_read_position() == 4);
    }

    SECTION("Get read position") {
        rav::ByteStream stream;
        REQUIRE(stream.size() == 0);
        REQUIRE(stream.write_ne<uint32_t>(1));
        REQUIRE(stream.size() == 4);
    }

    SECTION("Set write position") {
        rav::ByteStream stream;
        REQUIRE(stream.write_ne<uint32_t>(1));
        REQUIRE(stream.set_write_position(0));
        REQUIRE(stream.write_ne<uint32_t>(1));
        REQUIRE(stream.set_write_position(10));
        REQUIRE(stream.get_write_position() == 10);
        REQUIRE(stream.size() == 4);
        REQUIRE(stream.write_ne<uint32_t>(1));
        REQUIRE(stream.size() == 14);
        REQUIRE(stream.get_write_position() == 14);
    }

    SECTION("Flush") {
        rav::ByteStream stream;
        REQUIRE(stream.write_ne<uint32_t>(1));
        stream.flush();
    }

    SECTION("Construct with data") {
        rav::ByteStream stream({0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8});
        REQUIRE(stream.get_read_position() == 0);
        REQUIRE(stream.get_write_position() == 8);
        REQUIRE(stream.size() == 8);
    }
}
