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

#include "ravennakit/core/json.hpp"

#include <string>

namespace rav::nmos {

/**
 * Describes RTP Sender transport parameters. The constraints in this schema are minimum constraints, but may be further
 * constrained at the constraints' endpoint. As a minimum all senders must support `source_ip`, `destination_ip`,
 * `source_port`, `rtp_enabled` and `destination_port`. Senders supporting FEC and/or RTCP must support parameters
 * prefixed with `fec` and `rtcp` respectively.
 */
struct SenderTransportParamsRtp {
    /**
     * IP address from which RTP packets will be sent (IP address of interface bound to this output). The sender should
     * provide an enum in the constraints endpoint, which should contain the available interface addresses. If the
     * parameter is set to auto the sender should establish for itself which interface it should use, based on routing
     * rules or its own internal configuration.
     */
    std::optional<std::string> source_ip {};

    /**
     * IP address to which RTP packets will be sent. If auto is set the sender should select a multicast address to send
     * to itself. For example, it may implement MADCAP (RFC 2730), ZMAAP, or be allocated address by some other system
     * responsible for co-ordination multicast address use.
     */
    std::optional<std::string> destination_ip {};

    /**
     * Source port for RTP packets (auto = 5004 by default)
     */
    std::variant<std::monostate, int, std::string> source_port {};

    /**
     * destination port for RTP packets (auto = 5004 by default)
     */
    std::variant<std::monostate, int, std::string> destination_port {};

    /**
     * RTP transmission active/inactive
     */
    std::optional<bool> rtp_enabled {};
};

inline void
tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const SenderTransportParamsRtp& sender_transport_params) {
    jv = {
        {"source_ip", boost::json::value_from(sender_transport_params.source_ip)},
        {"destination_ip", boost::json::value_from(sender_transport_params.destination_ip)},
        {"source_port", boost::json::value_from(sender_transport_params.source_port)},
        {"destination_port", boost::json::value_from(sender_transport_params.destination_port)},
        {"rtp_enabled", boost::json::value_from(sender_transport_params.rtp_enabled)},
    };
}

inline SenderTransportParamsRtp tag_invoke(const boost::json::value_to_tag<SenderTransportParamsRtp>&, const boost::json::value& jv) {
    SenderTransportParamsRtp rtp {};
    if (const auto destination_ip = jv.try_at("destination_ip")) {
        rtp.destination_ip = destination_ip->as_string();
    }
    if (const auto destination_port = jv.try_at("destination_port")) {
        // rtp.destination_port = destination_port->to_number<uint16_t>();
        rtp.destination_port = boost::json::value_to<decltype(rtp.destination_port)>(*destination_port);
    }
    if (const auto source_ip = jv.try_at("source_ip")) {
        rtp.source_ip = source_ip->as_string();
    }
    if (const auto source_port = jv.try_at("source_port")) {
        // rtp.source_port = source_port->to_number<uint16_t>();
    }
    if (const auto rtp_enabled = jv.try_at("rtp_enabled")) {
        rtp.rtp_enabled = rtp_enabled->as_bool();
    }
    return rtp;
}

}  // namespace rav::nmos
