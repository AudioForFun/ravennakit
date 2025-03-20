/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/ptp/messages/ptp_pdelay_req_message.hpp"

tl::expected<rav::ptp::PdelayReqMessage, rav::ptp::Error>
rav::ptp::PdelayReqMessage::from_data(const BufferView<const uint8_t> data) {
    if (data.size() < k_message_size) {
        return tl::make_unexpected(Error::invalid_message_length);
    }

    PdelayReqMessage msg;
    msg.origin_timestamp = Timestamp::from_data(data);
    return msg;
}

void rav::ptp::PdelayReqMessage::write_to(ByteBuffer& buffer) const {
    return origin_timestamp.write_to(buffer);
}

std::string rav::ptp::PdelayReqMessage::to_string() const {
    return fmt::format("origin_timestamp={}", origin_timestamp.to_string());
}
