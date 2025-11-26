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

namespace rav {

/**
 * A non-owning view of some data that can be read from.
 */
class InputStreamView: public InputStream {
  public:
    /**
     * Constructs a new input stream view pointing to the given data. It doesn't take ownership of the data so make sure
     * that the data outlives the stream.
     * @param data The data to read from. Must not be nullptr.
     * @param size The size of the data.
     */
    InputStreamView(const uint8_t* data, size_t size);

    /**
     * Constructs a new input stream view pointing to the given container. It doesn't take ownership of the container so
     * make sure that the container outlives the stream.
     * @tparam T The container type.
     * @param container The container to read from. Must not be empty.
     */
    template<class T>
    explicit InputStreamView(const T& container) : InputStreamView(container.data(), container.size()) {}

    ~InputStreamView() override = default;

    InputStreamView(const InputStreamView&) = default;
    InputStreamView& operator=(const InputStreamView&) = default;

    InputStreamView(InputStreamView&&) noexcept = default;
    InputStreamView& operator=(InputStreamView&&) noexcept = default;

    /**
     * Resets the stream to its initial state by setting the read position to 0.
     */
    void reset();

    // input_stream overrides
    [[nodiscard]] tl::expected<size_t, Error> read(uint8_t* buffer, size_t size) override;
    [[nodiscard]] bool set_read_position(size_t position) override;
    [[nodiscard]] size_t get_read_position() override;
    [[nodiscard]] std::optional<size_t> size() const override;
    [[nodiscard]] bool exhausted() override;

  private:
    const uint8_t* data_ {};
    size_t size_ {};
    size_t read_position_ = 0;
};

}  // namespace rav
