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

    #include <CoreFoundation/CoreFoundation.h>

/**
 * RAII wrapper around CFTypeRef.
 * @tparam T The CFTypeRef type.
 */
template<class T>
class CfType {
  public:
    /**
     * Constructs a new cf_type from an existing T. The ref is assumed to be already retained, and this class will
     * assume ownership.
     * @param ref The T ref to manage.
     * @param retain True to retain the given ref, false otherwise (where it is assumed that the ref has been retained
     * before and needs to be placed under RAII).
     */
    explicit CfType(const T ref, const bool retain) : ref_(ref) {
        if (retain && ref_) {
            CFRetain(ref_);
        }
    }

    ~CfType() {
        reset();
    }

    /**
     * Copy constructor.
     * @param other The other T to copy from.
     */
    CfType(const CfType& other) {
        reset();  // Existing value
        ref_ = other.ref_;
        if (ref_ != nullptr) {
            CFRetain(ref_);
        }
    }

    /**
     * Copy assignment operator.
     * @param other The other cf_type to copy from.
     * @return Reference to this.
     */
    CfType& operator=(const CfType& other) {
        if (this != &other) {
            reset();  // Existing value
            ref_ = other.ref_;
            if (ref_ != nullptr) {
                CFRetain(ref_);
            }
        }
        return *this;
    }

    /**
     * Move constructor.
     * @param other The other cf_type to move from.
     */
    CfType(CfType&& other) noexcept {
        ref_ = other.ref_;
        other.ref_ = nullptr;
    }

    /**
     * Move assignment operator.
     * @param other The other cf_type to move from.
     * @return Reference to this.
     */
    CfType& operator=(CfType&& other) noexcept {
        if (this != &other) {
            reset();  // Existing value
            ref_ = other.ref_;
            other.ref_ = nullptr;
        }
        return *this;
    }

    /**
     * Returns true if the internal pointer is not nullptr.
     */
    explicit operator bool() const {
        return is_valid();
    }

    /**
     * @returns True if the internal pointer is valid.
     */
    [[nodiscard]] bool is_valid() const {
        return ref_ != nullptr;
    }

    /**
     * Release the T, and resets the internal pointer to nullptr.
     */
    void reset() {
        if (ref_ == nullptr) {
            return;
        }
        CFRelease(ref_);
        ref_ = nullptr;
    }

    /**
     * @return The raw CFTypeRef.
     */
    T get() const {
        return ref_;
    }

  private:
    T ref_;
};

#endif
