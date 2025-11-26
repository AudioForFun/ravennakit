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
#include "nmos_flow_core.hpp"

namespace rav::nmos {

struct FlowAudio: FlowCore {
    struct SampleRate {
        int numerator {};
        int denominator {1};
    };

    /// Format of the data coming from the Flow as a URN
    static constexpr auto k_format {"urn:x-nmos:format:audio"};

    /// Number of audio samples per second for this Flow
    SampleRate sample_rate {};
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const FlowAudio::SampleRate& sample_rate) {
    jv = {{"numerator", sample_rate.numerator}, {"denominator", sample_rate.denominator}};
}

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const FlowAudio& flow_audio) {
    tag_invoke(tag, jv, static_cast<const FlowCore&>(flow_audio));
    auto& object = jv.as_object();
    object["format"] = FlowAudio::k_format;
    object["sample_rate"] = boost::json::value_from(flow_audio.sample_rate);
}

}  // namespace rav::nmos
