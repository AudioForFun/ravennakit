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
