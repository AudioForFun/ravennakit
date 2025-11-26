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

#include "ravennakit/core/audio/audio_format.hpp"

namespace rav::nmos {

inline const char* audio_encoding_to_nmos_media_type(const AudioEncoding encoding) {
    switch (encoding) {
        case AudioEncoding::undefined:
            return "audio/undefined";
        case AudioEncoding::pcm_s8:
            return "audio/L8";
        case AudioEncoding::pcm_u8:
            return "audio/U8";  // Non-standard
        case AudioEncoding::pcm_s16:
            return "audio/L16";
        case AudioEncoding::pcm_s24:
            return "audio/L24";
        case AudioEncoding::pcm_s32:
            return "audio/L32";
        case AudioEncoding::pcm_f32:
            return "audio/F32";
        case AudioEncoding::pcm_f64:
            return "audio/F64";
        default:
            return "audio/unknown";
    }
}

}  // namespace rav::nmos
