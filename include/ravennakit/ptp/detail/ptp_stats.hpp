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

#include "ravennakit/core/math/sliding_stats.hpp"

#include <cstdint>

namespace rav::ptp {

/**
 * Holds different statistics of PTP measurements.
 */
struct Stats {
    constexpr static int64_t k_clock_step_threshold_seconds = 1;
    constexpr static double k_calibrated_threshold = 0.0018;

    SlidingStats offset_from_master {51};
    SlidingStats filtered_offset {51};
    uint32_t ignored_outliers = 0;
};

}  // namespace rav::ptp
