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

#include "ptp_default_ds.hpp"
#include "ravennakit/ptp/messages/ptp_announce_message.hpp"
#include "ravennakit/ptp/types/ptp_clock_quality.hpp"
#include "ravennakit/ptp/types/ptp_port_identity.hpp"

namespace rav::ptp {

/**
 * Represents the parent data set as described in IEEE1588-2019: 8.2.3.
 */
struct ParentDs {
    PortIdentity parent_port_identity;
    bool parent_stats {};
    ClockIdentity grandmaster_identity;
    ClockQuality grandmaster_clock_quality;
    uint16_t grandmaster_priority1 {};
    uint8_t grandmaster_priority2 {};

    ParentDs() = default;

    explicit ParentDs(const DefaultDs& default_ds) {
        parent_port_identity.clock_identity = default_ds.clock_identity;  // IEEE1588-2019: 8.2.3.2
        parent_stats = false;                                             // IEEE1588-2019: 8.2.3.3
        grandmaster_identity = default_ds.clock_identity;                 // IEEE1588-2019: 8.2.3.6
        grandmaster_clock_quality = default_ds.clock_quality;             // IEEE1588-2019: 8.2.3.7
        grandmaster_priority1 = default_ds.priority1;                     // IEEE1588-2019: 8.2.3.8
        grandmaster_priority2 = default_ds.priority2;                     // IEEE1588-2019: 8.2.3.9
    }

    /**
     * @return A string representation of the parent data set.
     */
    [[nodiscard]] std::string to_string() const {
        return fmt::format(
            "Parent port identity: {}, grandmaster identity: {}, grandmaster priority1: {}, grandmaster priority2: {}",
            parent_port_identity.clock_identity.to_string(), grandmaster_identity.to_string(), grandmaster_priority1, grandmaster_priority2
        );
    }
};

}  // namespace rav::ptp
