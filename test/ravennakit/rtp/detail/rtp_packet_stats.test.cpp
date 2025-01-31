/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/rtp/rtp_packet.hpp"
#include "ravennakit/rtp/detail/rtp_packet_stats.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rtp_packet_stats") {
    SECTION("Basic sequence") {
        rav::rtp_packet_stats stats;
        stats.update(10);
        stats.update(11);
        stats.update(12);
        const auto counts = stats.collect(rav::wrapping_uint16(11));
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_old == 0);
    }

    SECTION("Drop one packet") {
        rav::rtp_packet_stats stats;
        stats.update(10);
        REQUIRE(stats.collect(rav::wrapping_uint16(11)).dropped == 1);
        REQUIRE(stats.collect(rav::wrapping_uint16(11)).dropped == 0);
        REQUIRE(stats.collect(rav::wrapping_uint16(12)).dropped == 1);
    }

    SECTION("Drop two packets") {
        rav::rtp_packet_stats stats;
        stats.update(10);
        REQUIRE(stats.collect(rav::wrapping_uint16(12)).dropped == 2);
        REQUIRE(stats.collect(rav::wrapping_uint16(12)).dropped == 0);
        REQUIRE(stats.collect(rav::wrapping_uint16(14)).dropped == 2);
    }
}
