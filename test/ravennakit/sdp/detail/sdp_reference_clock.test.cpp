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

#include "ravennakit/sdp/detail/sdp_reference_clock.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::sdp::ReferenceClock") {
    SECTION("Test IEEE1588-2008 with domain") {
        const auto str = "ptp=IEEE1588-2008:39-A7-94-FF-FE-07-CB-D0:1";
        auto ref_clock = rav::sdp::parse_reference_clock(str);
        REQUIRE(ref_clock);
        REQUIRE(ref_clock->source_ == rav::sdp::ReferenceClock::ClockSource::ptp);
        REQUIRE(ref_clock->ptp_version_ == rav::sdp::ReferenceClock::PtpVersion::IEEE_1588_2008);
        REQUIRE(ref_clock->gmid_ == "39-A7-94-FF-FE-07-CB-D0");
        REQUIRE(ref_clock->domain_ == 1);
    }

    SECTION("Test IEEE1588-2002:traceable") {
        const auto str = "ptp=IEEE1588-2002:traceable";
        auto ref_clock = rav::sdp::parse_reference_clock(str);
        REQUIRE(ref_clock);
        REQUIRE(ref_clock->source_ == rav::sdp::ReferenceClock::ClockSource::ptp);
        REQUIRE(ref_clock->ptp_version_ == rav::sdp::ReferenceClock::PtpVersion::IEEE_1588_2002);
        REQUIRE(ref_clock->gmid_ == "traceable");
        REQUIRE_FALSE(ref_clock->domain_.has_value());
    }

    SECTION("Test IEEE802.1AS-2011") {
        const auto str = "ptp=IEEE802.1AS-2011:39-A7-94-FF-FE-07-CB-D0";
        auto ref_clock = rav::sdp::parse_reference_clock(str);
        REQUIRE(ref_clock);
        REQUIRE(ref_clock->source_ == rav::sdp::ReferenceClock::ClockSource::ptp);
        REQUIRE(ref_clock->ptp_version_ == rav::sdp::ReferenceClock::PtpVersion::IEEE_802_1AS_2011);
        REQUIRE(ref_clock->gmid_ == "39-A7-94-FF-FE-07-CB-D0");
        REQUIRE_FALSE(ref_clock->domain_.has_value());
    }

    SECTION("Test traceable") {
        const auto str = "ptp=traceable";
        auto ref_clock = rav::sdp::parse_reference_clock(str);
        REQUIRE(ref_clock);
        REQUIRE(ref_clock->source_ == rav::sdp::ReferenceClock::ClockSource::ptp);
        REQUIRE(ref_clock->ptp_version_ == rav::sdp::ReferenceClock::PtpVersion::traceable);
        REQUIRE_FALSE(ref_clock->gmid_.has_value());
        REQUIRE_FALSE(ref_clock->domain_.has_value());
    }
}
