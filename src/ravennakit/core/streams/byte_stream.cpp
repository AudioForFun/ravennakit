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

#include "ravennakit/core/streams/byte_stream.hpp"

rav::ByteStream::ByteStream(std::vector<uint8_t> data) : data_(std::move(data)), write_position_(data_.size()) {}

void rav::ByteStream::reset() {
    data_.clear();
    read_position_ = 0;
    write_position_ = 0;
}

tl::expected<size_t, rav::InputStream::Error> rav::ByteStream::read(uint8_t* buffer, const size_t size) {
    if (data_.size() - read_position_ < size) {
        return tl::unexpected(InputStream::Error::insufficient_data);
    }
    std::memcpy(buffer, data_.data() + read_position_, size);
    read_position_ += size;
    return size;
}

bool rav::ByteStream::set_read_position(const size_t position) {
    if (position > data_.size()) {
        return false;
    }
    read_position_ = position;
    return true;
}

size_t rav::ByteStream::get_read_position() {
    return read_position_;
}

std::optional<size_t> rav::ByteStream::size() const {
    return data_.size();
}

bool rav::ByteStream::exhausted() {
    return read_position_ >= data_.size();
}

tl::expected<void, rav::OutputStream::Error> rav::ByteStream::write(const uint8_t* buffer, const size_t size) {
    try {
        if (write_position_ + size > data_.size()) {
            data_.resize(write_position_ + size, 0);
        }
    } catch (...) {
        return tl::unexpected(OutputStream::Error::out_of_memory);
    }

    std::memcpy(data_.data() + write_position_, buffer, size);
    write_position_ += size;
    return {};
}

tl::expected<void, rav::OutputStream::Error> rav::ByteStream::set_write_position(const size_t position) {
    write_position_ = position;
    return {};
}

size_t rav::ByteStream::get_write_position() {
    return write_position_;
}

void rav::ByteStream::flush() {}
