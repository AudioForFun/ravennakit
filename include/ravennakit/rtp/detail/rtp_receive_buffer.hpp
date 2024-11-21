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
#include "ravennakit/core/containers/circular_buffer.hpp"
#include "ravennakit/rtp/rtp_packet_view.hpp"

namespace rav {

/**
 * A buffer which operates on bytes, unaware of its contents. Can be used to account for jitter when receiving RTP data.
 */
class rtp_receive_buffer {
  public:
    explicit rtp_receive_buffer() = default;

    /**
     * Resizes the buffer.
     * @param delay The delay in frames.
     * @param bytes_per_frame The number of bytes per frame.
     */
    void resize(const size_t delay, const size_t bytes_per_frame) {
        delay_ = delay;
        bytes_per_frame_ = bytes_per_frame;
        buffer_.resize(delay * k_buffer_size_delay_factor * bytes_per_frame_);
    }

    /**
     * Writes data to the buffer.
     * @param at_timestamp Places the data at this timestamp.
     * @param payload The data to write to the buffer.
     * @return true if the data was written, false if the buffer is full or the timestamp is too old.
     */
    bool write(const uint32_t at_timestamp, const buffer_view<uint8_t>& payload) {
        if (payload.size_bytes() % bytes_per_frame_ != 0) {
            RAV_ERROR("Buffer size must be a multiple of bytes per frame.");
            return false;
        }

        if (payload.size_bytes() > buffer_.size()) {
            RAV_ERROR("Payload size is larger than the buffer size.");
            return false;
        }

        if (at_timestamp < most_recent_ts_ - delay_) {
            RAV_WARNING("Discarding old data with timestamp: {}", at_timestamp);
            // TODO: We discard a full packet of data here. We should consider a more fine-grained approach where we
            // take the data which is still in the window of (most_recent_ts_ - delay_) and most_recent_ts_.
            // It is tempting to test the timestamp against most_recent_ts_ - delay_ - payload.size_bytes() /
            // bytes_per_frame_, but that might result in overwriting newer data with older data if the buffer is full.
            return false;
        }

        position.update(at_timestamp * bytes_per_frame_, buffer_.size(), payload.size());

        std::memcpy(buffer_.data() + position.index1, payload.data(), position.size1);

        if (position.size2 > 0) {
            std::memcpy(buffer_.data(), payload.data() + position.size1, position.size2);
        }

        if (at_timestamp > most_recent_ts_) {
            num_frames_ += at_timestamp - most_recent_ts_;
            most_recent_ts_ = at_timestamp;
        }

        return true;
    }

    /**
     * Reads data from the buffer.
     * @param buffer The destination to write the data to.
     * @param buffer_size The size of the buffer in bytes.
     * @returns The number of bytes read, which might be less than buffer_size if not enough data is available.
     */
    size_t read(uint8_t* buffer, const size_t buffer_size) {
        RAV_ASSERT(buffer != nullptr, "Buffer must not be nullptr.");
        RAV_ASSERT(buffer_size > 0, "Buffer size must be greater than 0.");

        if (num_frames_ < delay_) {
            return 0;  // No data is available for reading.
        }

        if (buffer_size % bytes_per_frame_ != 0) {
            RAV_WARNING("Buffer size must be a multiple of bytes per frame.");
            return 0;
        }

        const auto num_frames_to_read = std::min(buffer_size / bytes_per_frame_, num_frames_ - delay_);
        const auto read_ts = most_recent_ts_ - num_frames_;
        position.update(read_ts * bytes_per_frame_, buffer_.size(), num_frames_to_read * bytes_per_frame_);

        std::memcpy(buffer, buffer_.data() + position.index1, position.size1);
        
        if (position.size2 > 0) {
            std::memcpy(buffer + position.size1, buffer_.data(), position.size2);
        }

        // TODO: Zero out the data in the buffer after reading it to allow gaps in the data.

        num_frames_ -= num_frames_to_read;
        return num_frames_to_read * bytes_per_frame_;
    }

    /**
     * @returns The number of frames available to read excluding the delay.
     */
    [[nodiscard]] size_t num_frames_available_to_read() const {
        return num_frames_ > delay_ ? num_frames_ - delay_ : 0;
    }

    /**
     * @returns The number of bytes available to read excluding the delay.
     */
    [[nodiscard]] size_t num_bytes_available_to_read() const {
        return num_frames_available_to_read() * bytes_per_frame_;
    }

  private:
    static constexpr size_t k_buffer_size_delay_factor = 2;  // The buffer size is twice the delay.

    size_t most_recent_ts_ = 0;    // Producer ts
    size_t delay_ = 0;             // Number of frames of delay
    size_t num_frames_ = 0;        // Number of frames in the buffer
    size_t bytes_per_frame_ = 0;   // Number of bytes (octets) per frame
    fifo::position position;       // Used for read and write operations
    std::vector<uint8_t> buffer_;  // Stored the actual data

    void resize_buffer(const size_t size) {
        RAV_ASSERT(bytes_per_frame_ > 0, "A delay of 0 will result in a buffer of 0 size");
        buffer_.resize(size * bytes_per_frame_);
    }
};

}  // namespace rav
