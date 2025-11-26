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

#include <boost/asio.hpp>
#include "ravennakit/core/format.hpp"

namespace rav::rtp {

struct Session {
    boost::asio::ip::address connection_address;
    uint16_t rtp_port {};
    uint16_t rtcp_port {};

    [[nodiscard]] std::string to_string() const {
        return fmt::format("{}/{}/{}", connection_address.to_string(), rtp_port, rtcp_port);
    }

    [[nodiscard]] bool valid() const {
        return !connection_address.is_unspecified() && rtp_port != 0 && rtcp_port != 0;
    }

    friend auto operator==(const Session& lhs, const Session& rhs) -> bool {
        return std::tie(lhs.connection_address, lhs.rtp_port, lhs.rtcp_port)
            == std::tie(rhs.connection_address, rhs.rtp_port, rhs.rtcp_port);
    }

    friend bool operator!=(const Session& lhs, const Session& rhs) {
        return !(lhs == rhs);
    }
};

}  // namespace rav::rtp
