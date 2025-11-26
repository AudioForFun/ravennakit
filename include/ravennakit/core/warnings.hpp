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

#if defined(__clang__)
    #define RAV_BEGIN_IGNORE_WARNINGS _Pragma("clang diagnostic push")
_Pragma("clang diagnostic ignored \"-Wextra-semi\"") _Pragma("clang diagnostic ignored \"-Wundef\"") _Pragma(
    "clang diagnostic ignored \"-Wswitch-enum\""
)

#elif defined(__GNUC__) && (__GNUC__ >= 5)
    #define RAV_BEGIN_IGNORE_WARNINGS _Pragma("GCC diagnostic push")
#elif defined(_MSC_VER)
    #define RAV_BEGIN_IGNORE_WARNINGS _Pragma("warning (push, 0)") _Pragma("warning(disable:4668)")
#endif

#if defined(__clang__)
    #define RAV_END_IGNORE_WARNINGS _Pragma("clang diagnostic pop")
#elif defined(__GNUC__) && (__GNUC__ >= 5)
    #define RAV_END_IGNORE_WARNINGS _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
    #define RAV_END_IGNORE_WARNINGS _Pragma("warning (pop)")
#endif
