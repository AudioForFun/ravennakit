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
#include "nmos_sender_transport_params_rtp.hpp"
#include "ravennakit/sdp/sdp.hpp"

namespace rav::nmos {

/**
 * Describes a sender.
 * https://specs.amwa.tv/is-04/releases/v1.3.3/APIs/schemas/with-refs/sender.html
 */
struct Sender: ResourceCore {
    /**
     * Object indicating how this Sender is currently configured to send data.
     */
    struct Subscription {
        /// UUID of the Receiver to which this Sender is currently configured to send data. Only set if it is active,
        /// uses a unicast push-based transport and is sending to an NMOS Receiver; otherwise null.
        std::optional<boost::uuids::uuid> receiver_id;

        /// Sender is enabled and configured to send data.
        bool active {};
    };

    /// ID of the Flow currently passing via this Sender. Set to null when a Flow is not currently internally routed to
    /// the Sender.
    std::optional<boost::uuids::uuid> flow_id;

    /// Transport type used by the Sender in URN format (example: urn:x-nmos:transport:)
    std::string transport;

    /// Device ID which this Sender forms part of. This attribute is used to ensure referential integrity by registry
    /// implementations.
    boost::uuids::uuid device_id;

    /// HTTP(S) accessible URL to a file describing how to connect to the Sender. Set to null when the transport type
    /// used by the Sender does not require a transport file.
    std::optional<std::string> manifest_href;

    /// Array of interface bindings used by the Sender. Each interface binding is a string formatted as a URN. The array
    /// may be empty.
    std::vector<std::string> interface_bindings;

    /// Object indicating how this Sender is currently configured to send data.
    Subscription subscription;

    /**
     * @return True if the sender is valid, loosely following the NMOS JSON schema, or false otherwise.
     */
    [[nodiscard]] bool is_valid() const {
        if (id.is_nil()) {
            return false;
        }
        if (device_id.is_nil()) {
            return false;
        }
        if (flow_id.has_value() && flow_id->is_nil()) {
            return false;
        }
        return true;
    }

    std::function<tl::expected<void, ApiError>(const boost::json::value& patch_request)> on_patch_request;
    std::function<tl::expected<sdp::SessionDescription, ApiError>()> get_transport_file;
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const Sender::Subscription& subscription) {
    jv = {
        {"receiver_id", boost::json::value_from(subscription.receiver_id)},
        {"active", subscription.active},
    };
}

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const Sender& sender) {
    tag_invoke(tag, jv, static_cast<const ResourceCore&>(sender));
    auto& jv_sender = jv.as_object();
    if (const auto id = sender.flow_id) {
        jv_sender["flow_id"] = boost::json::value_from(boost::uuids::to_string(*id));
    } else {
        jv_sender["flow_id"] = nullptr;
    }
    jv_sender["transport"] = sender.transport;
    jv_sender["device_id"] = boost::uuids::to_string(sender.device_id);
    jv_sender["manifest_href"] = boost::json::value_from(sender.manifest_href);
    jv_sender["interface_bindings"] = boost::json::value_from(sender.interface_bindings);
    jv_sender["subscription"] = boost::json::value_from(sender.subscription);
}

}  // namespace rav::nmos
