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

#include "ravennakit/ptp/messages/ptp_announce_message.hpp"
#include "ravennakit/core/byte_order.hpp"

tl::expected<rav::ptp::AnnounceMessage, rav::ptp::Error>
rav::ptp::AnnounceMessage::from_data(const MessageHeader& header, BufferView<const uint8_t> data) {
    if (data.size() < k_message_size) {
        return tl::unexpected(Error::invalid_message_length);
    }

    AnnounceMessage msg;
    msg.header = header;
    msg.origin_timestamp = Timestamp::from_data(data);
    msg.current_utc_offset = data.read_be<int16_t>(10);
    // Byte 12 is reserved
    msg.grandmaster_priority1 = data[13];
    msg.grandmaster_clock_quality.clock_class = data[14];
    msg.grandmaster_clock_quality.clock_accuracy = static_cast<ClockAccuracy>(data[15]);
    msg.grandmaster_clock_quality.offset_scaled_log_variance = data.read_be<uint16_t>(16);
    msg.grandmaster_priority2 = data[18];
    msg.grandmaster_identity = ClockIdentity::from_data(data.subview(19));
    msg.steps_removed = data.read_be<uint16_t>(27);
    msg.time_source = static_cast<TimeSource>(data[29]);
    return msg;
}

std::string rav::ptp::AnnounceMessage::to_string() const {
    return fmt::format(
        "{} origin_timestamp={}.{:09d} current_utc_offset={} gm_priority1={} gm_clock_quality=({})", header.to_string(),
        origin_timestamp.raw_seconds(), origin_timestamp.raw_nanoseconds(), current_utc_offset, grandmaster_priority1,
        grandmaster_clock_quality.to_string()
    );
}
