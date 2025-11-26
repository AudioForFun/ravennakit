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
#include "ravennakit/core/streams/input_stream_view.hpp"
#include "ravennakit/ptp/messages/ptp_sync_message.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::ptp::SyncMessage") {
    SECTION("Unpack") {
        rav::ByteStream stream;
        constexpr std::array<const uint8_t, 30> data {
            0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90,
        };
        auto sync = rav::ptp::SyncMessage::from_data({}, rav::BufferView(data)).value();
        REQUIRE(sync.origin_timestamp.raw_seconds() == 0x123456789012);
        REQUIRE(sync.origin_timestamp.raw_nanoseconds() == 0x34567890);
    }

    SECTION("Pack") {
        rav::ptp::SyncMessage sync;
        sync.origin_timestamp = rav::ptp::Timestamp(0x123456789012, 0x34567890);
        rav::ByteBuffer buffer;
        sync.write_to(buffer);

        rav::InputStreamView buffer_view(buffer);
        REQUIRE(buffer_view.size() == rav::ptp::SyncMessage::k_message_length);
        REQUIRE(buffer_view.skip(rav::ptp::MessageHeader::k_header_size));
        REQUIRE(buffer_view.read_be<rav::uint48_t>() == sync.origin_timestamp.raw_seconds());
        REQUIRE(buffer_view.read_be<uint32_t>() == sync.origin_timestamp.raw_nanoseconds());
    }
}
