/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/ptp/types/ptp_time_interval.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("ptp_time_interval") {
    SECTION("Default constructor initializes to zero") {
        rav::ptp_time_interval interval;
        REQUIRE(interval.nanos() == 0);
        REQUIRE(interval.fraction() == 0);
    }

    SECTION("Constructor initializes with correct values") {
        rav::ptp_time_interval interval(5, 100);
        REQUIRE(interval.nanos() == 5);
        REQUIRE(interval.fraction() == 100);

        rav::ptp_time_interval negative_interval(-5, 50000);
        REQUIRE(negative_interval.nanos() == -5);
        REQUIRE(negative_interval.fraction() == 50000);
    }

    SECTION("Normalization of negative fractional part") {
        rav::ptp_time_interval interval(3, -10000);
        REQUIRE(interval.nanos() == 2); // Borrow 1 from nanos
        REQUIRE(interval.fraction() == rav::ptp_time_interval::k_fractional_scale - 10000);

        rav::ptp_time_interval interval2(-3, -10000);
        REQUIRE(interval2.nanos() == -4);
        REQUIRE(interval2.fraction() == rav::ptp_time_interval::k_fractional_scale - 10000);
    }

    SECTION("Normalization of overflow in fractional part") {
        rav::ptp_time_interval interval(3, rav::ptp_time_interval::k_fractional_scale + 10000);
        REQUIRE(interval.nanos() == 4); // Carry 1 to nanos
        REQUIRE(interval.fraction() == 10000);

        rav::ptp_time_interval interval2(-3, rav::ptp_time_interval::k_fractional_scale + 10000);
        REQUIRE(interval2.nanos() == -2);
        REQUIRE(interval2.fraction() == 10000);
    }

    SECTION("Arithmetic addition works correctly") {
        rav::ptp_time_interval interval1(3, 50000);
        rav::ptp_time_interval interval2(4, 70000);

        rav::ptp_time_interval result = interval1 + interval2;
        REQUIRE(result.nanos() == 8);
        REQUIRE(result.fraction() == 54464);

        // Check normalization during addition
        rav::ptp_time_interval interval3(3, rav::ptp_time_interval::k_fractional_scale - 10000);
        rav::ptp_time_interval interval4(1, 20000);

        rav::ptp_time_interval result2 = interval3 + interval4;
        REQUIRE(result2.nanos() == 5);
        REQUIRE(result2.fraction() == 10000);
    }

    SECTION("Arithmetic subtraction works correctly") {
        rav::ptp_time_interval interval1(7, 120000);
        rav::ptp_time_interval interval2(4, 70000);

        rav::ptp_time_interval result = interval1 - interval2;
        REQUIRE(result.nanos() == 3);
        REQUIRE(result.fraction() == 50000);

        // Check normalization during subtraction
        rav::ptp_time_interval interval3(5, 10000);
        rav::ptp_time_interval interval4(3, rav::ptp_time_interval::k_fractional_scale - 20000);

        rav::ptp_time_interval result2 = interval3 - interval4;
        REQUIRE(result2.nanos() == 1);
        REQUIRE(result2.fraction() == 30000);
    }

    SECTION("From wire positive") {
        const auto interval = rav::ptp_time_interval::from_wire_format(0x24000);
        REQUIRE(interval.nanos() == 0x2);
        REQUIRE(interval.fraction() == 0x4000);
    }

    SECTION("From wire negative") {
        const auto interval = rav::ptp_time_interval::from_wire_format(-0x24000);
        REQUIRE(interval.nanos() == -0x3);
        REQUIRE(interval.fraction() == 0xc000); // 0x10000 - 0x4000
    }

    SECTION("To wire positive") {
        const auto interval = rav::ptp_time_interval(0x2, 0x4000);
        REQUIRE(interval.to_wire_format() == 0x24000);
    }

    SECTION("To wire negative") {
        const auto interval = rav::ptp_time_interval(-0x3, 0xc000);
        REQUIRE(interval.to_wire_format() == -0x24000);
    }

    SECTION("Equality and inequality operators") {
        rav::ptp_time_interval interval1(5, 10000);
        rav::ptp_time_interval interval2(5, 10000);
        rav::ptp_time_interval interval3(6, 20000);

        REQUIRE(interval1 == interval2);
        REQUIRE(interval1 != interval3);
        REQUIRE(interval2 != interval3);
    }

    SECTION("Chained arithmetic operations") {
        rav::ptp_time_interval interval1(1, 0x2000);
        rav::ptp_time_interval interval2(2, 0x4000);
        rav::ptp_time_interval interval3(3, 0x8000);

        rav::ptp_time_interval result = interval1 + interval2 - interval3;
        REQUIRE(result.nanos() == -1);
        REQUIRE(result.fraction() == 0xe000);
    }

    SECTION("Self-assignment operators") {
        rav::ptp_time_interval interval1(5, 10000);
        rav::ptp_time_interval interval2(3, 20000);

        interval1 += interval2;
        REQUIRE(interval1.nanos() == 8);
        REQUIRE(interval1.fraction() == 30000);

        interval1 -= interval2;
        REQUIRE(interval1.nanos() == 5);
        REQUIRE(interval1.fraction() == 10000);
    }

    SECTION("Nanos rounded") {
        rav::ptp_time_interval interval1(5, 0x8000);
        REQUIRE(interval1.nanos_rounded() == 6);

        rav::ptp_time_interval interval2(5, 0x7fff);
        REQUIRE(interval2.nanos_rounded() == 5);
    }
}
