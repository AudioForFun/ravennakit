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

#include "rtsp_request.hpp"
#include "rtsp_response.hpp"
#include "ravennakit/core/containers/string_buffer.hpp"
#include "ravennakit/core/util/safe_function.hpp"

namespace rav::rtsp {

/**
 * Parses RTSP messages.
 */
class Parser final {
  public:
    static constexpr auto k_loop_upper_bound = 100'000;

    SafeFunction<void(const Request& request)> on_request;
    SafeFunction<void(const Response& response)> on_response;

    /**
     * The status of parsing.
     */
    enum class result {
        good,
        indeterminate,
        bad_method,
        bad_uri,
        bad_protocol,
        bad_version,
        bad_header,
        bad_end_of_headers,
        bad_status_code,
        bad_reason_phrase,
        unexpected_blank_line,
    };

    Parser() = default;

    /**
     * Parses the input and returns the result.
     * When a complete message is parsed, the appropriate event is emitted.
     * @param input The input to parse.
     * @return The result of the parsing.
     */
    result parse(StringBuffer& input);

    /**
     * Resets the state to initial state. This also removes event subscribers.
     */
    void reset() noexcept;

  private:
    enum class state {
        start,
        headers,
        data,
        complete,
    } state_ {state::start};

    std::string start_line_;
    Headers headers_;
    std::string data_;

    Request request_;
    Response response_;

    result handle_response();
    result handle_request();
};

}  // namespace rav::rtsp
