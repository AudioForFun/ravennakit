/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#pragma once

#include "ravennakit/core/fraction.hpp"
#include "ravennakit/core/platform.hpp"

#include <cstdint>

#if RAV_APPLE
    #include <mach/mach_time.h>
#endif

namespace rav {

/**
 * Provides access to the current time with the highest possible resolution.
 */
class high_resolution_clock {
  public:
    /**
     * @returns the current time in nanoseconds since an arbitrary point in time.
     * On macOS the value does not progress while the system is asleep.
     */
    static uint64_t now() {
        static const high_resolution_clock clock;
#if RAV_APPLE
        const uint64_t raw = mach_absolute_time();
        return raw * clock.timebase_.numerator / clock.timebase_.denominator;
#else
    #warning "high_resolution_clock is not implemented for this platform."
        return 0;
#endif
    }

  private:
#if RAV_APPLE
    fraction<uint32_t> timebase_ {};
#endif

    high_resolution_clock() {
#if RAV_APPLE
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        timebase_.numerator = info.numer;
        timebase_.denominator = info.denom;
#endif
    }
};

}  // namespace rav
