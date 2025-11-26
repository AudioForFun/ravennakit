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
#include "ravennakit/ptp/messages/ptp_delay_resp_message.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::ptp::DelayRespMessage") {
    SECTION("Unpack") {
        std::array<const uint8_t, 30> data {
            0x0,  0x1,  0x2,  0x3,  0x4,  0x5,               // ts seconds
            0x6,  0x7,  0x8,  0x9,                           // ts nanoseconds
            0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,  // clock identity
        };
        auto msg = rav::ptp::DelayRespMessage::from_data({}, rav::BufferView(data)).value();
        REQUIRE(msg.receive_timestamp.raw_seconds() == 0x102030405);
        REQUIRE(msg.receive_timestamp.raw_nanoseconds() == 0x06070809);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[0] == 0x10);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[1] == 0x20);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[2] == 0x30);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[3] == 0x40);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[4] == 0x50);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[5] == 0x60);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[6] == 0x70);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[7] == 0x80);
    }
}
