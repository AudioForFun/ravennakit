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

#include "ravennakit/ptp/messages/ptp_sync_message.hpp"

tl::expected<rav::ptp::SyncMessage, rav::ptp::Error>
rav::ptp::SyncMessage::from_data(const MessageHeader& header, const BufferView<const uint8_t> data) {
    if (data.size() < k_message_length - MessageHeader::k_header_size) {
        return tl::unexpected(Error::invalid_message_length);
    }

    SyncMessage msg;
    msg.header = header;
    msg.origin_timestamp = Timestamp::from_data(data);
    return msg;
}

void rav::ptp::SyncMessage::write_to(ByteBuffer& buffer) const {
    header.write_to(buffer);
    origin_timestamp.write_to(buffer);
}

std::string rav::ptp::SyncMessage::to_string() const {
    return fmt::format("origin_timestamp={}", origin_timestamp.to_string());
}
