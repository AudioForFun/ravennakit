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

#include <limits>
#include <type_traits>
#include <cstdint>
#include <optional>

namespace rav {

/**
 * Represents sequence numbers with rollover (wraparound) semantics. This class is designed to work with any unsigned
 * integer type that wraps around to 0 after reaching its maximum value. It also supports handling gaps, making it
 * suitable for scenarios involving packet loss.
 * Use this class for the sequence id in PTP, sequence number in RTP, timestamp in RTP, etc.
 */
template<class T>
class WrappingUint {
    static_assert(std::is_integral_v<T>, "WrappingUint only supports integral types");
    static_assert(std::is_unsigned_v<T>, "WrappingUint only supports unsigned types");

  public:
    /**
     * Default construct a sequence number with the value 0.
     */
    WrappingUint() = default;

    /**
     * Construct a sequence number with the given value.
     * @param value The initial value of the sequence number.
     */
    explicit WrappingUint(const T value) : value_(value) {}

    /**
     * Updates the value in the sequence. The number of steps taken from the previous value to given value
     * will be returned, taking into account wraparound. The current (internal) value will only progress forward, if the
     * value is older than the current value, it will return std::nullopt. The returned value can be used to detect
     * gaps (when value > 1).
     *
     * @param value The value to set.
     * @return The difference between the given value and the current value, taking into account wraparound. If the
     * value is older than the current value, std::nullopt will be returned.
     */
    std::optional<T> update(const T value) {
        if (is_older_than(value, value_)) {
            return std::nullopt;  // Value too old
        }

        const auto diff = value - value_;
        value_ = value;
        return static_cast<T>(diff);
    }

    /**
     * @returns The value of the sequence number.
     */
    [[nodiscard]] explicit operator T() const {
        return value_;
    }

    /**
     * @returns The value of the sequence number.
     */
    [[nodiscard]] T value() const {
        return value_;
    }

    /**
     * Assigns a new value to the sequence number.
     * @param value The new value of the sequence number.
     * @return This instance.
     */
    WrappingUint& operator=(const T value) {
        value_ = value;
        return *this;
    }

    /**
     * Increments the sequence number by 1 using modulo arithmetic.
     * @param value The value to increment by.
     * @return This instance.
     */
    WrappingUint& operator+=(const T value) {
        value_ += value;
        return *this;
    }

    /**
     * Decrements the sequence number by 1 using modulo arithmetic.
     * @param value The value to decrement by.
     * @return This instance.
     */
    WrappingUint& operator-=(const T value) {
        value_ -= value;
        return *this;
    }

    /**
     * Increments the sequence number by the given value.
     * @param value The value to increment by.
     * @return A new sequence number instance.
     */
    [[nodiscard]] WrappingUint operator+(const T value) const {
        return WrappingUint(value_ + value);
    }

    /**
     * Decrements the sequence number by the given value.
     * @param value The value to decrement by.
     * @return A new sequence number instance.
     */
    [[nodiscard]] WrappingUint operator-(const T value) const {
        return WrappingUint(value_ - value);
    }

    /**
     * Compares the sequence number with the given value.
     * @param other The value to compare with.
     * @return True if the sequence number is equal to the given value, false otherwise.
     */
    [[nodiscard]] bool operator==(T other) {
        return value_ == other;
    }

    /**
     * Compares the sequence number with the given value.
     * @param other The value to compare with.
     * @return True if the sequence number is not equal to the given value, false otherwise.
     */
    [[nodiscard]] bool operator!=(T other) {
        return value_ != other;
    }

    /**
     * Compares the sequence number with the given value.
     * @param lhs Left hand side.
     * @param rhs Right hand side.
     * @return True if the sequence number is less than the given value, false otherwise.
     */
    friend bool operator==(const WrappingUint& lhs, const WrappingUint& rhs) {
        return lhs.value_ == rhs.value_;
    }

    /**
     * Compares the sequence number with the given value.
     * @param lhs Left hand side.
     * @param rhs Right hand side.
     * @return True if the sequence number is not equal to the given value, false otherwise.
     */
    friend bool operator!=(const WrappingUint& lhs, const WrappingUint& rhs) {
        return lhs.value_ != rhs.value_;
    }

    /**
     * Compares the sequence number with the given value.
     * @param lhs Left hand side.
     * @param rhs Right hand side.
     * @return True if the sequence number is less than the given value, false otherwise.
     */
    friend bool operator<(const WrappingUint& lhs, const WrappingUint& rhs) {
        return is_older_than(lhs.value_, rhs.value_);
    }

    /**
     * Compares the sequence number with the given value.
     * @param lhs Left hand side.
     * @param rhs Right hand side.
     * @return True if the sequence number is less than or equal to the given value, false otherwise.
     */
    friend bool operator<=(const WrappingUint& lhs, const WrappingUint& rhs) {
        return lhs < rhs || lhs == rhs;
    }

    /**
     * Compares the sequence number with the given value.
     * @param lhs Left hand side.
     * @param rhs Right hand side.
     * @return True if the sequence number is greater than the given value, false otherwise.
     */
    friend bool operator>(const WrappingUint& lhs, const WrappingUint& rhs) {
        return !(lhs <= rhs);
    }

    /**
     * Compares the sequence number with the given value.
     * @param lhs Left hand side.
     * @param rhs Right hand side.
     * @return True if the sequence number is greater than or equal to the given value, false otherwise.
     */
    friend bool operator>=(const WrappingUint& lhs, const WrappingUint& rhs) {
        return lhs > rhs || lhs == rhs;
    }

    /**
     * Calculates the difference between two sequence numbers, taking into account wraparound.
     * The value will be positive if the other sequence number is newer than this one, and negative if this one is
     * newer.
     * @param other The other sequence number.
     * @return The difference between the two sequence numbers.
     */
    [[nodiscard]] std::make_signed_t<T> diff(const WrappingUint& other) const {
        return diff(other.value_);
    }

    /**
     * Calculates the difference between two sequence numbers, taking into account wraparound.
     * The value will be positive if the other sequence number is newer than this one, and negative if this one is
     * newer.
     * @param other The other sequence number.
     * @return The difference between the two sequence numbers.
     */
    [[nodiscard]] std::make_signed_t<T> diff(T other) const {
        // 1 -> 0
        if (is_older_than(other, value_)) {
            return static_cast<std::make_signed_t<T>>(value_ - other) * -1;
        }
        return static_cast<std::make_signed_t<T>>(other - value_);
    }

  private:
    T value_ {};

    /**
     * Checks if a is older than b, taking into account wraparound.
     * @param a The first value.
     * @param b The second value.
     * @return True if a is older than b, false otherwise.
     */
    static bool is_older_than(T a, T b) {
        return !(a == b) && static_cast<T>(b - a) < std::numeric_limits<T>::max() / 2 + 1;
    }
};

/// 8-bit wrapping unsigned integer.
using WrappingUint8 = WrappingUint<uint8_t>;

/// 16-bit wrapping unsigned integer.
using WrappingUint16 = WrappingUint<uint16_t>;

/// 32-bit wrapping unsigned integer.
using WrappingUint32 = WrappingUint<uint32_t>;

/// 64-bit wrapping unsigned integer.
using WrappingUint64 = WrappingUint<uint64_t>;

}  // namespace rav
