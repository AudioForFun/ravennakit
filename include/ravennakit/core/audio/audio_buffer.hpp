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

#include "audio_buffer_view.hpp"

#include <vector>
#include <cstring>

#include "ravennakit/core/assert.hpp"

namespace rav {

/**
 * A dynamically sizeable buffer to store non-interleaved audio, along with an array of pointers to the beginning of each channel.
 * @tparam T The sample type.
 */
template<class T>
class AudioBuffer: public AudioBufferView<T> {
  public:
    AudioBuffer() : AudioBufferView<T>(nullptr, 0, 0) {}

    /**
     * Constructs an audio buffer with the given number of channels and frames.
     * @param num_channels The number of channels.
     * @param num_frames The number of frames.
     */
    AudioBuffer(const size_t num_channels, const size_t num_frames) : AudioBufferView<T>(nullptr, 0, 0) {
        resize(num_channels, num_frames);
    }

    /**
     * Constructs an audio buffer with the given number of channels and frames and fills it with given value.
     * @param num_channels The number of channels.
     * @param num_frames The number of frames.
     * @param value_to_fill_with The value to fill the buffer with.
     */
    AudioBuffer(const size_t num_channels, const size_t num_frames, T value_to_fill_with) : AudioBufferView<T>(nullptr, 0, 0) {
        resize(num_channels, num_frames);
        std::fill(data_.begin(), data_.end(), value_to_fill_with);
    }

    /**
     * Constructs an audio buffer by copying from another buffer.
     * @param other The other buffer to copy from.
     */
    AudioBuffer(const AudioBuffer& other) : AudioBufferView<T>(nullptr, 0, 0) {
        data_ = other.data_;
        channels_.resize(other.channels_.size());
        update_channel_pointers();
    }

    /**
     * Constructs an audio buffer by moving from another buffer.
     * @param other The other buffer to move from.
     */
    AudioBuffer(AudioBuffer&& other) noexcept : AudioBufferView<T>(nullptr, 0, 0) {
        std::swap(data_, other.data_);
        std::swap(channels_, other.channels_);
        update_channel_pointers();
        other.update_channel_pointers();
    }

    /**
     * Copies the contents from another audio buffer to this buffer.
     * @param other The other buffer to copy from.
     * @return A reference to this buffer.
     */
    AudioBuffer& operator=(const AudioBuffer& other) {
        data_ = other.data_;
        channels_.resize(other.channels_.size());
        update_channel_pointers();
        return *this;
    }

    /**
     * Moves the contents from another audio buffer to this buffer. It does this by swapping, so other will have the
     * contents of this buffer.
     * @param other The other buffer to move from.
     * @return A reference to this buffer.
     */
    AudioBuffer& operator=(AudioBuffer&& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(channels_, other.channels_);
        update_channel_pointers();
        other.update_channel_pointers();  // Data is swapped, so we need to update the pointers of the other buffer.
        return *this;
    }

    /**
     * Prepares the audio buffer for the given number of channels and frames. New space will be zero initialized.
     * Existing data will be kept, except if the number of channels or frames is less than the current number of
     * channels or frames.
     * @param num_channels The number of channels.
     * @param num_frames The number of frames.
     */
    void resize(size_t num_channels, const size_t num_frames) {
        if (num_channels == 0 || num_frames == 0) {
            data_.clear();
            channels_.clear();
            return;
        }

        data_.resize(num_channels * num_frames, {});
        channels_.resize(num_channels);

        update_channel_pointers();
    }

  private:
    /// Holds the non-interleaved audio data (each channel consecutive).
    std::vector<T> data_;

    /// Holds pointers to the beginning of each channel.
    std::vector<T*> channels_;

    void update_channel_pointers() {
        for (size_t i = 0; i < channels_.size(); ++i) {
            channels_[i] = data_.data() + i * data_.size() / channels_.size();
        }
        const auto num_frames = channels_.empty() ? 0 : data_.size() / channels_.size();
        AudioBufferView<T>::update(channels_.data(), channels_.size(), num_frames);
    }
};

}  // namespace rav
