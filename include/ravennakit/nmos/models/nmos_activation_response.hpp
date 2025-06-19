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
#include "ravennakit/nmos/detail/nmos_timestamp.hpp"

namespace rav::nmos {

/**
 * Parameters concerned with activation of the transport parameters.
 * https://specs.amwa.tv/is-05/releases/v1.1.2/APIs/schemas/with-refs/activation-response-schema.html
 */
struct ActivationResponse {
    enum class Mode { activate_immediate, activate_scheduled_absolute, activate_scheduled_relative };

    /**
     * Mode of activation: immediate (on message receipt), scheduled_absolute (when internal clock >= requested_time),
     * scheduled_relative (when internal clock >= time of message receipt + requested_time), or null (no activation
     * scheduled). This parameter returns to null on the staged endpoint once an activation is completed or when it is
     * explicitly set to null. For immediate activations, in the response to the PATCH request this field will be set to
     * 'activate_immediate', but will be null in response to any subsequent GET requests.
     */
    std::optional<Mode> mode;

    /**
     * String formatted TAI timestamp (<seconds>:<nanoseconds>) indicating time (absolute or relative) for activation
     * requested. This field returns to null once the activation is completed on the staged endpoint or when the
     * resource is unlocked by setting the activation mode to null. For an immediate activation this field will always
     * be null on the staged endpoint, even in the response to the PATCH request.
     */
    std::optional<Timestamp> requested_time;

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

inline const char* to_string(const ActivationResponse::Mode mode) {
    switch (mode) {
        case ActivationResponse::Mode::activate_immediate:
            return "activate_immediate";
        case ActivationResponse::Mode::activate_scheduled_absolute:
            return "activate_scheduled_absolute";
        case ActivationResponse::Mode::activate_scheduled_relative:
            return "activate_scheduled_relative";
    }
    return "";
}

inline void
tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const ActivationResponse::Mode& mode) {
    jv = to_string(mode);
}

inline void
tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const ActivationResponse& activation_response) {
    jv = {
        {"mode", boost::json::value_from(activation_response.mode)},
        {"requested_time", boost::json::value_from(activation_response.requested_time)},
        {"activation_time", boost::json::value_from(activation_response.activation_time)},
    };
}

}  // namespace rav::nmos
