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

#include "ravennakit/core/platform.hpp"

#if RAV_APPLE

    #include <CoreFoundation/CoreFoundation.h>

/**
 * RAII wrapper around CFTypeRef.
 * @tparam T The CFTypeRef type.
 */
template<class T>
class cf_type {
  public:
    /**
     * Constructs a new cf_type from an existing T. The ref is assumed to be already retained, and this class will
     * assume ownership.
     * @param ref The T ref to manage.
     * @param retain True to retain the given ref, false otherwise (where it is assumed that the ref has been retained
     * before and needs to be placed under RAII).
     */
    explicit cf_type(const T ref, const bool retain) : ref_(ref) {
        if (retain && ref_) {
            CFRetain(ref_);
        }
    }

    ~cf_type() {
        reset();
    }

    /**
     * Copy constructor.
     * @param other The other T to copy from.
     */
    cf_type(const cf_type& other) {
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
    cf_type& operator=(const cf_type& other) {
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
    cf_type(cf_type&& other) noexcept {
        ref_ = other.ref_;
        other.ref_ = nullptr;
    }

    /**
     * Move assignment operator.
     * @param other The other cf_type to move from.
     * @return Reference to this.
     */
    cf_type& operator=(cf_type&& other) noexcept {
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
