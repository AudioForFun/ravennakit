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
