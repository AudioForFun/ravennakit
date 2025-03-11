/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/core/sync/rcu.hpp"

#include <catch2/catch_all.hpp>

static_assert(!std::is_copy_constructible_v<rav::rcu<int>>);
static_assert(!std::is_move_constructible_v<rav::rcu<int>>);
static_assert(!std::is_copy_assignable_v<rav::rcu<int>>);
static_assert(!std::is_move_assignable_v<rav::rcu<int>>);

static_assert(!std::is_copy_constructible_v<rav::rcu<int>::reader>);
static_assert(!std::is_move_constructible_v<rav::rcu<int>::reader>);
static_assert(!std::is_copy_assignable_v<rav::rcu<int>::reader>);
static_assert(!std::is_move_assignable_v<rav::rcu<int>::reader>);

static_assert(!std::is_copy_constructible_v<rav::rcu<int>::reader::read_lock>);
static_assert(!std::is_move_constructible_v<rav::rcu<int>::reader::read_lock>);
static_assert(!std::is_copy_assignable_v<rav::rcu<int>::reader::read_lock>);
static_assert(!std::is_move_assignable_v<rav::rcu<int>::reader::read_lock>);

TEST_CASE("rcu") {
    rav::rcu<std::string> rcu;
    rav::rcu<std::string>::reader reader(rcu);

    {
        const auto lock = reader.lock();
        REQUIRE(lock.get() == nullptr);

        rcu.update("Hello, World!");

        // As long as the first lock is alive, the value won't be updated for subsequent locks of the same reader.
        const auto lock2 = reader.lock();
        REQUIRE(lock2.get() == nullptr);
    }

    // Once the previous locks are destroyed, the value will be updated for new locks.
    const auto lock3 = reader.lock();
    REQUIRE(*lock3 == "Hello, World!");

    // TODO: Add more tests.
}
