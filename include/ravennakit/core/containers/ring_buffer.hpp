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

#include "ravennakit/core/assert.hpp"

#include <vector>
#include <optional>
#include <tuple>

namespace rav {

/**
 * A fixed size buffer that overwrites the oldest element when the buffer is full.
 * TODO: Replace with boost::circular_buffer.
 * @tparam T The type of the elements stored in the buffer.
 */
template<class T>
class RingBuffer {
  public:
    explicit RingBuffer(size_t size) : data_(size) {
        RAV_ASSERT(size > 0, "Ring buffer must have a size greater than zero");
    }

    RingBuffer(std::initializer_list<T> initializer_list) : data_(initializer_list), count_(initializer_list.size()) {}

    RingBuffer(const RingBuffer& other) = default;
    RingBuffer& operator=(const RingBuffer& other) = default;

    RingBuffer(RingBuffer&& other) noexcept {
        if (this != &other) {
            std::swap(data_, other.data_);
            std::swap(read_index_, other.read_index_);
            std::swap(write_index_, other.write_index_);
            std::swap(count_, other.count_);
        }
    }

    RingBuffer& operator=(RingBuffer&& other) noexcept {
        if (this != &other) {
            std::swap(data_, other.data_);
            std::swap(read_index_, other.read_index_);
            std::swap(write_index_, other.write_index_);
            std::swap(count_, other.count_);
            other.clear();
        }
        return *this;
    }

    /**
     * @return The oldest element in the buffer. No bounds checking is performed - be warned!
     */
    [[nodiscard]] T& front() {
        RAV_ASSERT(!empty(), "Cannot access front of empty ring buffer");
        return data_[read_index_];
    }

    /**
     * @return The newest element in the buffer. No bounds checking is performed - be warned!
     */
    [[nodiscard]] T& back() {
        RAV_ASSERT(!empty(), "Cannot access back of empty ring buffer");
        return data_[(write_index_ + data_.size() - 1) % data_.size()];
    }

    /**
     * Add an element to the buffer. If the buffer is full, the oldest element will be overwritten.
     * @param value The value to add to the buffer.
     * @returns True if the buffer was full and the oldest element was overwritten, false otherwise.
     */
    bool push_back(const T& value) {
        const bool overwritten = full();
        data_[write_index_] = value;
        write_index_ = (write_index_ + 1) % data_.size();
        if (count_ < data_.size()) {
            ++count_;
        } else {
            read_index_ = (read_index_ + 1) % data_.size();
        }
        return overwritten;
    }

    /**
     * @return The oldest element in the buffer, or std::nullopt if the buffer is empty.
     */
    [[nodiscard]] std::optional<T> pop_front() {
        if (empty()) {
            return std::nullopt;
        }
        T value = std::move(data_[read_index_]);
        read_index_ = (read_index_ + 1) % data_.size();
        --count_;
        return value;
    }

    /**
     * Indexing operator. Buffer must not have zero capacity.
     * @param index The logical index of the element to access. The index will wrap around if too high.
     * @return The element at the given index.
     */
    [[nodiscard]] T& operator[](const size_t index) {
        return data_[(read_index_ + index) % data_.size()];
    }

    /**
     * Indexing operator. Buffer must not have zero capacity.
     * @param index The logical index of the element to access. The index will wrap around if too high.
     * @return The element at the given index.
     */
    [[nodiscard]] const T& operator[](const size_t index) const {
        return data_[(read_index_ + index) % data_.size()];
    }

    /**
     * Get the size of the buffer. This is the number of elements currently stored in the buffer.
     * @return The size of the buffer.
     */
    [[nodiscard]] size_t size() const {
        return count_;
    }

    /**
     * Get the capacity of the buffer. This is the maximum number of elements that can be stored in the buffer.
     * @return The capacity of the buffer.
     */
    [[nodiscard]] size_t capacity() const {
        return data_.size();
    }

    /**
     * @returns True if the buffer is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const {
        return count_ == 0;
    }

    /**
     * @returns True if the buffer is full, false otherwise.
     */
    [[nodiscard]] bool full() const {
        return count_ == data_.size();
    }

    /**
     * Sets the counters to zero, effectively clearing the buffer. The capacity remains the same.
     */
    void clear() {
        read_index_ = 0;
        write_index_ = 0;
        count_ = 0;
    }

    /**
     * Resets the buffer, discarding existing contents.
     * @param new_capacity The new capacity of the buffer. If not provided, the capacity remains the same.
     */
    void reset(std::optional<size_t> new_capacity = std::nullopt) {
        if (new_capacity.has_value()) {
            data_.resize(*new_capacity);
        }
        clear();
    }

    [[nodiscard]] auto tie() {
        return std::tie(data_, read_index_, write_index_, count_);
    }

    friend bool operator==(const RingBuffer& lhs, const RingBuffer& rhs) {
        return lhs.tie() == rhs.tie();
    }

    friend bool operator!=(const RingBuffer& lhs, const RingBuffer& rhs) {
        return lhs.tie() != rhs.tie();
    }

    /**
     * An iterator for the ring buffer.
     */
    template<typename BufferType, typename ValueType>
    class IteratorBase {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        IteratorBase(BufferType& buffer, const size_t logical_index, const size_t remaining) :
            buffer_(buffer), logical_index_(logical_index), remaining_(remaining) {}

        reference operator*() {
            size_t physical_index = (buffer_.read_index_ + logical_index_) % buffer_.data_.size();
            return buffer_.data_[physical_index];
        }

        pointer operator->() {
            return &(**this);
        }

        IteratorBase& operator++() {
            ++logical_index_;
            --remaining_;
            return *this;
        }

        IteratorBase operator++(int) {
            IteratorBase temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const IteratorBase& other) const {
            return remaining_ == other.remaining_;
        }

        bool operator!=(const IteratorBase& other) const {
            return remaining_ != other.remaining_;
        }

      private:
        BufferType& buffer_;
        size_t logical_index_;
        size_t remaining_;
    };

    using Iterator = IteratorBase<RingBuffer, T>;
    using ConstIterator = IteratorBase<const RingBuffer, const T>;

    Iterator begin() {
        return Iterator(*this, 0, count_);
    }

    Iterator end() {
        return Iterator(*this, count_, 0);
    }

    ConstIterator begin() const {
        return ConstIterator(*this, 0, count_);
    }

    ConstIterator end() const {
        return ConstIterator(*this, count_, 0);
    }

    ConstIterator cbegin() const {
        return ConstIterator(*this, 0, count_);
    }

    ConstIterator cend() const {
        return ConstIterator(*this, count_, 0);
    }

  private:
    std::vector<T> data_;
    size_t read_index_ = 0;
    size_t write_index_ = 0;
    size_t count_ = 0;
};

}  // namespace rav
