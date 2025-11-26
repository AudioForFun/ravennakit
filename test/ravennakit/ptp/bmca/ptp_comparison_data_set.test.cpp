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

#include "ravennakit/ptp/bmca/ptp_comparison_data_set.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::ptp::ComparisonDataSet") {
    rav::ptp::ComparisonDataSet a;
    a.grandmaster_priority1 = 128;
    a.grandmaster_identity.data = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    a.grandmaster_clock_quality.clock_class = 0x12;
    a.grandmaster_clock_quality.clock_accuracy = rav::ptp::ClockAccuracy::lt_10_ns;
    a.grandmaster_clock_quality.offset_scaled_log_variance = 0x1234;
    a.grandmaster_priority2 = 128;
    a.steps_removed = 10;
    a.identity_of_senders.data = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    a.identity_of_receiver.clock_identity.data = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    a.identity_of_receiver.port_number = 2;
    rav::ptp::ComparisonDataSet b = a;  // Start with equal sets

    SECTION("Grandmaster identity is equal") {
        SECTION("Steps removed of a is better") {
            a.steps_removed -= 2;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better);
        }
        SECTION("Steps removed of a is worse") {
            a.steps_removed += 2;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse);
        }

        SECTION("Steps removed of a is better") {
            a.steps_removed--;
            SECTION("Receiver < Sender") {
                b.identity_of_receiver.clock_identity.data[0] = 0x00;
                REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better);
            }
            SECTION("Receiver > Sender") {
                b.identity_of_receiver.clock_identity.data[0] = 0x02;
                REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better_by_topology);
            }
        }

        SECTION("Steps removed of a is worse") {
            a.steps_removed++;
            SECTION("Receiver < Sender") {
                a.identity_of_receiver.clock_identity.data[0] = 0x00;
                REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse);
            }
            SECTION("Receiver > Sender") {
                a.identity_of_receiver.clock_identity.data[0] = 0x02;
                REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse_by_topology);
            }
        }

        SECTION("A has better sender identity") {
            a.identity_of_senders.data[0] = 0x02;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse_by_topology);
        }

        SECTION("A has worse sender identity") {
            a.identity_of_senders.data[0] = 0x00;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better_by_topology);
        }

        SECTION("A has higher port number") {
            a.identity_of_receiver.port_number++;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse_by_topology);
        }

        SECTION("A has lower port number") {
            a.identity_of_receiver.port_number--;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better_by_topology);
        }
    }

    SECTION("Grandmaster identity is not equal") {
        b.grandmaster_identity.data[0] = 0x00;

        SECTION("GM Priority1 of a is better") {
            a.grandmaster_priority1--;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better);
        }

        SECTION("GM Priority1 of a is worse") {
            a.grandmaster_priority1++;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse);
        }

        SECTION("GM Clock class of a is better") {
            a.grandmaster_clock_quality.clock_class--;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better);
        }

        SECTION("GM Clock class of a is worse") {
            a.grandmaster_clock_quality.clock_class++;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse);
        }

        SECTION("GM Clock accuracy of a is better") {
            a.grandmaster_clock_quality.clock_accuracy = rav::ptp::ClockAccuracy::lt_2_5_ns;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better);
        }

        SECTION("GM Clock accuracy of a is worse") {
            a.grandmaster_clock_quality.clock_accuracy = rav::ptp::ClockAccuracy::lt_25_ns;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse);
        }

        SECTION("GM Offset scaled log variance of a is better") {
            a.grandmaster_clock_quality.offset_scaled_log_variance--;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better);
        }

        SECTION("GM Offset scaled log variance of a is worse") {
            a.grandmaster_clock_quality.offset_scaled_log_variance++;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse);
        }

        SECTION("GM Priority2 of a is better") {
            a.grandmaster_priority2--;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better);
        }

        SECTION("GM Priority2 of a is worse") {
            a.grandmaster_priority2++;
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse);
        }

        SECTION("Tie breaker") {
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::better);
        }

        b.grandmaster_identity.data[0] = 0x02;

        SECTION("Tie breaker") {
            REQUIRE(a.compare(b) == rav::ptp::ComparisonDataSet::result::worse);
        }
    }

    SECTION("Order of ordering") {
        using ordering = rav::ptp::ComparisonDataSet::result;
        REQUIRE(ordering::worse_by_topology > ordering::worse);
        REQUIRE(ordering::error1 > ordering::worse_by_topology);
        REQUIRE(ordering::error2 > ordering::error1);
        REQUIRE(ordering::better_by_topology > ordering::error2);
        REQUIRE(ordering::better > ordering::better_by_topology);
    }
}
