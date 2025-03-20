/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/condition.hpp"

#include <thread>
#include <catch2/catch_all.hpp>

TEST_CASE("condition", "[condition]") {
    SECTION("Test signaling") {
        rav::Condition signal;

        std::thread t1([&signal] {
            signal.wait();
        });

        std::thread t2([&signal] {
            signal.signal();
        });

        t1.join();
        t2.join();

        // Waiting without resetting should not wait
        std::thread t3([&signal] {
            signal.wait();
        });

        t3.join();
    }

    SECTION("Test timeout") {
        rav::Condition signal;
        REQUIRE(signal.wait_for_ms(1) == false);
        signal.signal();
        REQUIRE(signal.wait_for_ms(1) == true);
    }
}
