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

#if RAV_WINDOWS

    #include <windows.h>

namespace rav {

inline LARGE_INTEGER query_performance_counter_frequency() {
    LARGE_INTEGER frequency;
    if (!QueryPerformanceFrequency(&frequency)) {
        return {};
    }
    return frequency;
}

inline LARGE_INTEGER query_performance_counter() {
    static const auto frequency = query_performance_counter_frequency();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter;
}

inline uint64_t query_performance_counter_ns() {
    static const auto frequency = query_performance_counter_frequency();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<uint64_t>((counter.QuadPart * 1'000'000'000) / frequency.QuadPart);
}

}  // namespace rav

#endif
