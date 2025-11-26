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

#include "ravennakit/core/warnings.hpp"
#include "ravennakit/core/platform.hpp"

#if defined(TRACY_ENABLE) && TRACY_ENABLE
    #if RAV_APPLE
        #define TracyFunction __PRETTY_FUNCTION__
    #endif

RAV_BEGIN_IGNORE_WARNINGS
    #include <tracy/Tracy.hpp>
RAV_END_IGNORE_WARNINGS

    #define TRACY_ZONE_SCOPED ZoneScoped  // NOLINT(bugprone-reserved-identifier)
    #define TRACY_PLOT(name, value) TracyPlot(name, value)
    #define TRACY_MESSAGE(message) TracyMessageL(message)
    #define TRACY_MESSAGE_COLOR(message, color) TracyMessageLC(message, color)
    #define TRACY_SET_THREAD_NAME(name) tracy::SetThreadName(name)
#else
    #define TRACY_ZONE_SCOPED
    #define TRACY_PLOT(...)
    #define TRACY_MESSAGE(...)
    #define TRACY_MESSAGE_COLOR(...)
    #define TRACY_SET_THREAD_NAME(...)
#endif

namespace rav {

/**
 * Sometimes you want to know when something happened, but you don't want to instrument functions which get called into.
 * Then use this function to get a quick and dirty point in time.
 */
inline void tracy_point() {
    TRACY_ZONE_SCOPED;
}

}  // namespace rav
