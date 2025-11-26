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

#include "ravennakit/core/sync/realtime_shared_object.hpp"

#include <future>
#include <catch2/catch_all.hpp>

static_assert(!std::is_copy_constructible_v<rav::RealtimeSharedObject<int>>);
static_assert(!std::is_move_constructible_v<rav::RealtimeSharedObject<int>>);
static_assert(!std::is_copy_assignable_v<rav::RealtimeSharedObject<int>>);
static_assert(!std::is_move_assignable_v<rav::RealtimeSharedObject<int>>);

static_assert(!std::is_copy_constructible_v<rav::RealtimeSharedObject<int>::RealtimeAccessGuard>);
static_assert(!std::is_move_constructible_v<rav::RealtimeSharedObject<int>::RealtimeAccessGuard>);
static_assert(!std::is_copy_assignable_v<rav::RealtimeSharedObject<int>::RealtimeAccessGuard>);
static_assert(!std::is_move_assignable_v<rav::RealtimeSharedObject<int>::RealtimeAccessGuard>);

TEST_CASE("rav::RealtimeSharedObject") {
    SECTION("Default state") {
        static constexpr auto k_string_a = "String                                           A";
        static constexpr auto k_string_b = "String                                           B";

        rav::RealtimeSharedObject<std::string> obj;
        {
            auto guard = obj.access_realtime();
            REQUIRE(guard.get() != nullptr);
            REQUIRE(guard->empty());
        }

        const auto old_empty_string = obj.update(k_string_a);
        REQUIRE(old_empty_string->empty());

        {
            auto guard = obj.access_realtime();
            REQUIRE(guard.get() != nullptr);
            REQUIRE(*guard == k_string_a);
        }

        const auto old_string_a = obj.update(k_string_b);
        REQUIRE(old_string_a);
        REQUIRE(*old_string_a == k_string_a);

        {
            auto guard = obj.access_realtime();
            REQUIRE(guard.get() != nullptr);
            REQUIRE(*guard == k_string_b);
        }

        const auto old_string_b = obj.update({});
        REQUIRE(old_string_b);
        REQUIRE(*old_string_b == k_string_b);
    }

    SECTION("Updating and reading the value should be thread safe") {
        static constexpr size_t num_values = 500;

        rav::RealtimeSharedObject<std::pair<size_t, std::string>> obj;

        std::atomic_bool keep_going {true};

        auto reader = std::async(std::launch::async, [&obj, &keep_going] {
            size_t num_values_read = 0;
            std::vector<std::string> values(num_values);

            while (num_values_read < num_values) {
                const auto guard = obj.access_realtime();
                if (guard.get() == nullptr) {
                    return std::vector<std::string>();
                }
                if (guard->second.empty()) {
                    continue;  // obj was default constructed
                }
                if (guard->first >= num_values) {
                    return std::vector<std::string>();  // obj was updated with an invalid index
                }
                auto& it = values.at(guard->first);
                if (it.empty()) {
                    it = guard->second;
                    ++num_values_read;
                }
            }

            keep_going = false;
            return values;
        });

        // Give reader thread some time to start.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        const auto writer = std::async(std::launch::async, [&obj, &keep_going] {
            // Writers are going to hammer the object with new values until the reader has read all values.
            while (keep_going) {
                for (size_t j = 0; j < num_values; ++j) {
                    std::ignore = obj.update(std::make_pair(j, std::to_string(j + 1)));
                    std::this_thread::yield();
                }
            }
        });

        const auto read_values = reader.get();
        keep_going = false;
        writer.wait();

        for (size_t i = 0; i < num_values; ++i) {
            REQUIRE(read_values[i] == std::to_string(i + 1));
        }
    }
}
