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
#include "ravennakit/ptp/types/ptp_clock_quality.hpp"
#include "ravennakit/ptp/types/ptp_timestamp.hpp"

namespace rav::ptp {

struct AnnounceMessage {
    MessageHeader header;
    Timestamp origin_timestamp;
    int16_t current_utc_offset {};  // Seconds
    uint8_t grandmaster_priority1 {};
    ClockQuality grandmaster_clock_quality;
    uint8_t grandmaster_priority2 {};
    ClockIdentity grandmaster_identity;
    uint16_t steps_removed {};
    TimeSource time_source {};

    /**
     * Create a ptp_announce_message from a buffer_view.
     * @param header
     * @param data The message data. Expects it to start at the beginning of the message, excluding the header.
     * @return A ptp_announce_message if the data is valid, otherwise a ptp_error.
     */
    static tl::expected<AnnounceMessage, Error>
    from_data(const MessageHeader& header, BufferView<const uint8_t> data);

    /**
     * @returns A string representation of the ptp_announce_message.
     */
    [[nodiscard]] std::string to_string() const;

    /**
     * @return A string representation of the source of the message.
     */
    [[nodiscard]] std::string source_to_string() const {
        return header.source_port_identity.clock_identity.to_string();
    }

  private:
    constexpr static size_t k_message_size = 30;  // Excluding header size
};

}  // namespace rav
