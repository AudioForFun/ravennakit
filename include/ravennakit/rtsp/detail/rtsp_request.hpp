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

#include "rtsp_headers.hpp"
#include "ravennakit/core/string_parser.hpp"

#include <string>

namespace rav::rtsp {

/**
 * Structure that represents an RTSP request.
 */
struct Request {
    std::string method;
    std::string uri;
    int rtsp_version_major {1};
    int rtsp_version_minor {0};
    Headers rtsp_headers;
    std::string data;

    Request() = default;

    Request(std::string request_method, std::string request_uri, std::string request_data = "") :
        method(std::move(request_method)), uri(std::move(request_uri)), data(std::move(request_data)) {}

    /**
     * Resets the request to its initial state.
     */
    void clear();

    /**
     * @return The encoded request as a string.
     */
    std::string encode(const char* newline = "\r\n") const;

    /**
     * Encoded the request into a string.
     * @param out The output string to append to.
     * @param newline
     */
    void encode_append(std::string& out, const char* newline = "\r\n") const;

    /**
     * Convert the request to a debug string.
     * @return The request as a debug string.
     */
    [[nodiscard]] std::string to_debug_string(bool include_data) const;
};

}  // namespace rav::rtsp
