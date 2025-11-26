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

#include "ravennakit/core/streams/output_stream.hpp"
#include "ravennakit/ptp/ptp_definitions.hpp"

namespace rav::ptp {

/**
 * PTP Clock Quality
 * IEEE1588-2019 section 7.6.2.5, Table 4
 */
struct ClockQuality {
    /// The clock class. Default is 248, for slave-only the value is 255.
    uint8_t clock_class {};
    ClockAccuracy clock_accuracy {ClockAccuracy::unknown};
    uint16_t offset_scaled_log_variance {};

    ClockQuality() = default;

    explicit ClockQuality(const bool slave_only) {
        clock_class = slave_only ? 255 : 248;
        clock_accuracy = ClockAccuracy::unknown;
        offset_scaled_log_variance = 0;
    }

    /**
     * Write the ptp_announce_message to a byte buffer.
     * @param buffer The buffer to write to.
     */
    void write_to(ByteBuffer& buffer) const {
        buffer.write_be<uint8_t>(clock_class);
        buffer.write_be<uint8_t>(static_cast<uint8_t>(clock_accuracy));
        buffer.write_be<uint16_t>(offset_scaled_log_variance);
    }

    /**
     * Create a PTP clock quality from a buffer_view.
     * @return A PTP clock quality if the data is valid, otherwise a PTP error.
     */
    [[nodiscard]] std::string to_string() const {
        return fmt::format(
            "clock_class={} clock_accuracy={} offset_scaled_log_variance={}", clock_class, rav::ptp::to_string(clock_accuracy),
            offset_scaled_log_variance
        );
    }
};

}  // namespace rav::ptp
