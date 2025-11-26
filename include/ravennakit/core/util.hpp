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

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace rav {

/**
 * Returns the number of elements in a c-style array.
 * @tparam Type The type of the elements.
 * @tparam N The total number of elements.
 * @return The number of elements in the array.
 */
template<typename Type, size_t N>
constexpr size_t num_elements_in_array(Type (&)[N]) noexcept {
    return N;
}

/**
 * Tests if two values are within a certain tolerance of each other.
 * @tparam T The type of the values to compare.
 * @param a The first value.
 * @param b The second value.
 * @param tolerance The tolerance.
 * @return True if the values are within the tolerance of each other, false otherwise.
 */
template<typename T>
bool is_within(T a, T b, T tolerance) {
    return std::fabs(a - b) <= tolerance;
}

/**
 * Tests if a value is between two other values, inclusive.
 * @tparam T The type of the values to compare.
 * @param a The value to test.
 * @param min The minimum value.
 * @param max The maximum value.
 * @return True if the value is between the minimum and maximum values, false otherwise.
 */
template<typename T>
bool is_between(T a, T min, T max) {
    return a >= min && a <= max;
}

}  // namespace rav
