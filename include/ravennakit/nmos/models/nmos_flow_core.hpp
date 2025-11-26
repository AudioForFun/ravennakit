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
#include "nmos_resource_core.hpp"

namespace rav::nmos {

struct FlowCore: ResourceCore {
    /// Globally unique identifier for the Source which initially created the Flow.
    boost::uuids::uuid source_id;

    /// Globally unique identifier for the Device which initially created the Flow.
    boost::uuids::uuid device_id;

    /// Array of UUIDs representing the Flow IDs of Grains which came together to generate this Flow (may change over
    /// the lifetime of this Flow).
    std::vector<boost::uuids::uuid> parents;
};

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const FlowCore& flow_core) {
    tag_invoke(tag, jv, static_cast<const ResourceCore&>(flow_core));
    auto& object = jv.as_object();
    object["source_id"] = boost::uuids::to_string(flow_core.source_id);
    object["device_id"] = boost::uuids::to_string(flow_core.device_id);
    object["parents"] = boost::json::value_from(flow_core.parents);
}

}  // namespace rav::nmos
