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

#include <boost/lexical_cast.hpp>
#include <boost/uuid.hpp>
#include <boost/json/conversion.hpp>
#include <boost/json/value_from.hpp>

namespace rav::nmos {

struct Device: ResourceCore {
    static constexpr auto k_type_generic = "urn:x-nmos:device:generic";
    static constexpr auto k_type_pipeline = "urn:x-nmos:device:pipeline";

    struct Control {
        /// URL to reach a control endpoint, whether http or otherwise
        std::string href;

        /// URN identifying the control format
        std::string type;

        /// Whether this endpoint requires authorization, not required
        std::optional<bool> authorization;
    };

    /// Device type URN (urn:x-nmos:device:<type>)
    std::string type {k_type_generic};

    /// Globally unique identifier for the Node which initially created the Device. This attribute is used to ensure
    /// referential integrity by registry implementations.
    boost::uuids::uuid node_id;

    /// Control endpoints exposed for the Device
    std::vector<Control> controls;

    /// UUIDs of Receivers attached to the Device (deprecated)
    std::vector<boost::uuids::uuid> receivers;

    /// UUIDs of Senders attached to the Device (deprecated)
    std::vector<boost::uuids::uuid> senders;
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const Device::Control& control) {
    jv = {{"href", control.href}, {"type", control.type}};
    if (control.authorization) {
        jv.as_object()["authorization"] = *control.authorization;
    }
}

inline Device::Control tag_invoke(const boost::json::value_to_tag<Device::Control>&, const boost::json::value& jv) {
    Device::Control control;
    control.href = jv.at("href").as_string();
    control.type = jv.at("type").as_string();
    if (const auto auth = jv.try_at("authorization"); auth && auth->is_bool()) {
        control.authorization = auth->get_bool();
    }
    return control;
}

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const Device& device) {
    tag_invoke(tag, jv, static_cast<const ResourceCore&>(device));
    auto& object = jv.as_object();
    object["type"] = device.type;
    object["node_id"] = to_string(device.node_id);
    object["controls"] = boost::json::value_from(device.controls);

    boost::json::array receivers;
    for (const auto& receiver : device.receivers) {
        receivers.push_back(boost::json::value(boost::lexical_cast<std::string>(receiver)));
    }
    object["receivers"] = receivers;

    boost::json::array senders;
    for (const auto& sender : device.senders) {
        senders.push_back(boost::json::value(boost::lexical_cast<std::string>(sender)));
    }
    object["senders"] = senders;
}

inline Device tag_invoke(const boost::json::value_to_tag<Device>&, const boost::json::value& jv) {
    Device device;
    device.id = boost::lexical_cast<boost::uuids::uuid>(std::string_view(jv.at("id").as_string()));
    device.version = Version::from_string(jv.at("version").as_string()).value();
    device.label = jv.at("label").as_string();
    device.description = jv.at("description").as_string();
    device.tags = boost::json::value_to<std::map<std::string, std::vector<std::string>>>(jv.at("tags"));
    device.type = jv.at("type").as_string();
    device.node_id = boost::lexical_cast<boost::uuids::uuid>(std::string_view(jv.at("node_id").as_string()));
    device.controls = boost::json::value_to<std::vector<Device::Control>>(jv.at("controls"));

    for (const auto& receiver : jv.at("receivers").as_array()) {
        device.receivers.push_back(boost::lexical_cast<boost::uuids::uuid>(std::string_view(receiver.as_string())));
    }

    for (const auto& sender : jv.at("senders").as_array()) {
        device.senders.push_back(boost::lexical_cast<boost::uuids::uuid>(std::string_view(sender.as_string())));
    }

    return device;
}

}  // namespace rav::nmos
