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
        rav::rtp_packet_stats stats(5);
        stats.update(10);
        stats.update(11);
        stats.update(12);
        stats.update(13);
        stats.update(14);
        REQUIRE(stats.count() == 5);
        const auto window = stats.get_window_counts();
        REQUIRE(window.dropped == 0);
        REQUIRE(window.duplicates == 0);
        REQUIRE(window.out_of_order == 0);
        REQUIRE(window.too_late == 0);
        REQUIRE(window.outside_window == 0);
        const auto total = stats.get_total_counts();
        REQUIRE(window == total);
    }

    SECTION("Drop one packet") {
        rav::rtp_packet_stats stats(3);
        stats.update(10);
        stats.update(12);
        REQUIRE(stats.count() == 3);
        const auto window = stats.get_window_counts();
        REQUIRE(window.dropped == 1);
        REQUIRE(window.duplicates == 0);
        REQUIRE(window.out_of_order == 0);
        REQUIRE(window.too_late == 0);
        REQUIRE(window.outside_window == 0);

        // Counters include the window
        auto total = stats.get_total_counts();
        REQUIRE(total.dropped == 1);
        REQUIRE(total.duplicates == 0);
        REQUIRE(total.out_of_order == 0);
        REQUIRE(total.too_late == 0);
        REQUIRE(total.outside_window == 0);

        // Move all previous packets out of the window
        stats.update(13);
        stats.update(14);
        stats.update(15);

        total = stats.get_total_counts();
        REQUIRE(total.dropped == 1);
        REQUIRE(total.duplicates == 0);
        REQUIRE(total.out_of_order == 0);
        REQUIRE(total.too_late == 0);
        REQUIRE(total.outside_window == 0);
    }

    SECTION("Drop two packets") {
        rav::rtp_packet_stats stats(5);
        stats.update(10);
        stats.update(13);
        REQUIRE(stats.count() == 4);
        const auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 2);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);

        stats.update(14);
        stats.update(15);
        stats.update(16);
        stats.update(17);

        const auto total = stats.get_total_counts();
        REQUIRE(total.dropped == 2);
        REQUIRE(total.duplicates == 0);
        REQUIRE(total.out_of_order == 0);
        REQUIRE(total.too_late == 0);
        REQUIRE(total.outside_window == 0);
    }

    SECTION("A packet older than the first packet is dropped") {
        rav::rtp_packet_stats stats(5);
        stats.update(10);
        stats.update(9);
        REQUIRE(stats.count() == 1);
        const auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);
    }

    SECTION("Too old") {
        rav::rtp_packet_stats stats(5);
        stats.update(10);
        stats.update(11);
        stats.update(12);
        stats.update(13);
        stats.update(14);
        stats.update(15);
        stats.update(10);
        REQUIRE(stats.count() == 5);
        const auto counts = stats.get_total_counts();
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 1);
    }

    SECTION("Drop, out of order, duplicates and too old") {
        rav::rtp_packet_stats stats(5);
        stats.update(10);
        stats.update(15);
        stats.update(10);  // Too old
        stats.update(13);  // Out of order
        stats.update(13);  // Our of order and duplicate
        REQUIRE(stats.count() == 5);
        const auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 3);  // Seq 11, 12 and 14 are dropped
        REQUIRE(counts.duplicates == 1);
        REQUIRE(counts.out_of_order == 2);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);
    }

    SECTION("Test the window") {
        rav::rtp_packet_stats stats(5);
        stats.update(10);
        // Skips 11, 12, 13, 14
        stats.update(15);
        stats.update(10);  // Too old
        stats.update(13);  // Out of order
        stats.update(13);  // Our of order and duplicate
        REQUIRE(stats.count() == 5);
        auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 3);     // Seq 11, 12 and 14 are dropped
        REQUIRE(counts.duplicates == 1);  // Seq 13 is duplicate
        REQUIRE(counts.out_of_order == 2);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);  // Window counts don't include too old packets

        // The window will slide one position, dropping seq 11
        stats.update(16);
        REQUIRE(stats.count() == 5);
        counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 2);  // Seq 12 and 14 are still dropped
        REQUIRE(counts.duplicates == 1);
        REQUIRE(counts.out_of_order == 2);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);

        // The window will slide three positions, dropping previous packets except 15.
        stats.update(17);
        stats.update(18);
        stats.update(19);
        stats.update(20);
        REQUIRE(stats.count() == 5);
        counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);

        auto totals = stats.get_total_counts();
        REQUIRE(totals.dropped == 3);
        REQUIRE(totals.duplicates == 1);
        REQUIRE(totals.out_of_order == 2);
        REQUIRE(totals.too_late == 0);
        REQUIRE(totals.outside_window == 1);
    }

    SECTION("Test wrap around") {
        rav::rtp_packet_stats stats(5);
        stats.reset(4);
        stats.update(0xffff - 2);
        stats.update(0xffff - 1);
        stats.update(0xffff);
        stats.update(0x0);
        REQUIRE(stats.count() == 4);
        auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);
    }

    SECTION("Test wrap around with drop") {
        rav::rtp_packet_stats stats(5);
        stats.reset(4);
        stats.update(0xffff - 2);
        stats.update(0xffff - 1);
        stats.update(0xffff);
        stats.update(0x1);
        REQUIRE(stats.count() == 4);
        auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 1);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);
    }

    SECTION("Test wrap around with drop, out of order, duplicates and too old") {
        rav::rtp_packet_stats stats(5);
        stats.reset(4);
        stats.update(0xffff - 2);
        stats.update(0x1);
        stats.update(0x1);
        stats.update(0xffff - 1);
        stats.update(0xffff);
        auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 1);
        REQUIRE(counts.duplicates == 1);
        REQUIRE(counts.out_of_order == 2);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);
    }

    SECTION("Mark too late") {
        rav::rtp_packet_stats stats(3);
        stats.update(1);
        stats.mark_packet_too_late(0);  // Too old
        stats.mark_packet_too_late(1);
        stats.mark_packet_too_late(2);  // Too new
        auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 1);
        REQUIRE(counts.outside_window == 0);

        stats.update(2);
        stats.update(3);
        stats.update(4);
        stats.update(5);
        stats.update(6);

        counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);

        auto totals = stats.get_total_counts();
        REQUIRE(totals.dropped == 0);
        REQUIRE(totals.duplicates == 0);
        REQUIRE(totals.out_of_order == 0);
        REQUIRE(totals.too_late == 1);
        REQUIRE(totals.outside_window == 0);
    }

    SECTION("Count 1 for every case, first the window, then the totals") {
        rav::rtp_packet_stats stats(5);
        stats.reset(4);
        stats.update(1);
        stats.update(4);
        stats.update(3);
        stats.update(5);
        stats.update(5);
        stats.mark_packet_too_late(3);

        auto window = stats.get_window_counts();
        REQUIRE(window.dropped == 1);
        REQUIRE(window.duplicates == 1);
        REQUIRE(window.out_of_order == 1);
        REQUIRE(window.too_late == 1);
        REQUIRE(window.outside_window == 0);

        auto totals = stats.get_total_counts();
        REQUIRE(totals.dropped == 1);
        REQUIRE(totals.duplicates == 1);
        REQUIRE(totals.out_of_order == 1);
        REQUIRE(totals.too_late == 1);
        REQUIRE(totals.outside_window == 0);

        // The window will have moved one position, dropping the first packet, so this one should be too old
        stats.update(1);
        totals = stats.get_total_counts();
        REQUIRE(totals.dropped == 1);
        REQUIRE(totals.duplicates == 1);
        REQUIRE(totals.out_of_order == 1);
        REQUIRE(totals.too_late == 1);
        REQUIRE(totals.outside_window == 1);

        // Slide the window so all values from the current window are collected in the totals
        stats.update(6);
        stats.update(7);
        stats.update(8);
        stats.update(9);
        totals = stats.get_total_counts();
        REQUIRE(totals.dropped == 1);
        REQUIRE(totals.duplicates == 1);
        REQUIRE(totals.out_of_order == 1);
        REQUIRE(totals.too_late == 1);
        REQUIRE(totals.outside_window == 1);

        window = stats.get_window_counts();
        REQUIRE(window == rav::rtp_packet_stats::counters {});
    }

    SECTION("Handling duplicates across the window") {
        rav::rtp_packet_stats stats(5);
        stats.reset(5);
        stats.update(100);
        stats.update(101);
        stats.update(101);
        stats.update(102);
        stats.update(102);
        stats.update(102);
        auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 3);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);
    }

    SECTION("Extreme out-of-order packets") {
        rav::rtp_packet_stats stats(5);
        stats.reset(5);
        stats.update(200);
        stats.update(205);
        stats.update(202);
        stats.update(204);
        stats.update(203);
        auto counts = stats.get_window_counts();
        REQUIRE(counts.out_of_order == 3);
    }

    SECTION("Reset behavior") {
        rav::rtp_packet_stats stats(5);
        stats.update(10);
        stats.update(12);
        stats.update(14);
        stats.mark_packet_too_late(12);
        stats.reset();
        auto counts = stats.get_window_counts();
        REQUIRE(counts.dropped == 0);
        REQUIRE(counts.duplicates == 0);
        REQUIRE(counts.out_of_order == 0);
        REQUIRE(counts.too_late == 0);
        REQUIRE(counts.outside_window == 0);
    }

    SECTION("Reset with new window size") {
        rav::rtp_packet_stats stats(5);
        stats.reset(3);
        stats.update(1);
        stats.update(2);
        stats.update(3);
        stats.update(4);
        REQUIRE(stats.count() == 3);
    }

    SECTION("Marking packets too late before arrival") {
        rav::rtp_packet_stats stats(5);
        stats.mark_packet_too_late(50);
        stats.update(50);
        stats.mark_packet_too_late(50);
        auto counts = stats.get_window_counts();
        REQUIRE(counts.too_late == 1);
    }

    SECTION("Continuous window updates with wraparound") {
        rav::rtp_packet_stats stats(5);
        stats.reset(4);
        for (uint16_t i = 0; i < 10; i++) {
            stats.update(0xfff0 + i * 2);
        }
        auto counts = stats.get_total_counts();
        REQUIRE(counts.dropped > 0);
    }

    SECTION("Handling maximum window size") {
        rav::rtp_packet_stats stats(0xffff);
        stats.update(0);
        stats.update(0xffff / 2 - 1);
        REQUIRE(stats.count() == 0x7fff);
        auto window = stats.get_window_counts();
        // auto totals = stats.get_total_counts();
        REQUIRE(window.dropped == 0x7ffd);
    }

    SECTION("Window + totals") {
        rav::rtp_packet_stats stats(0xf0);
        stats.update(0);
        stats.update(0xffff / 2 - 1);
        REQUIRE(stats.count() == 0xf0);
        auto totals = stats.get_total_counts();
        REQUIRE(totals.dropped == 0x7ffd);
    }

    SECTION("Test specific bug where the amount duplicates drops would suddenly jump to weird numbers") {
        rav::rtp_packet_stats stats(9);

        for (uint16_t i = 0; i < 0xffff; i++) {
            stats.update(i);
            INFO("i: " << i);
            REQUIRE(stats.get_total_counts() == rav::rtp_packet_stats::counters {});
        }

        stats.reset(0xffff);

        for (uint16_t i = 0; i < 0xffff; i++) {
            stats.update(i);
            INFO("i: " << i);
            REQUIRE(stats.get_total_counts() == rav::rtp_packet_stats::counters {});
        }
    }

    SECTION("Run couple of sequences, count drops") {
        rav::rtp_packet_stats stats(9);

        size_t dropped = 0;
        for (size_t i = 0; i < 3 * 0x10000; i++) {
            const auto seq = static_cast<uint16_t>(i);
            if (seq == 0x1) {
                dropped++;
                continue;  // Drop packet
            }
            stats.update(seq);
        }

        const auto window = stats.get_window_counts();
        const auto totals = stats.get_total_counts();
        REQUIRE(totals.dropped + window.dropped == dropped);
    }

    SECTION("Add counters") {
        rav::rtp_packet_stats::counters a {1, 2, 3, 4, 5};
        rav::rtp_packet_stats::counters b {1, 2, 3, 4, 5};
        auto c = a + b;
        REQUIRE(c.out_of_order == 2);
        REQUIRE(c.duplicates == 4);
        REQUIRE(c.dropped == 6);
        REQUIRE(c.too_late == 8);
        REQUIRE(c.outside_window == 10);
    }
}
