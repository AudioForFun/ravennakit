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

#include <cmath>
#include <algorithm>

namespace rav::ptp {

/**
 * Simple averaging filter.
 */
class BasicFilter {
  public:
    /**
     * Constructor.
     * @param gain The gain of the filter.
     */
    explicit BasicFilter(const double gain) : gain_(gain) {
        reset();
    }

    /**
     * Updates the filter with a new value.
     * @param value The new value.
     * @return The filtered value.
     */
    double update(double value) {
        const auto value_abs = std::fabs(value);
        if (value_abs > confidence_range_) {
            confidence_range_ *= 2.0;
            value = std::clamp(value, -confidence_range_, confidence_range_);
        } else {
            confidence_range_ -= (confidence_range_ - value_abs) * gain_;
        }
        return value * gain_;
    }

    /**
     * Resets the filter.
     */
    void reset() {
        confidence_range_ = 1.0;
    }

  private:
    double confidence_range_ {1.0};  // In seconds
    double gain_ {1.0};
};

}  // namespace rav::ptp
