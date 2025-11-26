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

#include "ravennakit/core/platform/apple/mach.hpp"
#include "ravennakit/core/platform/posix/clock.hpp"
#include "ravennakit/core/platform/windows/query_performance_counter.hpp"

namespace rav::clock {

inline uint64_t now_monotonic_high_resolution_ns() {
#if RAV_APPLE
    return mach_absolute_time_ns();
#elif RAV_WINDOWS
    return query_performance_counter_ns();
#elif RAV_POSIX
    return clock_get_time_ns();
#endif
}

}  // namespace rav::clock
