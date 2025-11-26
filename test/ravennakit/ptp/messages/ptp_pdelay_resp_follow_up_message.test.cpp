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
#include "ravennakit/ptp/messages/ptp_pdelay_resp_follow_up_message.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::ptp::PdelayRespFollowUpMessage") {
    SECTION("Unpack") {
        std::array<const uint8_t, 30> data {
            0x12, 0x34, 0x56, 0x78, 0x90, 0x12,              // ts seconds
            0x34, 0x56, 0x78, 0x90,                          // ts nanoseconds
            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,  // port identity
            0x99, 0xaa                                       // Port number
        };
        auto msg = rav::ptp::PdelayRespFollowUpMessage::from_data(rav::BufferView(data)).value();
        REQUIRE(msg.response_origin_timestamp.raw_seconds() == 0x123456789012);
        REQUIRE(msg.response_origin_timestamp.raw_nanoseconds() == 0x34567890);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[0] == 0x11);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[1] == 0x22);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[2] == 0x33);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[3] == 0x44);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[4] == 0x55);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[5] == 0x66);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[6] == 0x77);
        REQUIRE(msg.requesting_port_identity.clock_identity.data[7] == 0x88);
    }
}
