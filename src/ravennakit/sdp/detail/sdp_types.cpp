/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/sdp/detail/sdp_types.hpp"

#include "ravennakit/sdp/detail/sdp_constants.hpp"

std::string rav::sdp::to_string(const netw_type& type) {
    switch (type) {
        case netw_type::internet:
            return k_sdp_inet;
        case netw_type::undefined:
        default:
            return "undefined";
    }
}

std::string rav::sdp::to_string(const addr_type& type) {
    switch (type) {
        case addr_type::ipv4:
            return k_sdp_ipv4;
        case addr_type::ipv6:
            return k_sdp_ipv6;
        case addr_type::both:
            return k_sdp_wildcard;
        case addr_type::undefined:
        default:
            return "undefined";
    }
}

std::string rav::sdp::to_string(const media_direction& direction) {
    switch (direction) {
        case media_direction::sendrecv:
            return k_sdp_sendrecv;
        case media_direction::sendonly:
            return k_sdp_sendonly;
        case media_direction::recvonly:
            return k_sdp_recvonly;
        case media_direction::inactive:
            return k_sdp_inactive;
        default:
            return "undefined";
    }
}

std::string rav::sdp::to_string(const filter_mode& filter_mode) {
    switch (filter_mode) {
        case filter_mode::exclude:
            return "excl";
        case filter_mode::include:
            return "incl";
        case filter_mode::undefined:
        default:
            return "undefined";
    }
}
