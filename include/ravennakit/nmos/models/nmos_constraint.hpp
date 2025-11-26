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

#include <variant>

namespace rav::nmos {

/**
 * Definition of a single constraint record.
 */
struct Constraint {
    /// The inclusive maximum value the parameter can be set to.
    std::variant<std::monostate, int, float> maximum;
    /// The inclusive minimum value the parameter can be set to.
    std::variant<std::monostate, int, float> minimum;
    /// An array of allowed values
    std::vector<std::variant<bool, int, float, std::string>> enum_value;
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const Constraint& constraint) {
    auto& obj = jv.emplace_object();

    if (!std::get_if<std::monostate>(&constraint.maximum)) {
        obj["maximum"] = boost::json::value_from(constraint.maximum);
    }

    if (!std::get_if<std::monostate>(&constraint.minimum)) {
        obj["minimum"] = boost::json::value_from(constraint.minimum);
    }

    if (!constraint.enum_value.empty()) {
        obj["enum"] = boost::json::value_from(constraint.enum_value);
    }
}

}  // namespace rav::nmos
