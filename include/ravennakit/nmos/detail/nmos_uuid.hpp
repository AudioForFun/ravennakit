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

#include <boost/uuid.hpp>
#include <boost/json.hpp>

inline boost::json::value json_value_from_uuid(const boost::uuids::uuid& uuid) {
    return {boost::uuids::to_string(uuid)};
}

inline boost::json::value json_value_from_uuid(const std::optional<boost::uuids::uuid>& uuid) {
    if (uuid.has_value()) {
        return json_value_from_uuid(*uuid);
    }
    return {};
}

inline std::optional<boost::uuids::uuid> uuid_from_json(const boost::json::value& json) {
    if (!json.is_string()) {
        return std::nullopt;
    }
    const auto str = json.as_string();
    if (str.empty()) {
        return std::nullopt;
    }
    return boost::uuids::string_generator()(str.c_str());
}
