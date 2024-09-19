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
#include <cstring>

#include "ravennakit/platform/byte_order.hpp"

namespace rav {

/**
 * Simple stream implementation that writes to and reads from a vector.
 */
class vector_stream {
  public:
    vector_stream() = default;

    vector_stream(const vector_stream& other) = default;
    vector_stream(vector_stream&& other) noexcept = default;
    vector_stream& operator=(const vector_stream& other) = default;
    vector_stream& operator=(vector_stream&& other) noexcept = default;

    /**
     * Writes the given value to the stream in native byte order.
     * @tparam T The type of the value to write.
     * @param value The value to write.
     * @return True if the value was written successfully, false otherwise.
     */
    template<class T>
    bool write(T value) {
        add_capacity(sizeof(T));
        std::memcpy(data_.data() + write_position_, &value, sizeof(T));
        write_position_ += sizeof(T);
        return true;
    }

    /**
     * Writes the given value to the stream in big endian byte order.
     * @tparam T The type of the value to write.
     * @param value The value to write.
     * @return True if the value was written successfully, false otherwise.
     */
    template<class T>
    bool write_be(T value) {
        if constexpr (little_endian) {
            return write(byte_order::swap_bytes(value));
        }
        return write(value);
    }

    /**
     * Writes the given value to the stream in little endian byte order.
     * @tparam T The type of the value to write.
     * @param value The value to write.
     * @return True if the value was written successfully, false otherwise.
     */
    template<class T>
    bool write_le(T value) {
        if constexpr (big_endian) {
            return write(byte_order::swap_bytes(value));
        }
        return write(value);
    }

    /**
     * Reads a value from the stream in native byte order.
     * @tparam T The type of the value to read.
     * @return The value read from the stream, or a default-constructed value if the read failed.
     */
    template<class T>
    T read() {
        if (read_position_ + sizeof(T) > write_position_) {
            return T {};
        }
        T value;
        std::memcpy(&value, data_.data() + read_position_, sizeof(T));
        read_position_ += sizeof(T);
        return value;
    }

    /**
     * Reads a value from the stream in big endian byte order.
     * @tparam T The type of the value to read.
     * @return The value read from the stream, or a default-constructed value if the read failed.
     */
    template<class T>
    T read_be() {
        if constexpr (little_endian) {
            return byte_order::swap_bytes(read<T>());
        }
        return read<T>();
    }

    /**
     * Reads a value from the stream in little endian byte order.
     * @tparam T The type of the value to read.
     * @return The value read from the stream, or a default-constructed value if the read failed.
     */
    template<class T>
    T read_le() {
        if constexpr (big_endian) {
            return byte_order::swap_bytes(read<T>());
        }
        return read<T>();
    }

    /**
     * @return Returns a pointer to the data in the stream.
     */
    [[nodiscard]] const uint8_t* data() const {
        return data_.data();
    }

    /**
     * @return Returns the size of the data in the stream.
     */
    [[nodiscard]] size_t size() const {
        return std::min(data_.size(), read_position_);
    }

    /**
     * Resets the stream to its initial state.
     */
    void reset() {
        read_position_ = 0;
        write_position_ = 0;
        data_.clear();
    }

  private:
    std::vector<uint8_t> data_;
    size_t read_position_ = 0;
    size_t write_position_ = 0;

    void add_capacity(const size_t size) {
        data_.resize(data_.size() + size);
    }
};

}  // namespace rav
