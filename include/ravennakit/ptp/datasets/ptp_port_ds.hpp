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

#include "ravennakit/ptp/ptp_definitions.hpp"
#include "ravennakit/ptp/ptp_profiles.hpp"
#include "ravennakit/ptp/types/ptp_port_identity.hpp"
#include "ravennakit/ptp/types/ptp_timestamp.hpp"

namespace rav::ptp {

/**
 * Port data set. IEEE 1588-2019: 8.2.15.
 */
struct PortDs {
    PortIdentity port_identity;
    State port_state {State::undefined};
    /// Valid range: [0,5]
    int8_t log_min_delay_req_interval {0};  // Required for e2e only
    TimeInterval mean_link_delay;           // Required for p2p only

    /// Specifies the mean time interval between successive Announce messages. Should be uniform throughout a domain.
    /// IEEE 1588-2019: 7.7.2.2
    int8_t log_announce_interval {1};

    /// Number of announceIntervals. Should be uniform throughout a domain. Recommended is at least 3.
    /// IEEE 1588-2019: 7.7.3.1
    uint8_t announce_receipt_timeout {3};

    /// Sync interval.
    /// IEEE 1588-2019: 7.7.2.3
    int8_t log_sync_interval {1};

    DelayMechanism delay_mechanism {};       // Required for p2p only
    int8_t log_min_pdelay_req_interval {0};  // Required for p2p only
    uint8_t version_number {2};              // 4 bits on the wire (one nibble)
    uint8_t minor_version_number {1};        // 4 bits on the wire (one nibble)
    TimeInterval delay_asymmetry;

    /**
     * Checks the internal state of this object according to IEEE1588-2019. Asserts when something is wrong.
     * @param profile The profile to check against.
     */
    void assert_valid_state(const Profile& profile) const {
        port_identity.assert_valid_state();
        RAV_ASSERT(port_state != State::undefined, "port_state is undefined");
        RAV_ASSERT(profile.port_ds.log_announce_interval_range.contains(log_announce_interval), "log_announce_interval is out of range");
        RAV_ASSERT(profile.port_ds.log_sync_interval_range.contains(log_sync_interval), "log_sync_interval is out of range");
        RAV_ASSERT(
            profile.port_ds.log_min_delay_req_interval_range.contains(log_min_delay_req_interval),
            "log_min_delay_req_interval is out of range"
        );
        RAV_ASSERT(
            profile.port_ds.announce_receipt_timeout_range.contains(announce_receipt_timeout), "announce_receipt_timeout is out of range"
        );
        if (profile.port_ds.log_pdelay_req_interval_default.has_value()) {
            RAV_ASSERT(
                profile.port_ds.log_pdelay_req_interval_range.value().contains(log_min_pdelay_req_interval),
                "log_min_pdelay_req_interval is out of range"
            );
        }
    }
};

}  // namespace rav::ptp
