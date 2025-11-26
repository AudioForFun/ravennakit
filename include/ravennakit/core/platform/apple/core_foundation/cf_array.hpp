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

#include "cf_type.hpp"
#include "ravennakit/core/platform.hpp"

#if RAV_APPLE

    #include <CoreFoundation/CoreFoundation.h>

namespace rav {

/**
 * RAII wrapper for CFArrayRef.
 */
template<class E>
class CfArray: public CfType<CFArrayRef> {
  public:
    /**
     * Constructs a new cf_array from an existing CFArrayRef. The array_ref is assumed to be already retained, and this
     * class will assume ownership.
     * @param array_ref The CFArrayRef to manage.
     * @param retain True to retain the given array_ref, false otherwise (where it is assumed that the array_ref has
     * been retained before and needs to be placed under RAII).
     */
    explicit CfArray(const CFArrayRef array_ref, const bool retain = true) : CfType(array_ref, retain) {}

    /**
     * @return The amount of elements in the array.
     */
    [[nodiscard]] CFIndex count() const {
        if (!is_valid()) {
            return 0;
        }
        return CFArrayGetCount(get());
    }

    /**
     * Access an element in the array.
     * @param index The index of the element to access.
     * @return The element at the given index. Retain to keep the element alive.
     */
    E operator[](const CFIndex index) const {
        return static_cast<E>(CFArrayGetValueAtIndex(get(), index));
    }
};

}  // namespace rav

#endif
