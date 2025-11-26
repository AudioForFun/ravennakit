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

#include "nmos_activation.hpp"
#include "ravennakit/core/json.hpp"
#include "ravennakit/nmos/detail/nmos_timestamp.hpp"

namespace rav::nmos {

/**
 * Parameters concerned with activation of the transport parameters.
 * https://specs.amwa.tv/is-05/releases/v1.1.2/APIs/schemas/with-refs/activation-response-schema.html
 */
struct ActivationResponse: Activation {
    /**
     * String formatted TAI timestamp (<seconds>:<nanoseconds>) indicating the absolute time the sender or receiver will
     * or did actually activate for scheduled activations, or the time activation occurred for immediate activations. On
     * the staged endpoint this field returns to null once the activation is completed or when the resource is unlocked
     * by setting the activation mode to null. For immediate activations on the staged endpoint this property will be
     * the time the activation actually occurred in the response to the PATCH request, but null in response to any GET
     * requests thereafter.
     */
    std::optional<Timestamp> activation_time;
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const ActivationResponse& activation_response) {
    jv = {
        {"mode", boost::json::value_from(activation_response.mode)},
        {"requested_time", boost::json::value_from(activation_response.requested_time)},
        {"activation_time", boost::json::value_from(activation_response.activation_time)},
    };
}

}  // namespace rav::nmos
