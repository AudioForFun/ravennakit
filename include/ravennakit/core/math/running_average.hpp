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

#include <type_traits>

namespace rav {

/**
 * A running average.
 */
class RunningAverage {
  public:
    /**
     * Adds a new value to the running average.
     * @param value The value to add.
     */
    void add(const double value) {
        count_++;
        average_ += (value - average_) / static_cast<double>(count_);
    }

    /**
     * Adds a new value to the running average.
     * @tparam U The type of the value to add.
     * @param value The value to add.
     */
    template<class U>
    void add(U value) {
        add(static_cast<double>(value));
    }

    /**
     * @return The current average.
     */
    [[nodiscard]] double average() const {
        return average_;
    }

    /**
     * @returns The number of values added to the running average.
     */
    [[nodiscard]] size_t count() const {
        return count_;
    }

    /**
     * Resets the running average.
     */
    void reset() {
        count_ = 0;
        average_ = 0;
    }

  private:
    double average_ {};
    size_t count_ {};
};

}  // namespace rav
