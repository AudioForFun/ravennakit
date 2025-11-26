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

#include "ravennakit/core/byte_order.hpp"
#include "ravennakit/core/expected.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace rav {

/**
 * Baseclass for classes that want to provide stream-like access to data.
 */
class InputStream {
  public:
    enum class Error {
        insufficient_data,
        failed_to_set_read_position,
    };

    InputStream() = default;
    virtual ~InputStream() = default;

    /**
     * Reads data from the stream into the given buffer.
     * If the stream doesn't have enough data, then nothing will be read.
     * @param buffer The buffer to read data into.
     * @param size The number of bytes to read.
     * @return The number of bytes read.
     */
    [[nodiscard]] virtual tl::expected<size_t, Error> read(uint8_t* buffer, size_t size) = 0;

    /**
     * Sets the read position in the stream.
     * @param position The new read position.
     * @return True if the read position was successfully set.
     */
    [[nodiscard]] virtual bool set_read_position(size_t position) = 0;

    /**
     * @return The current read position in the stream.
     */
    [[nodiscard]] virtual size_t get_read_position() = 0;

    /**
     * @return The total number of bytes in this stream. Not all streams support this operation, in which case an empty
     * optional is returned.
     */
    [[nodiscard]] virtual std::optional<size_t> size() const = 0;

    /**
     * @return The number of bytes remaining to read in this stream. Not all streams support this operation, in which
     * case an empty optional is returned.
     */
    [[nodiscard]] std::optional<size_t> remaining();

    /**
     * @return True if the stream has no more data to read.
     */
    [[nodiscard]] virtual bool exhausted() = 0;

    /**
     * Skips size amount of bytes in the stream.
     * @param size The number of bytes to skip.
     * @return True if the skip was successful.
     */
    [[nodiscard]] bool skip(size_t size);

    /**
     * Reads size amount of bytes from the stream and returns it as a string.
     * Note: returned string might contain non-printable characters.
     * @param size The number of bytes to read.
     * @return The string read from the stream.
     */
    [[nodiscard]] tl::expected<std::string, Error> read_as_string(size_t size);

    /**
     * Reads a value from the given stream in native byte order (not to be confused with network-endian).
     * @tparam Type The type of the value to read.
     * @return The decoded value.
     */
    template<typename Type, std::enable_if_t<std::is_trivially_copyable_v<Type>, bool> = true>
    [[nodiscard]] tl::expected<Type, Error> read_ne() {
        Type value;
        auto result = read(reinterpret_cast<uint8_t*>(std::addressof(value)), sizeof(Type));
        if (!result) {
            return tl::unexpected(result.error());
        }
        if (result.value() != sizeof(Type)) {
            return tl::unexpected(Error::insufficient_data);
        }
        return value;
    }

    /**
     * Reads a big-endian value from the given stream.
     * @tparam Type The type of the value to read.
     * @return The decoded value.
     */
    template<typename Type, std::enable_if_t<std::is_trivially_copyable_v<Type>, bool> = true>
    [[nodiscard]] tl::expected<Type, Error> read_be() {
        return read_ne<Type>().map([](Type value) {
            return swap_if_le(value);
        });
    }

    /**
     * Reads a little-endian value from the given stream.
     * @tparam Type The type of the value to read.
     * @return The decoded value.
     */
    template<typename Type, std::enable_if_t<std::is_trivially_copyable_v<Type>, bool> = true>
    [[nodiscard]] tl::expected<Type, Error> read_le() {
        return read_ne<Type>().map([](Type value) {
            return swap_if_be(value);
        });
    }

    /**
     * @param e The error to convert to a string.
     * @return The string representation of the error.
     */
    static const char* to_string(const Error e) {
        switch (e) {
            case Error::insufficient_data:
                return "insufficient data";
            case Error::failed_to_set_read_position:
                return "failed to set read position";
        }
        return "unknown error";
    }
};

}  // namespace rav
