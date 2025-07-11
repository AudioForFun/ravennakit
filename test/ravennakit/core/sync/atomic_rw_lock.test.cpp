/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/core/sync/atomic_rw_lock.hpp"
#include "ravennakit/core/util/exclusive_access_guard.hpp"

#include <catch2/catch_all.hpp>

#include <future>
#include <thread>
#include <boost/mpl/integral_c_tag.hpp>

static_assert(!std::is_copy_constructible_v<rav::AtomicRwLock>);
static_assert(!std::is_move_constructible_v<rav::AtomicRwLock>);
static_assert(!std::is_copy_assignable_v<rav::AtomicRwLock>);
static_assert(!std::is_move_assignable_v<rav::AtomicRwLock>);

TEST_CASE("rav::AtomicRwLock") {
    SECTION("Test basic operation") {
        rav::AtomicRwLock lock;

        REQUIRE(lock.lock_exclusive());
        REQUIRE_FALSE(lock.try_lock_shared());
        REQUIRE_FALSE(lock.try_lock_exclusive());
        lock.unlock_exclusive();

        REQUIRE(lock.lock_shared());
        REQUIRE(lock.lock_shared());
        REQUIRE(lock.try_lock_shared());

        REQUIRE_FALSE(lock.try_lock_exclusive());

        lock.unlock_shared();
        lock.unlock_shared();
        lock.unlock_shared();

        REQUIRE(lock.lock_exclusive());
        lock.unlock_exclusive();
    }

    SECTION("Test writer-starvation avoidance") {
        rav::AtomicRwLock lock;

    }

    SECTION("Single writer, multiple readers") {
        rav::AtomicRwLock lock;

        std::atomic_bool error {};
        std::atomic_bool done {false};

        static constexpr size_t k_max_value = 200;

        std::atomic<int8_t> counter {};

        std::vector<std::thread> readers;
        readers.reserve(20);

        // Try locks
        for (int i = 0; i < 10; ++i) {
            readers.emplace_back([&lock, &counter, &error, &done] {
                while (!done) {
                    if (lock.try_lock_shared()) {
                        if (counter.fetch_add(2, std::memory_order_relaxed) % 2 != 0) {
                            // Odd which means a writer is active
                            error = true;
                            return;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(15));
                        if (counter.fetch_sub(2, std::memory_order_relaxed) % 2 != 0) {
                            // Odd which means a writer is active
                            error = true;
                            return;
                        }
                        lock.unlock_shared();
                    }
                    std::this_thread::yield();
                }
            });
        }

        // Locks
        for (int i = 0; i < 10; ++i) {
            readers.emplace_back([&lock, &error, &done, &counter] {
                while (!done) {
                    if (!lock.lock_shared()) {
                        error = true;
                        return;
                    }
                    if (counter.fetch_add(2, std::memory_order_relaxed) % 2 != 0) {
                        // Odd which means a writer is active
                        error = true;
                        return;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(15));
                    if (counter.fetch_sub(2, std::memory_order_relaxed) % 2 != 0) {
                        // Odd which means a writer is active
                        error = true;
                        return;
                    }
                    lock.unlock_shared();
                }
            });
        }

        std::thread writer([&lock, &error, &counter, &done]() {
            for (size_t i = 0; i < k_max_value; ++i) {
                if (!lock.lock_exclusive()) {
                    error = true;
                    return;
                }
                if (counter.fetch_add(1, std::memory_order_relaxed) > 0) {
                    error = true;
                    return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                if (counter.fetch_sub(1, std::memory_order_relaxed) != 1) {
                    error = true;
                    return;
                };
                lock.unlock_exclusive();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            done = true;
        });

        writer.join();

        for (auto& reader : readers) {
            reader.join();
        }

        REQUIRE_FALSE(error);
    }
}
