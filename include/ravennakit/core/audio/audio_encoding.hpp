/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <optional>
#include <cstring>

namespace rav {

enum class AudioEncoding : uint8_t {
    undefined,
    pcm_s8,
    pcm_u8,
    pcm_s16,
    pcm_s24,
    pcm_s32,
    pcm_f32,
    pcm_f64,
};

/**
 * @return The number of bytes per sample.
 */
inline uint8_t audio_encoding_bytes_per_sample(const AudioEncoding encoding) {
    switch (encoding) {
        case AudioEncoding::pcm_s8:
        case AudioEncoding::pcm_u8:
            return 1;
        case AudioEncoding::pcm_s16:
            return 2;
        case AudioEncoding::pcm_s24:
            return 3;
        case AudioEncoding::pcm_s32:
        case AudioEncoding::pcm_f32:
            return 4;
        case AudioEncoding::pcm_f64:
            return 8;
        case AudioEncoding::undefined:
        default:
            return 0;
    }
}

/**
 * @return The ground value for the encoding.
 */
inline uint8_t audio_encoding_ground_value(const AudioEncoding encoding) {
    switch (encoding) {
        case AudioEncoding::pcm_u8:
            return 0x80;
        case AudioEncoding::pcm_s8:
        case AudioEncoding::pcm_s16:
        case AudioEncoding::pcm_s24:
        case AudioEncoding::pcm_s32:
        case AudioEncoding::pcm_f32:
        case AudioEncoding::pcm_f64:
        case AudioEncoding::undefined:
        default:
            return 0;
    }
}

inline const char* audio_encoding_to_string(const AudioEncoding encoding) {
    switch (encoding) {
        case AudioEncoding::undefined:
            return "undefined";
        case AudioEncoding::pcm_s8:
            return "pcm_s8";
        case AudioEncoding::pcm_u8:
            return "pcm_u8";
        case AudioEncoding::pcm_s16:
            return "pcm_s16";
        case AudioEncoding::pcm_s24:
            return "pcm_s24";
        case AudioEncoding::pcm_s32:
            return "pcm_s32";
        case AudioEncoding::pcm_f32:
            return "pcm_f32";
        case AudioEncoding::pcm_f64:
            return "pcm_f64";
        default:
            return "unknown";
    }
}

inline std::optional<AudioEncoding> audio_encoding_from_string(const char* str) {
    if (!str) {
        return std::nullopt;
    }
    if (std::strcmp(str, "pcm_s8") == 0) {
        return AudioEncoding::pcm_s8;
    }
    if (std::strcmp(str, "pcm_u8") == 0) {
        return AudioEncoding::pcm_u8;
    }
    if (std::strcmp(str, "pcm_s16") == 0) {
        return AudioEncoding::pcm_s16;
    }
    if (std::strcmp(str, "pcm_s24") == 0) {
        return AudioEncoding::pcm_s24;
    }
    if (std::strcmp(str, "pcm_s32") == 0) {
        return AudioEncoding::pcm_s32;
    }
    if (std::strcmp(str, "pcm_f32") == 0) {
        return AudioEncoding::pcm_f32;
    }
    if (std::strcmp(str, "pcm_f64") == 0) {
        return AudioEncoding::pcm_f64;
    }
    if (std::strcmp(str, "undefined") == 0) {
        return AudioEncoding::undefined;
    }
    return std::nullopt;
}

}  // namespace rav
