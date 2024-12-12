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

#include "cf_type.hpp"
#include "ravennakit/core/platform.hpp"

#if RAV_APPLE

    #include <CoreFoundation/CoreFoundation.h>

namespace rav {

/**
 * RAII wrapper for CFArrayRef.
 */
template<class E>
class cf_array: public cf_type<CFArrayRef> {
  public:
    /**
     * Constructs a new cf_array from an existing CFArrayRef. The array_ref is assumed to be already retained, and this
     * class will assume ownership.
     * @param array_ref The CFArrayRef to manage.
     * @param retain True to retain the given array_ref, false otherwise (where it is assumed that the array_ref has
     * been retained before and needs to be placed under RAII).
     */
    explicit cf_array(const CFArrayRef array_ref, const bool retain = true) : cf_type(array_ref, retain) {}

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
