/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
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
