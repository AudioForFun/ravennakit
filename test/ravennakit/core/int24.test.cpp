/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/int24.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("audio_data | int24_t", "[audio_data]") {
    SECTION("int32 to int24") {
        const rav::int24_t min(-8388608);
        REQUIRE(static_cast<int32_t>(min) == -8388608);

        const rav::int24_t max(8388607);
        REQUIRE(static_cast<int32_t>(max) == 8388607);

        const rav::int24_t zero(0);
        REQUIRE(static_cast<int32_t>(zero) == 0);
    }
}
