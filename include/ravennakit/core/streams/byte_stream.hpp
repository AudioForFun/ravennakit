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

#include "input_stream.hpp"
#include "output_stream.hpp"

#include <vector>

namespace rav {

/**
 * Simple stream implementation that writes to and reads from a vector.
 */
class ByteStream final: public InputStream, public OutputStream {
  public:
    ByteStream() = default;
    explicit ByteStream(std::vector<uint8_t> data);

    /**
     * Resets the stream to its initial state by clearing the data and setting the read and write positions to 0.
     */
    void reset();

    // input_stream overrides
    [[nodiscard]] tl::expected<size_t, InputStream::Error> read(uint8_t* buffer, size_t size) override;
    [[nodiscard]] bool set_read_position(size_t position) override;
    [[nodiscard]] size_t get_read_position() override;
    [[nodiscard]] std::optional<size_t> size() const override;
    [[nodiscard]] bool exhausted() override;

    // output_stream overrides
    [[nodiscard]] tl::expected<void, OutputStream::Error> write(const uint8_t* buffer, size_t size) override;
    [[nodiscard]] tl::expected<void, OutputStream::Error> set_write_position(size_t position) override;
    [[nodiscard]] size_t get_write_position() override;
    void flush() override;

  private:
    std::vector<uint8_t> data_;
    size_t read_position_ = 0;
    size_t write_position_ = 0;
};

}  // namespace rav
