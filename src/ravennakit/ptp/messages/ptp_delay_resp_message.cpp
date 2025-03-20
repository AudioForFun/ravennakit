/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/ptp/messages/ptp_delay_resp_message.hpp"

tl::expected<rav::ptp::DelayRespMessage, rav::ptp::Error>
rav::ptp::DelayRespMessage::from_data(const MessageHeader& header, const BufferView<const uint8_t> data) {
    if (data.size() < k_message_size) {
        return tl::unexpected(Error::invalid_message_length);
    }
    DelayRespMessage msg;
    msg.header = header;
    msg.receive_timestamp = Timestamp::from_data(data);
    auto port_identity = PortIdentity::from_data(data.subview(10));
    if (!port_identity) {
        return tl::unexpected(port_identity.error());
    }
    msg.requesting_port_identity = port_identity.value();
    return msg;
}

std::string rav::ptp::DelayRespMessage::to_string() const {
    return fmt::format(
        "receive_timestamp={} requesting_port_identity={}", receive_timestamp.to_string(),
        requesting_port_identity.to_string()
    );
}