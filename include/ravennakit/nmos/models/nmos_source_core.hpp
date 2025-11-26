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

/**
 * Describes a source.
 * https://specs.amwa.tv/is-04/releases/v1.3.3/APIs/schemas/with-refs/source_core.html
 */
struct SourceCore: ResourceCore {
    /// Globally unique identifier for the Device which initially created the Source. This attribute is used to ensure
    /// referential integrity by registry implementations.
    boost::uuids::uuid device_id;

    /// Array of UUIDs representing the Source IDs of Grains which came together at the input to this Source (may change
    /// over the lifetime of this Source)
    std::vector<boost::uuids::uuid> parents;

    /// Reference to clock in the originating Node.
    std::optional<std::string> clock_name;
};

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const SourceCore& source) {
    tag_invoke(tag, jv, static_cast<const ResourceCore&>(source));
    auto& obj = jv.as_object();
    obj["caps"] = boost::json::object();  // Capabilities (not yet defined)
    obj["device_id"] = boost::uuids::to_string(source.device_id);
    obj["parents"] = boost::json::value_from(source.parents);
    obj["clock_name"] = boost::json::value_from(source.clock_name);
}

}  // namespace rav::nmos
