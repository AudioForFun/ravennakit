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

#include "ravennakit/core/containers/byte_buffer.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::ByteBuffer") {
    rav::ByteBuffer buffer;
    buffer.write_be<uint8_t>(0x12);
    buffer.write_be<uint16_t>(0x3456);
    buffer.write_be<uint32_t>(0x789ABCDE);
    buffer.write_be<uint64_t>(0xFEDCBA9876543210);
    REQUIRE(buffer.size() == 15);
    REQUIRE(buffer.data()[0] == 0x12);
    REQUIRE(buffer.data()[1] == 0x34);
    REQUIRE(buffer.data()[2] == 0x56);
    REQUIRE(buffer.data()[3] == 0x78);
    REQUIRE(buffer.data()[4] == 0x9A);
    REQUIRE(buffer.data()[5] == 0xBC);
    REQUIRE(buffer.data()[6] == 0xDE);
    REQUIRE(buffer.data()[7] == 0xFE);
    REQUIRE(buffer.data()[8] == 0xDC);
    REQUIRE(buffer.data()[9] == 0xBA);
    REQUIRE(buffer.data()[10] == 0x98);
    REQUIRE(buffer.data()[11] == 0x76);
    REQUIRE(buffer.data()[12] == 0x54);
    REQUIRE(buffer.data()[13] == 0x32);
    REQUIRE(buffer.data()[14] == 0x10);
}
