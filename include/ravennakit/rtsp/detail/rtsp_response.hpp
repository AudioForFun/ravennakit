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

#include <string>

namespace rav::rtsp {

/**
 * Structure that represents an RTSP response.
 */
struct Response {
    int status_code {};
    std::string reason_phrase;
    int rtsp_version_major {1};
    int rtsp_version_minor {0};
    Headers rtsp_headers;
    std::string data;

    Response() = default;
    Response(int status, const std::string& reason);
    Response(int status, std::string reason, std::string data_);

    /**
     * Clears the contents of the request.
     */
    void clear() {
        status_code = {};
        reason_phrase.clear();
        rtsp_version_major = {};
        rtsp_version_minor = {};
        rtsp_headers.clear();
        data.clear();
    }

    /**
     * Encode the response into a string, meant for sending over the wire.
     * @param newline The newline character to use.
     * @return The encoded response as a string.
     */
    std::string encode(const char* newline = "\r\n") const;

    /**
     * Encoded the response into a string.
     * @param out The output string to append to.
     * @param newline The newline character to use.
     */
    void encode_append(std::string& out, const char* newline = "\r\n") const;

    /**
     * Convert the response to a debug string.
     * @return The response as a debug string.
     */
    [[nodiscard]] std::string to_debug_string(bool include_data) const;
};

}  // namespace rav::rtsp
