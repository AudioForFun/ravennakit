/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
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
