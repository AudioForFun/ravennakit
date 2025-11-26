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

#include "nmos_constraint.hpp"

namespace rav::nmos {

/**
 * Used to express the dynamic constraints on RTP transport parameters. These constraints may be set and changed at run
 * time. Every transport parameter must have an entry, even if it is only an empty object.
 */
struct ConstraintsRtp {
    Constraint source_ip;
    Constraint destination_port;
    Constraint rtp_enabled;
    std::optional<Constraint> source_port;     // Required for senders
    std::optional<Constraint> destination_ip;  // Required for senders
    std::optional<Constraint> interface_ip;    // Required for receivers
    std::optional<Constraint> multicast_ip;    // Required for receivers if supported
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const ConstraintsRtp& constraint_rtp) {
    boost::json::object obj {
        {"source_ip", boost::json::value_from(constraint_rtp.source_ip)},
        {"destination_port", boost::json::value_from(constraint_rtp.destination_port)},
        {"rtp_enabled", boost::json::value_from(constraint_rtp.rtp_enabled)},
    };

    if (constraint_rtp.source_port.has_value()) {
        obj["source_port"] = boost::json::value_from(constraint_rtp.source_port);
    }
    if (constraint_rtp.destination_ip.has_value()) {
        obj["destination_ip"] = boost::json::value_from(constraint_rtp.destination_ip);
    }
    if (constraint_rtp.interface_ip.has_value()) {
        obj["interface_ip"] = boost::json::value_from(constraint_rtp.interface_ip);
    }
    if (constraint_rtp.multicast_ip.has_value()) {
        obj["multicast_ip"] = boost::json::value_from(constraint_rtp.multicast_ip);
    }

    jv = std::move(obj);
}

}  // namespace rav::nmos
