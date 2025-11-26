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

#include "ravennakit/rtsp/detail/rtsp_request.hpp"

#include "ravennakit/core/util/uri.hpp"

#include <catch2/catch_all.hpp>

void rav::rtsp::Request::clear() {
    method.clear();
    uri.clear();
    rtsp_version_major = {};
    rtsp_version_minor = {};
    rtsp_headers.clear();
    data.clear();
}

std::string rav::rtsp::Request::encode(const char* newline) const {
    std::string out;
    encode_append(out, newline);
    return out;
}

void rav::rtsp::Request::encode_append(std::string& out, const char* newline) const {
    fmt::format_to(std::back_inserter(out), "{} {} RTSP/{}.{}{}", method, uri, rtsp_version_major, rtsp_version_minor, newline);
    rtsp_headers.encode_append(out, true);
    if (!data.empty()) {
        fmt::format_to(std::back_inserter(out), "content-length: {}{}", data.size(), newline);
    }
    out += newline;
    out += data;
}

std::string rav::rtsp::Request::to_debug_string(bool include_data) const {
    std::string out;
    fmt::format_to(std::back_inserter(out), "{} {} RTSP/{}.{}", method, uri, rtsp_version_major, rtsp_version_minor);
    out += rtsp_headers.to_debug_string();
    if (include_data && !data.empty()) {
        out += "\n";
        out += string_replace(data, "\r\n", "\n");
    }
    return out;
}
