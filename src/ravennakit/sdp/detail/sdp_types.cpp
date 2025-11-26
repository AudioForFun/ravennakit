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

#include "ravennakit/sdp/detail/sdp_types.hpp"

#include "ravennakit/sdp/detail/sdp_constants.hpp"

const char* rav::sdp::to_string(const NetwType& type) {
    switch (type) {
        case NetwType::internet:
            return k_sdp_inet;
        case NetwType::undefined:
        default:
            return "undefined";
    }
}

const char* rav::sdp::to_string(const AddrType& type) {
    switch (type) {
        case AddrType::ipv4:
            return k_sdp_ipv4;
        case AddrType::ipv6:
            return k_sdp_ipv6;
        case AddrType::both:
            return k_sdp_wildcard;
        case AddrType::undefined:
        default:
            return "undefined";
    }
}

const char* rav::sdp::to_string(const MediaDirection& direction) {
    switch (direction) {
        case MediaDirection::sendrecv:
            return k_sdp_sendrecv;
        case MediaDirection::sendonly:
            return k_sdp_sendonly;
        case MediaDirection::recvonly:
            return k_sdp_recvonly;
        case MediaDirection::inactive:
            return k_sdp_inactive;
        default:
            return "undefined";
    }
}

const char* rav::sdp::to_string(const FilterMode& filter_mode) {
    switch (filter_mode) {
        case FilterMode::exclude:
            return "excl";
        case FilterMode::include:
            return "incl";
        case FilterMode::undefined:
        default:
            return "undefined";
    }
}
