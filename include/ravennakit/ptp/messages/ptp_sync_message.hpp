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

#pragma once

#include "ptp_message_header.hpp"
#include "ravennakit/core/containers/byte_buffer.hpp"
#include "ravennakit/ptp/types/ptp_timestamp.hpp"

namespace rav::ptp {

struct SyncMessage {
    constexpr static size_t k_message_length = MessageHeader::k_header_size + 10;

    MessageHeader header;
    Timestamp origin_timestamp;
    Timestamp receive_timestamp;  // Not part of the message on the wire, but used for calculations

    /**
     * Create a ptp_announce_message from a buffer_view.
     * @param header The header of the message.
     * @param data The message data. Expects it to start at the beginning of the message, excluding the header.
     * @return A ptp_announce_message if the data is valid, otherwise a ptp_error.
     */
    static tl::expected<SyncMessage, Error> from_data(const MessageHeader& header, BufferView<const uint8_t> data);

    /**
     * Write the ptp_announce_message to a byte buffer.
     * @param buffer The buffer to write to.
     */
    void write_to(ByteBuffer& buffer) const;

    /**
     * @returns A string representation of the ptp_announce_message.
     */
    [[nodiscard]] std::string to_string() const;
};

}  // namespace rav::ptp
