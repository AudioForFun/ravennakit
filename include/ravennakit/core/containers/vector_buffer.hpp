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
#include <vector>

#include "ravennakit/core/byte_order.hpp"
#include "ravennakit/core/assert.hpp"

namespace rav {

/**
 * Simple buffer around a vector that allows for easy reading and writing of values.
 */
template<class T>
class VectorBuffer {
  public:
    VectorBuffer() = default;

    explicit VectorBuffer(size_t size) : data_(size) {}

    VectorBuffer(std::initializer_list<T> values) : data_(values) {}

    VectorBuffer(const VectorBuffer& other) = default;
    VectorBuffer(VectorBuffer&& other) noexcept = default;
    VectorBuffer& operator=(const VectorBuffer& other) = default;
    VectorBuffer& operator=(VectorBuffer&& other) noexcept = default;

    bool operator==(const std::vector<T>& other) const {
        return data_ == other;
    }

    /**
     * Writes the given value to the buffer in native byte order.
     * @param value The value to write.
     */
    void push_back(T value) {
        data_.push_back(value);
    }

    /**
     * Writes the given values to the buffer in native byte order.
     * @param values The values to write.
     */
    void push_back(std::initializer_list<T> values) {
        for (const auto& value : values) {
            push_back(value);
        }
    }

    /**
     * Writes the given value to the buffer in big endian byte order.
     * @tparam T The type of the value to write.
     * @param value The value to write.
     */
    void push_back_be(T value) {
        push_back(swap_if_le(value));
    }

    /**
     * Writes the given value to the buffer in big endian byte order.
     * @tparam T The type of the value to write.
     * @param values The values to write.
     */
    void push_back_be(std::initializer_list<T> values) {
        for (const auto& value : values) {
            push_back_be(value);
        }
    }

    /**
     * Writes the given value to the buffer in little endian byte order.
     * @tparam T The type of the value to write.
     * @param value The value to write.
     * @return True if the value was written successfully, false otherwise.
     */
    void push_back_le(T value) {
        push_back(swap_if_be(value));
    }

    /**
     * Writes the given value to the buffer in little endian byte order.
     * @tparam T The type of the value to write.
     * @param values The values to write.
     */
    void push_back_le(std::initializer_list<T> values) {
        for (const auto& value : values) {
            push_back_le(value);
        }
    }

    /**
     * Reads a value from the buffer in native byte order.
     * @tparam T The type of the value to read.
     * @return The value read from the stream, or a default-constructed value if the read failed.
     */
    T read() {
        if (read_position_ >= data_.size()) {
            return T {};
        }
        return data_[read_position_++];
    }

    /**
     * Reads a value from the buffer in big endian byte order.
     * @tparam T The type of the value to read.
     * @return The value read from the stream, or a default-constructed value if the read failed.
     */
    T read_be() {
        return swap_if_le(read());
    }

    /**
     * Reads a value from the buffer in little endian byte order.
     * @tparam T The type of the value to read.
     * @return The value read from the stream, or a default-constructed value if the read failed.
     */
    T read_le() {
        return swap_if_be(read());
    }

    /**
     * @return Returns a pointer to the data in the buffer.
     */
    [[nodiscard]] const T* data() const {
        return data_.data();
    }

    /**
     * @return Returns a pointer to the data in the buffer.
     */
    [[nodiscard]] T* data() {
        return data_.data();
    }

    /**
     * @return Returns the size of the data in the buffer.
     */
    [[nodiscard]] size_t size() const {
        RAV_ASSERT(
            read_position_ <= data_.size(), "Read position ought to be less than or equal to the size of the data"
        );
        return data_.size() - read_position_;
    }

    /**
     * @returns True if the stream is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const {
        return read_position_ >= data_.size();
    }

    /**
     * Resizes the stream to the given size.
     * @param size The new size of the stream.
     */
    void resize(size_t size) {
        data_.resize(size);
        if (read_position_ > size) {
            read_position_ = size;
        }
    }

    /**
     * Resets the stream to its initial state.
     */
    void reset() {
        read_position_ = 0;
        data_.clear();
    }

  private:
    std::vector<T> data_;
    size_t read_position_ = 0;
};

}  // namespace rav
