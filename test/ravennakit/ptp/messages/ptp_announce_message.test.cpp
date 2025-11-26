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
#include "ravennakit/ptp/messages/ptp_announce_message.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::ptp::AnnounceMessage") {
    SECTION("Unpack from data") {
        constexpr std::array<const uint8_t, 30> data = {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06,              // origin_timestamp.seconds
            0x07, 0x08, 0x09, 0x0a,                          // origin_timestamp.nanoseconds
            0x0b, 0x0c,                                      // current_utc_offset
            0x00,                                            // reserved
            0x0d,                                            // grandmaster_priority1
            0x0e, 0x20, 0x10, 0x11,                          // grandmaster_clock_quality
            0x12,                                            // grandmaster_priority2
            0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,  // grandmaster_identity
            0x1b, 0x1c,                                      // steps_removed
            0x40,                                            // time source
        };

        auto announce = rav::ptp::AnnounceMessage::from_data({}, rav::BufferView(data));
        REQUIRE(announce);
        REQUIRE(announce->origin_timestamp.raw_seconds() == 0x010203040506);
        REQUIRE(announce->origin_timestamp.raw_nanoseconds() == 0x0708090a);
        REQUIRE(announce->current_utc_offset == 0x0b0c);
        REQUIRE(announce->grandmaster_priority1 == 0x0d);
        REQUIRE(announce->grandmaster_clock_quality.clock_class == 0x0e);
        REQUIRE(announce->grandmaster_clock_quality.clock_accuracy == rav::ptp::ClockAccuracy::lt_25_ns);
        REQUIRE(announce->grandmaster_clock_quality.offset_scaled_log_variance == 0x1011);
        REQUIRE(announce->grandmaster_priority2 == 0x12);
        REQUIRE(announce->grandmaster_identity.data[0] == 0x13);
        REQUIRE(announce->grandmaster_identity.data[1] == 0x14);
        REQUIRE(announce->grandmaster_identity.data[2] == 0x15);
        REQUIRE(announce->grandmaster_identity.data[3] == 0x16);
        REQUIRE(announce->grandmaster_identity.data[4] == 0x17);
        REQUIRE(announce->grandmaster_identity.data[5] == 0x18);
        REQUIRE(announce->grandmaster_identity.data[6] == 0x19);
        REQUIRE(announce->grandmaster_identity.data[7] == 0x1a);
        REQUIRE(announce->steps_removed == 0x1b1c);
        REQUIRE(announce->time_source == rav::ptp::TimeSource::ptp);
    }
}
