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

#include "ravennakit/core/json.hpp"

#include <optional>
#include <string>

namespace rav::nmos {

/**
 * Transport file parameters. 'data' and 'type' must both be strings or both be null. If 'type' is non-null 'data' is
 * expected to contain a valid instance of the specified media type."
 */
struct TransportFile {
    /// Content of the transport file
    std::optional<std::string> data {};

    /// IANA assigned media type for file (e.g. application/sdp)
    std::optional<std::string> type {};
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const TransportFile& transport_file) {
    jv = {
        {"data", boost::json::value_from(transport_file.data)},
        {"type", boost::json::value_from(transport_file.type)},
    };
}

inline TransportFile tag_invoke(const boost::json::value_to_tag<TransportFile>&, const boost::json::value& jv) {
    TransportFile transport_file;
    transport_file.data = jv.at("data").as_string();
    transport_file.type = jv.at("type").as_string();
    return transport_file;
}

}  // namespace rav::nmos