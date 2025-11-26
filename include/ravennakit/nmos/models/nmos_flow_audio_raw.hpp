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
#include "nmos_flow_audio.hpp"

namespace rav::nmos {

struct FlowAudioRaw: FlowAudio {
    /// Subclassification of the format, using IANA assigned media types where available, or other values defined in the
    /// NMOS Parameter Registers.
    std::string media_type {};  // audio/L24, audio/L20, audio/L16, audio/L8

    /// Bit depth of the audio samples.
    int bit_depth {};  // 8, 16, 20, 24

    /**
     * @return True if the flow is valid, loosely following the NMOS JSON schema, or false otherwise.
     */
    [[nodiscard]] bool is_valid() const {
        if (id.is_nil()) {
            return false;
        }
        if (media_type.empty()) {
            return false;
        }
        if (bit_depth <= 0) {
            return false;
        }
        if (sample_rate.numerator <= 0 || sample_rate.denominator <= 0) {
            return false;
        }
        return true;
    }
};

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const FlowAudioRaw& flow_audio_raw) {
    tag_invoke(tag, jv, static_cast<const FlowAudio&>(flow_audio_raw));
    auto& object = jv.as_object();
    object["media_type"] = flow_audio_raw.media_type;
    object["bit_depth"] = flow_audio_raw.bit_depth;
}

}  // namespace rav::nmos
