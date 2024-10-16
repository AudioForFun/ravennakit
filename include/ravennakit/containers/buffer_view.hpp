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

#include <cstddef>

namespace rav {

/**
 * A class similar to std::string_view but for raw data buffers.
 * @tparam Type The data type.
 */
template<class Type>
class buffer_view {
  public:
    buffer_view() = default;

    buffer_view(const buffer_view& other) = default;
    buffer_view& operator=(const buffer_view& other) = default;

    buffer_view(buffer_view&& other) noexcept = default;
    buffer_view& operator=(buffer_view&& other) noexcept = default;

    /**
     * Constructs a view pointing to given data.
     * @param data The data to refer to.
     * @param count The number of elements in the buffer.
     */
    buffer_view(Type* data, const size_t count) : data_(data), size_(count) {
        if (data_ == nullptr) {
            size_ = 0;
        }
    }

    /**
     * @param index The index to access.
     * @returns Value for given index, without bounds checking.
     */
    Type operator[](size_t index) const {
        return data_[index];
    }

    /**
     * @returns A pointer to the data, or nullptr if this view is not pointing at any data.
     */
    [[nodiscard]] const Type* data() const {
        return data_;
    }

    /**
     * @returns A pointer to the data, or nullptr if this view is not pointing at any data.
     */
    [[nodiscard]] Type* data() {
        return data_;
    }

    /**
     * @returns The number of elements in the buffer.
     */
    [[nodiscard]] size_t size() const {
        return size_;
    }

    /**
     * @returns The size of the buffer in bytes.
     */
    [[nodiscard]] size_t size_bytes() const {
        return size_ * sizeof(Type);
    }

    /**
     * @returns True if the buffer is empty.
     */
    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    /**
     * Returns a new buffer_view pointing to the same data, but reinterpreted as a different type.
     * WARNING! Reinterpreting data can potentially lead to undefined behavior. Rules for reinterpret_cast apply.
     * @tparam NewType The type of the reinterpretation.
     * @return The new buffer_view.
     */
    template<class NewType>
    buffer_view<NewType> reinterpret() const {
        return buffer_view<NewType>(reinterpret_cast<NewType*>(data_), size_ * sizeof(Type) / sizeof(NewType));
    }

  private:
    Type* data_ {nullptr};
    size_t size_ {0};
};

}  // namespace rav
