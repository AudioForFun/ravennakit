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
class BufferView {
  public:
    BufferView() = default;

    BufferView(const BufferView& other) = default;
    BufferView& operator=(const BufferView& other) = default;

    BufferView(BufferView&& other) noexcept = default;
    BufferView& operator=(BufferView&& other) noexcept = default;

    /**
     * Constructs a view pointing to given data.
     * @param data The data to refer to.
     * @param count The number of elements in the buffer.
     */
    BufferView(Type* data, const size_t count) : data_(data), count_(count) {
        if (data_ == nullptr) {
            count_ = 0;
        }
    }

    /**
     * @returns A pointer to the data, or nullptr if this view is not pointing at any data.
     */
    [[nodiscard]] const Type* data() const {
        return data_;
    }

    /**
     * @returns The number of elements in the buffer.
     */
    [[nodiscard]] size_t size() const {
        return count_;
    }

    /**
     * @returns The size of the buffer in bytes.
     */
    [[nodiscard]] size_t size_bytes() const {
        return count_ * sizeof(Type);
    }

    /**
     * @returns True if the buffer is valid, or false if the view doesn't point at any data. A buffer of size 0 is
     * considered valid.
     */
    [[nodiscard]] bool is_valid() const {
        return data_ != nullptr;
    }

    /**
     * @returns True if the buffer is empty.
     */
    [[nodiscard]] bool empty() const {
        return count_ == 0;
    }

  private:
    Type* data_ {nullptr};
    size_t count_ {0};
};

}  // namespace rsdk
