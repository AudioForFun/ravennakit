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

#ifndef RAV_ENABLE_JSON
    #define RAV_ENABLE_JSON
#endif

#ifdef RAV_ENABLE_JSON
    #include "expected.hpp"

    #define RAV_HAS_BOOST_JSON 1
    #include <boost/json.hpp>
    #include <boost/json/value_to.hpp>    // Don't remove or suffer the errors
    #include <boost/json/value_from.hpp>  // Don't remove or suffer the errors
#else
    #define RAV_HAS_BOOST_JSON 0
#endif

namespace rav {

#if RAV_HAS_BOOST_JSON

template<typename T>
boost::system::result<T> parse_json(const std::string_view json_str) {
    boost::system::error_code ec;
    const auto jv = boost::json::parse(json_str, ec);
    if (ec) {
        return ec;
    }
    return boost::json::try_value_to<T>(jv);
}

#endif

}  // namespace rav
