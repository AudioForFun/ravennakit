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

#include "ravennakit/core/assert.hpp"
#include "ravennakit/core/containers/detail/fifo.hpp"

rav::Fifo::Position::Position(const size_t timestamp, const size_t capacity, const size_t number_of_elements) {
    update(timestamp, capacity, number_of_elements);
}

void rav::Fifo::Position::update(const size_t timestamp, const size_t capacity, const size_t number_of_elements) {
    RAV_ASSERT(number_of_elements <= capacity, "Number of elements must be less than or equal to capacity.");
    index1 = timestamp % capacity;
    size1 = number_of_elements;
    size2 = 0;

    if (index1 + number_of_elements > capacity) {
        size1 = capacity - index1;
        size2 = number_of_elements - size1;
    }
}

size_t rav::Fifo::Spmc::size() const {
    return write_ts_.load() - read_ts_.load();
}

rav::Fifo::Single::Lock rav::Fifo::Single::prepare_for_write(const size_t number_of_elements) {
    if (write_ts_ - read_ts_ + number_of_elements > capacity_) {
        return {};
    }

    Lock write_lock([this, number_of_elements] {
        write_ts_ += number_of_elements;
    });
    write_lock.position.update(write_ts_, capacity_, number_of_elements);
    return write_lock;
}

rav::Fifo::Single::Lock rav::Fifo::Single::prepare_for_read(const size_t number_of_elements) {
    if (write_ts_ - read_ts_ < number_of_elements) {
        return {};
    }

    Lock read_lock([this, number_of_elements] {
        read_ts_ += number_of_elements;
    });
    read_lock.position.update(read_ts_, capacity_, number_of_elements);
    return read_lock;
}

size_t rav::Fifo::Single::size() const {
    return write_ts_ - read_ts_;
}

void rav::Fifo::Single::resize(const size_t capacity) {
    reset();
    capacity_ = capacity;
}

void rav::Fifo::Single::reset() {
    read_ts_ = 0;
    write_ts_ = 0;
}

rav::Fifo::Spsc::Lock rav::Fifo::Spsc::prepare_for_write(const size_t number_of_elements) {
    if (write_ts_.load() - read_ts_.load() + number_of_elements > capacity_) {
        return {};  // Not enough free space in buffer.
    }

    Lock write_lock([this, number_of_elements] {
        write_ts_.fetch_add(number_of_elements);
    });
    write_lock.position.update(write_ts_, capacity_, number_of_elements);
    return write_lock;
}

rav::Fifo::Spsc::Lock rav::Fifo::Spsc::prepare_for_read(const size_t number_of_elements) {
    if (write_ts_.load() - read_ts_.load() < number_of_elements) {
        return {};  // Not enough data available.
    }

    Lock read_lock([this, number_of_elements] {
        read_ts_.fetch_add(number_of_elements);
    });
    read_lock.position.update(read_ts_, capacity_, number_of_elements);
    return read_lock;
}

size_t rav::Fifo::Spsc::size() const {
    return write_ts_.load() - read_ts_.load();
}

void rav::Fifo::Spsc::resize(const size_t capacity) {
    reset();
    capacity_ = capacity;
}

void rav::Fifo::Spsc::reset() {
    read_ts_ = 0;
    write_ts_ = 0;
}

rav::Fifo::Mpsc::Lock rav::Fifo::Mpsc::prepare_for_write(const size_t number_of_elements) {
    std::unique_lock guard(mutex_);

    if (write_ts_.load() - read_ts_.load() + number_of_elements > capacity_) {
        return {};  // Not enough free space in buffer.
    }

    Lock write_lock(
        [this, number_of_elements] {
            write_ts_.fetch_add(number_of_elements);
        },
        std::move(guard)
    );
    write_lock.position.update(write_ts_, capacity_, number_of_elements);
    return write_lock;
}

rav::Fifo::Mpsc::Lock rav::Fifo::Mpsc::prepare_for_read(const size_t number_of_elements) {
    if (write_ts_.load() - read_ts_.load() < number_of_elements) {
        return {};  // Not enough data available.
    }
    Lock read_lock([this, number_of_elements] {
        read_ts_.fetch_add(number_of_elements);
    });
    read_lock.position.update(read_ts_, capacity_, number_of_elements);
    return read_lock;
}

size_t rav::Fifo::Mpsc::size() const {
    return write_ts_.load() - read_ts_.load();
}

void rav::Fifo::Mpsc::resize(const size_t capacity) {
    reset();
    capacity_ = capacity;
}

void rav::Fifo::Mpsc::reset() {
    read_ts_ = 0;
    write_ts_ = 0;
}

rav::Fifo::Spmc::Lock rav::Fifo::Spmc::prepare_for_write(const size_t number_of_elements) {
    if (write_ts_.load() - read_ts_.load() + number_of_elements > capacity_) {
        return {};  // Not enough free space in buffer.
    }

    Lock write_lock([this, number_of_elements] {
        write_ts_.fetch_add(number_of_elements);
    });
    write_lock.position.update(write_ts_, capacity_, number_of_elements);
    return write_lock;
}

rav::Fifo::Spmc::Lock rav::Fifo::Spmc::prepare_for_read(const size_t number_of_elements) {
    std::unique_lock guard(mutex_);

    if (write_ts_.load() - read_ts_.load() < number_of_elements) {
        return {};  // Not enough data available.
    }

    Lock read_lock(
        [this, number_of_elements] {
            read_ts_.fetch_add(number_of_elements);
        },
        std::move(guard)
    );
    read_lock.position.update(read_ts_, capacity_, number_of_elements);
    return read_lock;
}

void rav::Fifo::Spmc::resize(const size_t capacity) {
    reset();
    capacity_ = capacity;
}

void rav::Fifo::Spmc::reset() {
    read_ts_ = 0;
    write_ts_ = 0;
}

rav::Fifo::Mpmc::Lock rav::Fifo::Mpmc::prepare_for_write(const size_t number_of_elements) {
    std::unique_lock guard(mutex_);

    if (write_ts_ - read_ts_ + number_of_elements > capacity_) {
        return {};  // Not enough free space in buffer.
    }

    Lock write_lock(
        [this, number_of_elements] {
            write_ts_ += number_of_elements;
        },
        std::move(guard)
    );
    write_lock.position.update(write_ts_, capacity_, number_of_elements);
    return write_lock;
}

rav::Fifo::Mpmc::Lock rav::Fifo::Mpmc::prepare_for_read(const size_t number_of_elements) {
    std::unique_lock guard(mutex_);

    if (write_ts_ - read_ts_ < number_of_elements) {
        return {};  // Not enough data available.
    }

    Lock read_lock(
        [this, number_of_elements] {
            read_ts_ += number_of_elements;
        },
        std::move(guard)
    );
    read_lock.position.update(read_ts_, capacity_, number_of_elements);
    return read_lock;
}

size_t rav::Fifo::Mpmc::size() {
    std::unique_lock guard(mutex_);
    return write_ts_ - read_ts_;
}

void rav::Fifo::Mpmc::resize(const size_t capacity) {
    reset();
    capacity_ = capacity;
}

void rav::Fifo::Mpmc::reset() {
    read_ts_ = 0;
    write_ts_ = 0;
}
