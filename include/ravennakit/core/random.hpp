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

#include <random>
#include <string>
#include <algorithm>
#include <chrono>

namespace rav {

class Random {
  public:
    /**
     * Generates alphanumeric random string.
     * @param length The length of the string to generate.
     * @return The pseudo randomly generated string
     */
    std::string generate_random_string(const size_t length) {
        const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::uniform_int_distribution dist(0, static_cast<int>(charset.size()) - 1);

        std::string result(length, 0);
        std::generate_n(result.begin(), length, [&]() {
            return charset[static_cast<size_t>(dist(generator_))];
        });
        return result;
    }

    /**
     * Generates a random integer between min and max.
     * @tparam T The type of the integer.
     * @param min The minimum value.
     * @param max The maximum value.
     * @return The pseudo randomly generated integer.
     */
    template<typename T = int>
    int get_random_int(T min, T max) {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(generator_);
    }

    /**
     * Generates a random interval between min_ms and max_ms with the granularity of one ms.
     * @tparam T The type of the uniform_int_distribution IntType.
     * @param min_ms The minimum value in milliseconds.
     * @param max_ms The maximum value in milliseconds.
     * @return The pseudo randomly generated interval.
     */
    template<typename T = int>
    std::chrono::milliseconds get_random_interval_ms(T min_ms, T max_ms) {
        return std::chrono::milliseconds(get_random_int<T>(min_ms, max_ms));
    }

  private:
    std::random_device rd_;
    std::mt19937 generator_ {rd_()};
};

}  // namespace rav
