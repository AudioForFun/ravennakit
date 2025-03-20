/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/ptp/messages/ptp_follow_up_message.hpp"

#include "ravennakit/ptp/messages/ptp_message_header.hpp"

tl::expected<rav::ptp::FollowUpMessage, rav::ptp::Error>
rav::ptp::FollowUpMessage::from_data(const MessageHeader& header, const BufferView<const uint8_t> data) {
    if (data.size() < k_message_size) {
        return tl::unexpected(Error::invalid_message_length);
    }

    FollowUpMessage msg;
    msg.header = header;
    msg.precise_origin_timestamp = Timestamp::from_data(data);
    return msg;
}

std::string rav::ptp::FollowUpMessage::to_string() const {
    return fmt::format("precise_origin_timestamp={}", precise_origin_timestamp.to_string());
}
