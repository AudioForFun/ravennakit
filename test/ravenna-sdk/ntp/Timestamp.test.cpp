/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include <catch2/catch_all.hpp>
#include <ravenna-sdk/ntp/Timestamp.hpp>

TEST_CASE("Timestamp full resolution", "[Timestamp]") {
    const rav::ntp::Timestamp timestamp {0x01234567, 0x89abcdef};

    REQUIRE(timestamp.integer() == 0x01234567);
    REQUIRE(timestamp.fraction() == 0x89abcdef);
}

TEST_CASE("Timestamp from compact uint16", "[Timestamp]") {
    const auto ts = rav::ntp::Timestamp::from_compact(0x0123, 0x4567);

    REQUIRE(ts.integer() == 0x0123);
    REQUIRE(ts.fraction() == 0x45670000);
}

TEST_CASE("Timestamp from compact uint32", "[Timestamp]") {
    const auto ts = rav::ntp::Timestamp::from_compact(0x01234567);

    REQUIRE(ts.integer() == 0x0123);
    REQUIRE(ts.fraction() == 0x45670000);
}
