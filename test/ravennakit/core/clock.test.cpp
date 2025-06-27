/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/core/clock.hpp"

#include <catch2/catch_all.hpp>
#include <thread>

TEST_CASE("rav::clock") {
    SECTION("now_monotonic_high_resolution_ns()") {
        SECTION("now") {
            const auto now = rav::clock::now_monotonic_high_resolution_ns();
            REQUIRE(now > 0);
        }
        SECTION("Progression") {
            for (int i = 0; i < 100; ++i) {
                const auto now = rav::clock::now_monotonic_high_resolution_ns();
                std::this_thread::sleep_for(std::chrono::nanoseconds(100));
                REQUIRE(rav::clock::now_monotonic_high_resolution_ns() >= now + 100);
            }
        }
    }
}
