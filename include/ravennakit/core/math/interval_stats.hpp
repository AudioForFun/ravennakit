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

#include "ravennakit/ptp/detail/ptp_basic_filter.hpp"

#include <cstdint>

namespace rav {

/**
 * Keeps track of an ema with outliers filtered out and a max deviation.
 */
struct IntervalStats {
    /// Interval as measured over time
    double interval = 0.0;
    /// The max deviation compared to interval
    double max_deviation = 0.0;
    /// Whether this struct has been initialized
    bool initialized = false;
    /// The alpa of ema calculation. Lower is smoother.
    double alpha = 0.001;

    void update(const double interval_ms) {
        if (!initialized) {
            interval = interval_ms;
            initialized = true;
            return;
        }

        const auto ema = alpha * interval_ms + (1.0 - alpha) * interval;
        const auto step = ema - interval;

        if (step > current_step_size_) {
            interval += current_step_size_;  // Limit positive change
            current_step_size_ = std::min(current_step_size_ * 2.0, k_max_step_size);
        } else if (step < -current_step_size_) {
            interval -= current_step_size_;  // Limit negative change
            current_step_size_ = std::min(current_step_size_ * 2.0, k_max_step_size);
        } else {
            interval = ema;  // Change is within limit
            current_step_size_ = std::max(current_step_size_ / 2.0, k_min_step_size);
        }

        max_deviation = std::max(std::fabs(interval_ms - interval), max_deviation);
    }

  private:
    static constexpr auto k_min_step_size = 0.00001;
    static constexpr auto k_max_step_size = 100'000.0;
    double current_step_size_ = k_min_step_size;
};

}  // namespace rav
