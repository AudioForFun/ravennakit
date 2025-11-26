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

#include <chrono>

namespace rav {

/**
 * A class that throttles a value to a given interval.
 * @tparam T The type of the value to throttle.
 */
template<class T>
class Throttle {
  public:
    Throttle() = default;

    /**
     * Constructs the throttle with the given interval.
     * @param interval The interval to throttle the value to.
     */
    explicit Throttle(const std::chrono::milliseconds interval) : interval_(interval) {}

    /**
     * @param interval The interval to throttle the value to.
     */
    void set_interval(const std::chrono::milliseconds interval) {
        interval_ = interval;
    }

    /**
     * Updates the value and if the interval has passed since the last update returns the new value. If the value is
     * equal to the current value, nothing will happen and the function will return an empty optional.
     * @param value The new value.
     * @return The value if changed and the interval was passed, otherwise an empty optional.
     */
    std::optional<T> update(T value) {
        value_ = value;
        return get_throttled();
    }

    /**
     * @return The value, which might be empty if no value was set before.
     */
    std::optional<T> get() {
        return value_;
    }

    /**
     * @return The value if the interval has passed since the last update, otherwise an empty optional. The last set
     * value will be returned, even if the value wasn't changed since the last call to update.
     */
    std::optional<T> get_throttled() {
        if (!value_.has_value()) {
            return {};
        }
        if (const auto now = std::chrono::steady_clock::now(); now > last_update_ + interval_) {
            last_update_ = now;
            return value_;
        }
        return {};
    }

    /**
     * Clears the stored value.
     */
    void clear() {
        value_.reset();
    }

  private:
    std::optional<T> value_ {};
    std::chrono::steady_clock::time_point last_update_ {};
    std::chrono::milliseconds interval_ {100};
};

/**
 * Specialization for void, which doesn't store a value.
 */
template<>
class Throttle<void> {
  public:
    Throttle() = default;

    /**
     * Constructs the throttle with the given interval.
     * @param interval The interval to throttle the value to.
     */
    explicit Throttle(const std::chrono::milliseconds interval) : interval_(interval) {}

    /**
     * @param interval The interval to throttle the value to.
     */
    void set_interval(const std::chrono::milliseconds interval) {
        interval_ = interval;
    }

    /**
     * Updates the value and if the interval has passed since the last update returns the new value. If the value is
     * equal to the current value, nothing will happen and the function will return an empty optional.
     * @return The value if changed and the interval was passed, otherwise an empty optional.
     */
    bool update() {
        const auto now = std::chrono::steady_clock::now();
        if (now > last_update_ + interval_) {
            last_update_ = now;
            return true;
        }
        return false;
    }

  private:
    std::chrono::steady_clock::time_point last_update_ {};
    std::chrono::milliseconds interval_ {100};
};

}  // namespace rav
