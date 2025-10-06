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

#include "ravennakit/core/math/sliding_stats.hpp"
#include "ravennakit/core/util/throttle.hpp"

#include <cstdint>

namespace rav::ptp {

/**
 * Holds different statistics of PTP measurements.
 */
struct Stats {
    constexpr static int64_t k_clock_step_threshold_seconds = 1;
    constexpr static double k_calibrated_threshold = 0.0018;

    SlidingStats offset {51};
    SlidingStats filtered_offset {51};
    uint32_t ignored_outliers = 0;
    Throttle<void> trace_adjustments_throttle {std::chrono::seconds(5)};
};

}  // namespace rav::ptp
