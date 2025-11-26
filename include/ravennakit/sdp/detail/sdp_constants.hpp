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

#pragma once

namespace rav::sdp {

constexpr auto k_sdp_ptime = "ptime";
constexpr auto k_sdp_max_ptime = "maxptime";
constexpr auto k_sdp_rtp_map = "rtpmap";
constexpr auto k_sdp_sendrecv = "sendrecv";
constexpr auto k_sdp_sendonly = "sendonly";
constexpr auto k_sdp_recvonly = "recvonly";
constexpr auto k_sdp_inactive = "recvonly";
constexpr auto k_sdp_ts_refclk = "ts-refclk";
constexpr auto k_sdp_sync_time = "sync-time";
constexpr auto k_sdp_group = "group";
constexpr auto k_sdp_clock_deviation = "clock-deviation";
constexpr auto k_sdp_mid = "mid";
constexpr auto k_sdp_inet = "IN";
constexpr auto k_sdp_ipv4 = "IP4";
constexpr auto k_sdp_ipv6 = "IP6";
constexpr auto k_sdp_wildcard = "*";
constexpr auto k_sdp_crlf = "\r\n";

}  // namespace rav::sdp
