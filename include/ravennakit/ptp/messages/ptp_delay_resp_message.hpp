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
#include "ravennakit/ptp/ptp_error.hpp"
#include "ravennakit/ptp/types/ptp_port_identity.hpp"
#include "ravennakit/ptp/types/ptp_timestamp.hpp"

#include "ravennakit/core/expected.hpp"

namespace rav::ptp {

struct DelayRespMessage {
    MessageHeader header;
    Timestamp receive_timestamp;
    PortIdentity requesting_port_identity;

    /**
     * Create a ptp_announce_message from a buffer_view.
     * @param header The message header belonging to the message.
     * @param data The message data. Expects it to start at the beginning of the message, excluding the header.
     * @return A ptp_announce_message if the data is valid, otherwise a ptp_error.
     */
    static tl::expected<DelayRespMessage, Error> from_data(const MessageHeader& header, BufferView<const uint8_t> data);

    /**
     * @returns A string representation of the ptp_announce_message.
     */
    [[nodiscard]] std::string to_string() const;

  private:
    constexpr static size_t k_message_size = 20;  // Excluding header size
};

}  // namespace rav::ptp
