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

#include "ravennakit/nmos/detail/nmos_timestamp.hpp"

#include <boost/json/value.hpp>
#include <boost/json/value_from.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <map>

namespace rav::nmos {

struct ResourceCore {
    /// Globally unique identifier for the resource
    boost::uuids::uuid id;

    /// String formatted TAI timestamp (<seconds>:<nanoseconds>) indicating precisely when an attribute of the resource
    /// last changed
    Version version;

    /// Freeform string label for the resource
    std::string label;

    /// Detailed description of the resource
    std::string description;

    /// Key value set of freeform string tags to aid in filtering resources. Values should be represented as an array of
    /// strings. Can be empty.
    std::map<std::string, std::vector<std::string>> tags;
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const ResourceCore& resource) {
    jv = {
        {"id", boost::uuids::to_string(resource.id)}, {"version", resource.version.to_string()},        {"label", resource.label},
        {"description", resource.description},        {"tags", boost::json::value_from(resource.tags)},
    };
}

}  // namespace rav::nmos
