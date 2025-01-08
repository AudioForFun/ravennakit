/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <cmath>

namespace rav {

/**
 * Represents TimeInterval as specified in PTP IEEE1588-2019. TimeInterval is the time interval expressed in
 * nanoseconds, multiplied by 2^16. This structure stores the data in bigger integers to allow for math operations
 * without overflowing or precision loss.
 */
class ptp_time_interval {
  public:
    ptp_time_interval() = default;

    /**
     * Constructs a ptp_time_interval from nanoseconds and fraction.
     * Ensures that the fraction part is normalized to always be positive.
     * @param nanos The number of nanoseconds.
     * @param fraction The nanosecond fraction part of the time interval.
     */
    ptp_time_interval(const int64_t nanos, const int32_t fraction) {
        nanos_ = nanos;
        fraction_ = fraction;
        normalize();
    }

    /**
     * @return The number of nanoseconds.
     */
    [[nodiscard]] int64_t nanos() const {
        return nanos_;
    }

    /**
     * @return The number of nanoseconds, rounded to the nearest nanosecond.
     */
    [[nodiscard]] int64_t nanos_rounded() const {
        if (fraction_ >= k_fractional_scale / 2) {
            return nanos_ + 1;
        }
        return nanos_;
    }

    /**
     * @return The nanosecond fraction part of the time interval.
     */
    [[nodiscard]] int32_t fraction() const {
        return fraction_;
    }

    /**
     * Create a ptp_time_interval from a wire format value where the nanoseconds are in the high 48 bits and the
     * fraction is in the low 16 bits.
     * @param value The wire format value.
     * @return The ptp_time_interval.
     */
    static ptp_time_interval from_wire_format(const int64_t value) {
        ptp_time_interval interval;
        interval.nanos_ = value / k_fractional_scale;
        interval.fraction_ = static_cast<int32_t>(value % k_fractional_scale);
        interval.normalize();
        return interval;
    }

    /**
     * Convert the ptp_time_interval to wire format where the nanoseconds are in the high 48 bits and the fraction is in
     * the low 16 bits.
     * @return The wire format value.
     */
    [[nodiscard]] int64_t to_wire_format() const {
        return nanos_ * k_fractional_scale + fraction_;
    }

    /**
     * Adds two ptp_time_intervals together.
     * @param other The time interval to add.
     * @return The sum of the two time intervals.
     */
    ptp_time_interval operator+(const ptp_time_interval& other) const {
        return {nanos_ + other.nanos_, fraction_ + other.fraction_};
    }

    /**
     * Subtracts two ptp_time_intervals.
     * @param other The time interval to subtract.
     * @return The difference of the two time intervals.
     */
    ptp_time_interval operator-(const ptp_time_interval& other) const {
        return {nanos_ - other.nanos_, fraction_ - other.fraction_};
    }

    /**
     * Adds another ptp_time_interval to this one.
     * @param other The time interval to add.
     * @return A reference to this object.
     */
    ptp_time_interval& operator+=(const ptp_time_interval& other) {
        nanos_ += other.nanos_;
        fraction_ += other.fraction_;
        normalize();
        return *this;
    }

    /**
     * Subtracts another ptp_time_interval from this one.
     * @param other The time interval to subtract.
     * @return A reference to this object.
     */
    ptp_time_interval& operator-=(const ptp_time_interval& other) {
        nanos_ -= other.nanos_;
        fraction_ -= other.fraction_;
        normalize();
        return *this;
    }

    /**
     * Equality operator.
     * @param other The time interval to compare to.
     * @return True if the time intervals are equal, false otherwise.
     */
    bool operator==(const ptp_time_interval& other) const {
        return nanos_ == other.nanos_ && fraction_ == other.fraction_;
    }

    /**
     * Inequality operator.
     * @param other The time interval to compare to.
     * @return True if the time intervals are not equal, false otherwise.
     */
    bool operator!=(const ptp_time_interval& other) const {
        return !(*this == other);
    }

    constexpr static int64_t k_fractional_scale = 0x10000;  // pow(2, 16) = 65536

  private:
    int64_t nanos_ {};     // 48 bits on the wire
    int32_t fraction_ {};  // Range: [0, k_time_interval_multiplier)

    /**
     * Normalizes the time interval such that the fraction part is always non-negative.
     */
    void normalize() {
        if (fraction_ < 0) {
            const auto borrow = (-fraction_ + k_fractional_scale - 1) / k_fractional_scale;
            nanos_ -= borrow;
            fraction_ += static_cast<int32_t>(borrow * k_fractional_scale);
        } else if (fraction_ >= k_fractional_scale) {
            const int64_t carry = fraction_ / k_fractional_scale;
            nanos_ += carry;
            fraction_ -= static_cast<int32_t>(carry * k_fractional_scale);
        }
    }
};

}  // namespace rav
