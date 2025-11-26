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

#include "ravennakit/core/clock.hpp"
#include "ravennakit/core/util/tracy.hpp"
#include "types/ptp_timestamp.hpp"

namespace rav::ptp {

/**
 * Maintains a local clock corrected to the timebase of another time source, most likely a PTP master clock.
 */
class LocalClock {
  public:
    /**
     * @return The best estimate of 'now' in the timescale of the grand master clock.
     */
    [[nodiscard]] Timestamp now() const {
        return get_adjusted_time(system_monotonic_now());
    }

    /**
     * Returns the adjusted time of the clock, which is the time in the timescale of the grand master clock.
     * @param system_time The system time to adjust.
     * @return The adjusted time in the timescale of the grand master clock.
     */
    [[nodiscard]] Timestamp get_adjusted_time(const Timestamp system_time) const {
        TRACY_ZONE_SCOPED;
        const auto elapsed = system_time.to_seconds_double() - last_sync_.to_seconds_double();
        auto result = last_sync_;
        result.add_seconds(elapsed * frequency_ratio_);
        result.add_seconds(shift_);
        return result;
    }

    /**
     * Returns the adjusted time of the clock, which is the time in the timescale of the grand master clock.
     * @param host_time_nanos The host time in nanoseconds.
     * @return The adjusted time in the timescale of the grand master clock.
     */
    [[nodiscard]] Timestamp get_adjusted_time(const uint64_t host_time_nanos) const {
        TRACY_ZONE_SCOPED;
        return get_adjusted_time(Timestamp(host_time_nanos));
    }

    /**
     * Adjusts the correction of this clock by adding the given shift and frequency ratio.
     * @param offset_from_master The shift to apply to the clock.
     */
    void adjust(const double offset_from_master) {
        TRACY_ZONE_SCOPED;
        last_sync_ = system_monotonic_now();
        shift_ += -offset_from_master;

        constexpr double max_ratio = 0.5;  // +/-
        const auto nominal_ratio = 0.001 * std::pow(-offset_from_master, 3) + 1.0;
        frequency_ratio_ = std::clamp(nominal_ratio, 1.0 - max_ratio, 1 + max_ratio);
        adjustments_since_last_step_++;
    }

    /**
     * Steps the clock to the given offset from the master clock. This is used when the clock is out of sync and needs
     * to be reset.
     * @param offset_from_master The offset from the master clock in seconds.
     */
    void step(const double offset_from_master) {
        TRACY_ZONE_SCOPED;
        last_sync_ = system_monotonic_now();
        shift_ += -offset_from_master;
        frequency_ratio_ = 1.0;
        adjustments_since_last_step_ = 0;
        calibrated_ = false;
    }

    /**
     * @return The current frequency ratio of the clock.
     */
    [[nodiscard]] double get_frequency_ratio() const {
        return frequency_ratio_;
    }

    /**
     *
     * @return The current shift of the clock.
     */
    [[nodiscard]] double get_shift() const {
        return shift_;
    }

    /**
     * @return True if the clock is valid, false otherwise. It does this by checking if the last sync time is valid.
     */
    [[nodiscard]] bool is_valid() const {
        return last_sync_.valid();
    }

    /**
     * @return True when the clock is locked, false otherwise. A clock is considered locked when it has received enough
     * adjustments. When a clock steps, the adjustments are reset.
     */
    [[nodiscard]] bool is_locked() const {
        TRACY_ZONE_SCOPED;
        return adjustments_since_last_step_ >= k_lock_threshold;
    }

    /**
     * Sets the calibrated state of the clock. A clock is considered calibrated when it has received enough adjustments
     * and is within the calibrated threshold.
     */
    void set_calibrated(const bool calibrated) {
        calibrated_ = calibrated;
    }

    /**
     * @return True if the clock is calibrated, false otherwise. A clock is considered calibrated when it has received
     * enough adjustments and is within the calibrated threshold.
     */
    [[nodiscard]] bool is_calibrated() const {
        return is_locked() && calibrated_;
    }

  private:
    constexpr static size_t k_lock_threshold = 10;

    Timestamp last_sync_ {};
    double shift_ {};
    double frequency_ratio_ = 1.0;
    size_t adjustments_since_last_step_ {};
    bool calibrated_ = false;

    static Timestamp system_monotonic_now() {
        return Timestamp(clock::now_monotonic_high_resolution_ns());
    }
};

// LocalClock should be trivially copyable to pass it through lock-free containers.
static_assert(std::is_trivially_copyable_v<LocalClock>);

}  // namespace rav::ptp
