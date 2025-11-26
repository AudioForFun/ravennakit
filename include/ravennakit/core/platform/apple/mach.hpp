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

#include "ravennakit/core/platform.hpp"

#if RAV_APPLE

    #include <mach/mach_time.h>

namespace rav {

inline mach_timebase_info_data_t get_mach_timebase_info() {
    mach_timebase_info_data_t info {};
    if (mach_timebase_info(&info) != KERN_SUCCESS) {
        return {1, 1};  // On failure return ratio 1
    }
    return info;
}

inline uint64_t mach_absolute_time_to_nanoseconds(const uint64_t absolute_time) {
    static const auto info = get_mach_timebase_info();
    return absolute_time * info.numer / info.denom;
}

inline uint64_t mach_nanoseconds_to_absolute_time(const uint64_t nanoseconds) {
    static const auto info = get_mach_timebase_info();
    return nanoseconds * info.denom / info.numer;
}

inline uint64_t mach_absolute_time_ns() {
    return mach_absolute_time_to_nanoseconds(mach_absolute_time());
}

inline bool mach_wait_until_ns(const uint64_t nanoseconds) {
    return mach_wait_until(mach_nanoseconds_to_absolute_time(nanoseconds)) == KERN_SUCCESS;
}

}  // namespace rav

#endif
