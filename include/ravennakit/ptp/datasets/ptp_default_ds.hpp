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

#include "ravennakit/ptp/types/ptp_clock_identity.hpp"
#include "ravennakit/ptp/types/ptp_clock_quality.hpp"
#include "ravennakit/ptp/types/ptp_sdo_id.hpp"

namespace rav::ptp {

/**
 * Represents the default data set as described in IEEE1588-2019: 8.2.1.
 */
struct DefaultDs {
    // Static members
    ClockIdentity clock_identity;
    uint16_t number_ports {};
    // Dynamic members
    ClockQuality clock_quality;
    // Configurable members
    uint8_t priority1 {128};  // Default for default profile
    uint8_t priority2 {128};  // Default for default profile

    /// Domain number
    /// Default profile: 0
    uint8_t domain_number {0};

    /// Slave only
    /// Default profile: false (if configurable)
    bool slave_only {false};  // Default for default profile

    SdoId sdo_id;  // 12 bit on the wire (0-4095), default for default profile

    explicit DefaultDs(const bool slave_only_) {
        slave_only = slave_only_;
        clock_quality = ClockQuality(slave_only);
    }
};

}  // namespace rav::ptp
