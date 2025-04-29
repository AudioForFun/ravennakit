/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
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

}
