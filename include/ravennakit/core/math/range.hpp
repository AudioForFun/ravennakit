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

#include "../assert.hpp"

namespace rav {

/**
 * Represents a range of values. Both start and end are inclusive. Range must be valid (start <= end).
 * @tparam T The value type.
 */
template<class T>
class Range {
  public:
    Range() = default;

    constexpr Range(T start, T end) : start_(start), end_(end) {
        RAV_ASSERT(start_ <= end_, "Invalid range");
    }

    /**
     * @return The start of the range.
     */
    [[nodiscard]] T start() const {
        return start_;
    }

    /**
     * @return The end of the range.
     */
    [[nodiscard]] T end() const {
        return end_;
    }

    /**
     * Check if the value is within the range. Both start and end are inclusive.
     * @param value The value to check.
     * @return True if the value is within the range, false otherwise.
     */
    [[nodiscard]] bool contains(const T& value) const {
        return value >= start_ && value <= end_;
    }

  private:
    T start_ {};
    T end_ {};
};

}  // namespace rav
