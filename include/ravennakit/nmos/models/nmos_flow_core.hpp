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
