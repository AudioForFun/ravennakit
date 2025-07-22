/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/util/defer.hpp"

#include "catch2/catch_all.hpp"

TEST_CASE("rav::Defer") {
    int count = 0;
    SECTION("Defer with initial function") {
        {
            rav::Defer defer([&count] {
                count++;
            });
        }
        REQUIRE(count == 1);
    }

    SECTION("Defer won't happen when reset") {
        {
            rav::Defer defer([&count] {
                count++;
            });
            defer.release();
        }
        REQUIRE(count == 0);
    }
}
