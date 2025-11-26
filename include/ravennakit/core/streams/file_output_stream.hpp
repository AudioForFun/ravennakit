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

#include "output_stream.hpp"
#include "ravennakit/core/file.hpp"

namespace rav {

/**
 * An implementation of output_stream for writing to a file.
 */
class FileOutputStream final: public OutputStream {
  public:
    explicit FileOutputStream(const std::filesystem::path& file) {
        ofstream_.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofstream_.open(file, std::ios::binary);
        if (!ofstream_.is_open()) {  // Note: not sure if this check is necessary
            RAV_THROW_EXCEPTION("Failed to open file");
        }
        ofstream_.seekp(0);
    }

    ~FileOutputStream() override = default;

    // output_stream overrides
    tl::expected<void, Error> write(const uint8_t* buffer, const size_t size) override {
        ofstream_.write(reinterpret_cast<const char*>(buffer), static_cast<std::streamsize>(size));
        return {};
    }

    tl::expected<void, Error> set_write_position(const size_t position) override {
        ofstream_.seekp(static_cast<std::streamsize>(position));
        return {};
    }

    [[nodiscard]] size_t get_write_position() override {
        return static_cast<size_t>(ofstream_.tellp());
    }

    void flush() override {
        ofstream_.flush();
    }

  private:
    std::ofstream ofstream_;
};

}  // namespace rav
