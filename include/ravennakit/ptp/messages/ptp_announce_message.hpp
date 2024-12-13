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

#include "ptp_message_header.hpp"

namespace rav {

struct ptp_announce_message {
    ptp_message_header header;
    ptp_timestamp origin_timestamp;
    int16_t current_utc_offset {};
    uint8_t grandmaster_priority1 {};
    ptp_clock_quality grandmaster_clock_quality;
    uint8_t grandmaster_priority2 {};
    ptp_clock_identity grandmaster_identity;
    uint16_t steps_removed {};
    ptp_time_source gm_time_base_indicator{};

    static tl::expected<ptp_announce_message, ptp_error> from_data(buffer_view<const uint8_t> data);

    [[nodiscard]] std::string to_string() const;
private:
    constexpr static size_t k_message_size = 30; // Excluding header size
};

}
