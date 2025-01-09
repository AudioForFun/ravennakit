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

#include "ptp_local_clock.hpp"
#include "ravennakit/core/tracy.hpp"
#include "ravennakit/core/math/running_average.hpp"
#include "ravennakit/core/math/sliding_window_average.hpp"
#include "types/ptp_timestamp.hpp"

#include <cstdint>

namespace rav {

/**
 * A class that maintains a local PTP clock as close as possible to some grand master clock.
 * This particular implementation maintains a 'virtual' clock based on the local clock and a correction value.
 */
class ptp_local_ptp_clock {
  public:
    ptp_local_ptp_clock() : last_sync_(ptp_local_clock::now()) {}

    static ptp_timestamp now_local() {
        return ptp_local_clock::now();
    }

    [[nodiscard]] ptp_timestamp now() const {
        auto now = ptp_local_clock::now();
        // let elapsed = roclock_time - self.last_sync;
        auto elapsed = now - last_sync_;
        // let corr = elapsed * self.freq_scale_ppm_diff / 1_000_000;
        auto correction = 0; // elapsed * frequency_correction_ppm_ / 1'000'000;

        now.add(shift_);

        return now;
    }

    [[nodiscard]] bool is_calibrated() const {
        return false;  // TODO: Implement calibration
    }

    void adjust(const double frequency_ppm) {
        const auto local_now = ptp_local_clock::now();
        const auto ptp_now = now();
        shift_ = ptp_now - local_now;
        last_sync_ = local_now;
        frequency_correction_ppm_ = frequency_ppm;
        freq_average_.add(frequency_ppm);
        freq_window_average_.add(frequency_ppm);
        TRACY_PLOT("Frequency (avg)", freq_average_.average());
        TRACY_PLOT("Frequency (sliding avg)", freq_window_average_.average());
    }

    void step_clock(const ptp_time_interval offset_from_master) {
        last_sync_ = ptp_local_clock::now();
        const auto multiplier = 1'000'000.0 + frequency_correction_ppm_;
        const auto reciprocal = 1'000'000.0 / multiplier;
        shift_ += offset_from_master * -1;// * reciprocal;
        freq_average_.reset();
        freq_window_average_.reset();
        TRACY_MESSAGE("Clock stepped");
    }

  private:
    ptp_timestamp last_sync_ {};  // Timestamp from ptp_local_clock when the clock was last synchronized
    ptp_time_interval shift_ {};
    double frequency_correction_ppm_ {};
    running_average freq_average_;
    sliding_window_average freq_window_average_{2024};
};

}  // namespace rav
