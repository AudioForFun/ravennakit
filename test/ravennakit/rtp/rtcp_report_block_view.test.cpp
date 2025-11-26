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

#include <ravennakit/rtp/rtcp_report_block_view.hpp>

#include <catch2/catch_all.hpp>

namespace {
std::array<uint8_t, 24> default_packet {
    0x00, 0x01, 0x02, 0x03,  // SSRC
    0x04, 0x05, 0x06, 0x07,  // Fraction lost | cumulative number of packets lost
    0x08, 0x09, 0x0a, 0x0b,  // Extended highest sequence number received
    0x0c, 0x0d, 0x0e, 0x0f,  // Inter arrival jitter
    0x10, 0x11, 0x12, 0x13,  // Last SR (LSR)
    0x14, 0x15, 0x16, 0x17,  // Delay since last SR (DLSR)
};
}

TEST_CASE("rav::rtcp::ReportBlockView") {
    SECTION("rtcp_report_block_view::validate()", "[rtcp_report_block_view]") {
        SECTION("Validation should fail when the view doesn't point to data") {
            const rav::rtcp::ReportBlockView report(nullptr, 0);
            REQUIRE_FALSE(report.validate());
        }

        SECTION("Validation should fail when the packet is too short") {
            const rav::rtcp::ReportBlockView report(default_packet.data(), 23);
            REQUIRE_FALSE(report.validate());
        }

        SECTION("Validation should fail when the packet is too long") {
            const rav::rtcp::ReportBlockView report(default_packet.data(), 25);
            REQUIRE_FALSE(report.validate());
        }

        SECTION("Or else validation should pass") {
            const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
            REQUIRE(report.validate());
        }
    }

    SECTION("rtcp_report_block_view::ssrc()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        REQUIRE(report.ssrc() == 0x00010203);
    }

    SECTION("rtcp_report_block_view::fraction_lost()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        REQUIRE(report.fraction_lost() == 0x04);
    }

    SECTION("rtcp_report_block_view::number_of_packets_lost()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        REQUIRE(report.number_of_packets_lost() == 0x00050607);
    }

    SECTION("rtcp_report_block_view::extended_highest_sequence_number_received()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        REQUIRE(report.extended_highest_sequence_number_received() == 0x08090a0b);
    }

    SECTION("rtcp_report_block_view::inter_arrival_jitter()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        REQUIRE(report.inter_arrival_jitter() == 0x0c0d0e0f);
    }

    SECTION("rtcp_report_block_view::last_sr_timestamp()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        const auto ts = report.last_sr_timestamp();
        REQUIRE(ts.integer() == 0x1011);
        REQUIRE(ts.fraction() == 0x12130000);
    }

    SECTION("rtcp_report_block_view::delay_since_last_sr()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        REQUIRE(report.delay_since_last_sr() == 0x14151617);
    }

    SECTION("rtcp_report_block_view::data()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        REQUIRE(report.data() == default_packet.data());
    }

    SECTION("rtcp_report_block_view::size()", "[rtcp_report_block_view]") {
        const rav::rtcp::ReportBlockView report(default_packet.data(), default_packet.size());
        REQUIRE(report.size() == default_packet.size());
    }
}
