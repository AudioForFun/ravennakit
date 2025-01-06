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
#include "types/ptp_timestamp.hpp"

#include <cstdint>

namespace rav {

/**
 * A class that maintains a local PTP clock as close as possible to some grand master clock.
 * This particular implementation maintains a 'virtual' clock based on the local clock and a correction value.
 */
class ptp_local_ptp_clock {
  public:
    [[nodiscard]] ptp_timestamp now() const {
        auto now = ptp_local_clock::now();
        now.add_correction(offset_from_master_);
        return now;
    }

    [[nodiscard]] bool is_calibrated() const {
        return false;  // TODO: Implement calibration
    }

    void adjust(const ptp_time_interval offset_from_master) {
        const auto diff =
            (offset_from_master - offset_from_master_) / ptp_timestamp::k_correction_field_multiplier / 1'000;
        TRACY_PLOT("Adjusted PTP clock (us): ", diff);
        RAV_TRACE("Adjusted clock by {} us", diff);
        offset_from_master_ = offset_from_master;
    }

  private:
    ptp_time_interval offset_from_master_ {};
};

}  // namespace rav
