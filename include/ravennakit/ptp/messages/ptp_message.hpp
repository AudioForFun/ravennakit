/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#pragma once

#include "ptp_announce_message.hpp"
#include "ptp_delay_req_message.hpp"

namespace rav {

using ptp_message_variant = std::variant<ptp_announce_message, ptp_sync_message, ptp_delay_req_message>;

struct ptp_message {
    ptp_message_variant message;

    static tl::expected<ptp_message, ptp_error> from_data(buffer_view<const uint8_t> data);

    [[nodiscard]] std::string to_string() const;
};

}
