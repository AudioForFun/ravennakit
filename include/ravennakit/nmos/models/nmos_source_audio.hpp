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
#include "nmos_source_core.hpp"

namespace rav::nmos {

/**
 * Describes an audio source.
 * https://specs.amwa.tv/is-04/releases/v1.3.3/APIs/schemas/with-refs/source_audio.html
 */
struct SourceAudio: SourceCore {
    static constexpr auto k_format = "urn:x-nmos:format:audio";

    struct Channel {
        /// Label for this channel (free text).
        std::string label;
    };

    /// Array of objects describing the audio channels
    std::vector<Channel> channels;

    /**
     * @return True if the receiver is valid, loosely following the NMOS JSON schema, or false otherwise.
     */
    bool is_valid() const {
        if (id.is_nil()) {
            return false;
        }
        if (channels.empty()) {
            return false;
        }
        return true;
    }
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const SourceAudio::Channel& channel) {
    jv = {
        {"label", boost::json::value_from(channel.label)},
    };
}

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const SourceAudio& source) {
    tag_invoke(tag, jv, static_cast<const SourceCore&>(source));
    auto& obj = jv.as_object();
    obj["format"] = SourceAudio::k_format;
    obj["channels"] = boost::json::value_from(source.channels);
}

}  // namespace rav::nmos
