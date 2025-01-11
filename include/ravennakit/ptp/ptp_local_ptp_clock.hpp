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

#include "detail/ptp_basic_filter.hpp"
#include "detail/ptp_measurement.hpp"
#include "ravennakit/core/tracy.hpp"
#include "ravennakit/core/util.hpp"
#include "ravennakit/core/chrono/high_resolution_clock.hpp"
#include "ravennakit/core/math/running_average.hpp"
#include "ravennakit/core/math/sliding_average.hpp"
#include "ravennakit/core/math/sliding_median.hpp"
#include "types/ptp_timestamp.hpp"

#include <cstdint>

namespace rav {

/**
 * A class that maintains a local PTP clock as close as possible to some grand master clock.
 * This particular implementation maintains a 'virtual' clock based on the local clock and a correction value.
 */
class ptp_local_ptp_clock {
  public:
    ptp_local_ptp_clock() = default;

    static ptp_timestamp system_clock_now() {
        return ptp_timestamp(high_resolution_clock::now());
    }

    [[nodiscard]] ptp_timestamp now() const {
        const auto system_now = system_clock_now();
        const auto elapsed = (system_now - last_sync_).total_seconds_double();
        auto result = last_sync_;
        result.add_seconds(elapsed * frequency_ratio_);
        result.add_seconds(shift_);
        return result;
    }

    [[nodiscard]] ptp_timestamp now(const ptp_timestamp local_timestamp) const {
        const auto elapsed = local_timestamp.total_seconds_double() - last_sync_.total_seconds_double();
        auto result = last_sync_;
        result.add_seconds(elapsed * frequency_ratio_);
        result.add_seconds(shift_);
        return result;
    }

    // TODO: Make a difference between 'locked' and 'calibrated'
    // Calibrated means that the clock has been adjusted enough times to be considered stable
    // Locked means that the clock has been adjusted enough times to be considered stable and is locked to the master
    [[nodiscard]] bool is_calibrated() const {
        return adjustments_since_last_step_ >= k_calibrated_threshold;
    }

    void adjust(const ptp_measurement<double>& measurement) {
        const auto system_now = system_clock_now();
        shift_ = now().total_seconds_double() - system_now.total_seconds_double();
        last_sync_ = system_now;

        // Filter out outliers
        if (is_calibrated() && offset_median_.is_outlier(measurement.offset_from_master, 0.0015)) {
            RAV_WARNING("Ignoring outlier in offset from master: {}", measurement.offset_from_master * 1000.0);
            TRACY_MESSAGE("Ignoring outlier in offset from master");
            return;
        }

        const auto offset = offset_filter_.update(measurement.offset_from_master);
        offset_median_.add(offset);
        TRACY_PLOT("Filtered offset (ms)", offset * 1000.0);
        TRACY_PLOT("Offset from master (ms median)", offset_median_.median() * 1000.0);
        TRACY_PLOT("Adjustments since last step", static_cast<int64_t>(adjustments_since_last_step_));

        if (is_calibrated()) {
            constexpr double base = 1.5;        // The higher the value, the faster the clock will adjust (>= 1.0)
            constexpr double max_ratio = 0.5;   // +/-
            constexpr double max_step = 0.001;  // Maximum step size
            const auto nominal_ratio = std::clamp(std::pow(base, -offset), 1.0 - max_ratio, 1 + max_ratio);

            if (std::fabs(nominal_ratio - frequency_ratio_) > max_step) {
                if (frequency_ratio_ < nominal_ratio) {
                    frequency_ratio_ += max_step;
                } else {
                    frequency_ratio_ -= max_step;
                }
            } else {
                frequency_ratio_ = nominal_ratio;
            }
        } else {
            adjustments_since_last_step_++;
            frequency_ratio_ = 1.0;
        }

        TRACY_PLOT("Frequency ratio", frequency_ratio_);
    }

    void step_clock(const double offset_from_master_seconds) {
        last_sync_ = system_clock_now();
        shift_ += -offset_from_master_seconds;
        offset_median_.reset();
        frequency_ratio_ = 1.0;
        adjustments_since_last_step_ = 0;
    }

  private:
    constexpr static size_t k_calibrated_threshold = 10;
    ptp_timestamp last_sync_ = system_clock_now();
    double shift_ {};
    double frequency_ratio_ = 1.0;
    sliding_median offset_median_ {101};
    size_t adjustments_since_last_step_ {};
    ptp_basic_filter offset_filter_ {0.5};
};

}  // namespace rav
