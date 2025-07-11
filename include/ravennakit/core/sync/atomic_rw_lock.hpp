/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#pragma once
#include "ravennakit/core/assert.hpp"

#include <atomic>
#include <cstdint>
#include <thread>

namespace rav {

/**
 * A reader writer lock around atomics,
 */
class AtomicRwLock {
  public:
    static constexpr size_t k_loop_upper_bound = 300'000;
    const uint32_t k_yield_threshold = 10;  // The first number of attempts to do without yielding

    /**
     * Attempts to acquire an exclusive lock, spinning util it succeeds, or until the loop upper bounds is reached.
     * @return True if the lock was acquired, or false if the loop upper bound was reached.
     */
    bool lock_exclusive() {
        for (size_t i = 0; i < k_loop_upper_bound; ++i) {
            uint32_t prev_readers = readers.load(std::memory_order_acquire);
            if (prev_readers <= 1) {
                if (readers.compare_exchange_weak(prev_readers, std::numeric_limits<uint32_t>::max())) {
                    return true;
                }
            }

            if (prev_readers % 2 == 0) {
                // Indicate that there is a writer waiting by making reader count uneven
                readers.compare_exchange_weak(prev_readers, prev_readers + 1);
            }

            if (i >= k_yield_threshold) {
                std::this_thread::yield();
            }
        }
        RAV_ERROR("Loop upper bound reached");
        return false;
    }

    /**
     * Attempts to acquire an exclusive lock.
     * @return True if the lock was acquired, or false if not.
     */
    bool try_lock_exclusive() {
        uint32_t prev_readers = readers.load(std::memory_order_acquire);
        if (prev_readers <= 1) {
            if (readers.compare_exchange_strong(prev_readers, std::numeric_limits<uint32_t>::max())) {
                return true;
            }
        }
        return false;
    }

    /**
     * Releases the exclusive lock. Only call this when a successful call to lock_exclusive or try_lock_exclusive was
     * made before.
     */
    void unlock_exclusive() {
        const uint32_t prev_readers = readers.load(std::memory_order_acquire);
        if (prev_readers != std::numeric_limits<uint32_t>::max()) {
            RAV_ASSERT_FALSE("Not exclusively locked");
            return;
        }
        readers.store(0, std::memory_order_release);
    }

    /**
     * Attempts to acquire a shared lock, spinning util it succeeds, or until the loop upper bounds is reached.
     * @return True if the lock was acquired, or false if the loop upper bound was reached.
     */
    bool lock_shared() {
        for (size_t i = 0; i < k_loop_upper_bound; ++i) {
            uint32_t prev_readers = readers.load(std::memory_order_acquire);

            if (prev_readers % 2 == 0 && prev_readers < std::numeric_limits<uint32_t>::max()) {
                if (prev_readers >= std::numeric_limits<uint32_t>::max() - 2) {
                    RAV_ERROR("Too many readers");
                    return false;
                }
                if (readers.compare_exchange_weak(prev_readers, prev_readers + 2)) {
                    return true;
                }
            }

            if (i >= k_yield_threshold) {
                std::this_thread::yield();
            }
        }
        RAV_ERROR("Loop upper bound reached");
        return false;
    }

    /**
     * Attempts to acquire a shared lock.
     * @return True if the lock was acquired, or false if not.
     */
    bool try_lock_shared() {
        uint32_t prev_readers = readers.load(std::memory_order_acquire);

        if (prev_readers % 2 == 0 && prev_readers < std::numeric_limits<uint32_t>::max()) {
            if (prev_readers >= std::numeric_limits<uint32_t>::max() - 2) {
                RAV_ERROR("Too many readers");
                return false;
            }
            if (readers.compare_exchange_weak(prev_readers, prev_readers + 2)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Releases the shared lock. Only call this when a successful call to lock_shared or try_lock_shared was
     * made before.
     */
    void unlock_shared() {
        const uint32_t prev_readers = readers.load(std::memory_order_acquire);
        if (prev_readers == std::numeric_limits<uint32_t>::max() || prev_readers == 0) {
            RAV_ASSERT_FALSE("Not shared locked");
            return;
        }
        readers.fetch_sub(2, std::memory_order_release);
    }

  private:
    std::atomic<uint32_t> readers {0};
};

}  // namespace rav
