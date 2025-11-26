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

#include "ravennakit/core/sync/atomic_rw_lock.hpp"

#include <catch2/catch_all.hpp>

#include <future>
#include <thread>

static_assert(!std::is_copy_constructible_v<rav::AtomicRwLock>);
static_assert(!std::is_move_constructible_v<rav::AtomicRwLock>);
static_assert(!std::is_copy_assignable_v<rav::AtomicRwLock>);
static_assert(!std::is_move_assignable_v<rav::AtomicRwLock>);

TEST_CASE("rav::AtomicRwLock") {
    SECTION("Test basic operation") {
        rav::AtomicRwLock lock;

        {
            auto guard = lock.lock_exclusive();
            REQUIRE(guard);

            REQUIRE(lock.is_locked());
            REQUIRE(lock.is_locked_exclusively());

            auto guard2 = lock.try_lock_shared();
            REQUIRE_FALSE(guard2);

            auto guard3 = lock.try_lock_exclusive();
            REQUIRE_FALSE(guard3);

            REQUIRE(lock.is_locked());
            REQUIRE(lock.is_locked_exclusively());
        }

        {
            auto guard = lock.lock_shared();
            REQUIRE(guard);

            REQUIRE(lock.is_locked());
            REQUIRE(lock.is_locked_shared());

            auto guard2 = lock.lock_shared();
            REQUIRE(guard2);

            auto guard3 = lock.try_lock_shared();
            REQUIRE(guard3);

            auto guard4 = lock.try_lock_exclusive();
            REQUIRE_FALSE(guard4);

            REQUIRE(lock.is_locked());
            REQUIRE(lock.is_locked_shared());
        }

        {
            auto guard = lock.lock_exclusive();
            REQUIRE(guard);

            REQUIRE(lock.is_locked());
            REQUIRE(lock.is_locked_exclusively());
        }

        REQUIRE_FALSE(lock.is_locked());
        REQUIRE_FALSE(lock.is_locked_shared());
        REQUIRE_FALSE(lock.is_locked_exclusively());
    }

    SECTION("Multiple writers, multiple readers") {
        rav::AtomicRwLock lock;

        std::atomic<bool> error {};
        std::atomic<int8_t> exclusive_counter {};

        std::vector<std::thread> readers;
        readers.reserve(10);
        for (size_t i = 0; i < readers.capacity(); ++i) {
            readers.emplace_back([&lock, &exclusive_counter, &error] {
                int succeeded_times = 0;
                while (succeeded_times < 10) {
                    const auto guard = lock.try_lock_shared();
                    if (!guard) {
                        continue;
                    }

                    if (exclusive_counter.fetch_add(2, std::memory_order_relaxed) % 2 != 0) {
                        // Odd which means a writer is active
                        error = true;
                        return;
                    }
                    succeeded_times++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(15));
                    if (exclusive_counter.fetch_sub(2, std::memory_order_relaxed) % 2 != 0) {
                        // Odd which means a writer is active
                        error = true;
                        return;
                    }
                }
            });
        }

        std::vector<std::thread> try_readers;
        try_readers.reserve(10);
        for (size_t i = 0; i < try_readers.capacity(); ++i) {
            try_readers.emplace_back([&lock, &error, &exclusive_counter] {
                int succeeded_times = 0;
                while (succeeded_times < 10) {
                    const auto guard = lock.lock_shared();
                    if (!guard) {
                        error = true;
                        return;
                    }

                    if (exclusive_counter.fetch_add(2, std::memory_order_relaxed) % 2 != 0) {
                        // Odd which means a writer is active
                        error = true;
                        return;
                    }
                    succeeded_times++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(15));
                    if (exclusive_counter.fetch_sub(2, std::memory_order_relaxed) % 2 != 0) {
                        // Odd which means a writer is active
                        error = true;
                        return;
                    }
                }
            });
        }

        // Writers
        std::vector<std::thread> writers;
        writers.reserve(10);
        for (size_t i = 0; i < writers.capacity(); ++i) {
            writers.emplace_back([&lock, &error, &exclusive_counter] {
                int succeeded_times = 0;
                while (succeeded_times < 10) {
                    const auto guard = lock.lock_exclusive();
                    if (!guard) {
                        error = true;
                        return;
                    }
                    if (exclusive_counter.fetch_add(1, std::memory_order_relaxed) > 0) {
                        error = true;
                        return;
                    }
                    succeeded_times++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    if (exclusive_counter.fetch_sub(1, std::memory_order_relaxed) != 1) {
                        error = true;
                        return;
                    }
                }
            });
        }

        for (auto& writer : writers) {
            writer.join();
        }

        for (auto& reader : readers) {
            reader.join();
        }

        for (auto& reader : try_readers) {
            reader.join();
        }

        REQUIRE_FALSE(error);
    }

    SECTION("Try lock shared should always succeed when not locked exclusively") {
        static constexpr size_t iterations = 100'000;
        static constexpr int num_readers = 10;

        rav::AtomicRwLock lock;
        std::atomic failure {false};
        std::vector<std::thread> readers;
        std::atomic<int8_t> num_readers_ready {};
        readers.reserve(10);

        for (size_t i = 0; i < readers.capacity(); ++i) {
            readers.emplace_back([&lock, &num_readers_ready, &failure] {
                num_readers_ready.fetch_add(1);

                while (num_readers_ready.load() < num_readers) {
                    std::this_thread::yield();
                }

                for (size_t times = 0; times < iterations; ++times) {
                    if (failure) {
                        break;
                    }
                    const auto guard = lock.try_lock_shared();
                    if (!guard) {
                        failure = true;
                    }
                }
            });
        }

        for (auto& reader : readers) {
            reader.join();
        }

        REQUIRE_FALSE(failure);
    }
}
