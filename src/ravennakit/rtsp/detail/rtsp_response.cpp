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

#include "ravennakit/rtsp/detail/rtsp_response.hpp"

rav::rtsp::Response::Response(const int status, const std::string& reason) : Response(status, reason, {}) {}

rav::rtsp::Response::Response(const int status, std::string reason, std::string data_) :
    status_code(status), reason_phrase(std::move(reason)), data(std::move(data_)) {}

std::string rav::rtsp::Response::encode(const char* newline) const {
    std::string out;
    encode_append(out, newline);
    return out;
}

void rav::rtsp::Response::encode_append(std::string& out, const char* newline) const {
    fmt::format_to(
        std::back_inserter(out), "RTSP/{}.{} {} {}{}", rtsp_version_major, rtsp_version_minor, status_code, reason_phrase, newline
    );
    rtsp_headers.encode_append(out, true);
    if (!data.empty()) {
        fmt::format_to(std::back_inserter(out), "content-length: {}{}", data.size(), newline);
    }
    out += newline;
    out += data;
}

std::string rav::rtsp::Response::to_debug_string(const bool include_data) const {
    std::string out;
    fmt::format_to(std::back_inserter(out), "RTSP/{}.{} {} {}", rtsp_version_major, rtsp_version_minor, status_code, reason_phrase);
    out += rtsp_headers.to_debug_string();
    if (include_data && !data.empty()) {
        out += "\n";
        out += string_replace(data, "\r\n", "\n");
    }
    return out;
}
