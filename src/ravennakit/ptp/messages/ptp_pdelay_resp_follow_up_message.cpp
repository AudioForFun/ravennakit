/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/ptp/messages/ptp_pdelay_resp_follow_up_message.hpp"

tl::expected<rav::ptp::PdelayRespFollowUpMessage, rav::ptp::Error>
rav::ptp::PdelayRespFollowUpMessage::from_data(const BufferView<const uint8_t> data) {
    if (data.size() < k_message_size) {
        return tl::make_unexpected(Error::invalid_message_length);
    }

    PdelayRespFollowUpMessage msg;
    msg.response_origin_timestamp = Timestamp::from_data(data);
    auto port_identity = PortIdentity::from_data(data.subview(Timestamp::k_size));
    if (!port_identity) {
        return tl::make_unexpected(port_identity.error());
    }
    msg.requesting_port_identity = port_identity.value();
    return msg;
}

std::string rav::ptp::PdelayRespFollowUpMessage::to_string() const {
    return fmt::format(
        "response_origin_timestamp={} requesting_port_identity={}", response_origin_timestamp.to_string(),
        requesting_port_identity.to_string()
    );
}
