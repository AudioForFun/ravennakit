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

#include <boost/json/value.hpp>
#include <boost/json/value_to.hpp>
#include <boost/json/value_from.hpp>

#include <string>
#include <boost/beast/http/status.hpp>

namespace rav::nmos {

struct ApiError {
    unsigned code {};
    std::string error {};
    std::string debug {};

    ApiError() = default;

    ApiError(boost::beast::http::status status, std::string error_msg, std::string debug_msg = {}) :
        code(static_cast<decltype(code)>(status)), error(std::move(error_msg)), debug(std::move(debug_msg)) {
        if (debug.empty()) {
            debug = "error: " + error;
        }
    }
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const ApiError& value) {
    jv = {{"code", value.code}, {"error", value.error}, {"debug", value.debug}};
}

inline ApiError tag_invoke(const boost::json::value_to_tag<ApiError>&, const boost::json::value& jv) {
    auto obj = jv.as_object();
    ApiError error;
    error.code = static_cast<uint32_t>(obj.at("code").as_int64());
    error.error = obj.at("error").as_string();
    if (const auto v = obj.at("debug").try_as_string()) {
        error.debug = *v;
    }
    return error;
}

}  // namespace rav::nmos
