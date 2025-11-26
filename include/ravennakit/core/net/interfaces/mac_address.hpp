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

#include "ravennakit/core/exception.hpp"
#include "ravennakit/core/string.hpp"

#include <algorithm>
#include <array>
#include <cstdint>

namespace rav {

/**
 * Represents a MAC address.
 */
class MacAddress {
  public:
    MacAddress() = default;

    /**
     * Construct a MAC address from a byte array, assuming the array is (at least) 6 bytes long.
     * @param bytes The byte array containing the MAC address.
     */
    explicit MacAddress(const uint8_t* bytes) {
        std::copy_n(bytes, 6, address_.data());
    }

    /**
     * Constructs a MAC address from 6 individual bytes.
     * @param byte0 Byte 0
     * @param byte1 Byte 1
     * @param byte2 Byte 2
     * @param byte3 Byte 3
     * @param byte4 Byte 4
     * @param byte5 Byte 5
     */
    MacAddress(
        const uint8_t byte0, const uint8_t byte1, const uint8_t byte2, const uint8_t byte3, const uint8_t byte4, const uint8_t byte5
    ) :
        address_ {byte0, byte1, byte2, byte3, byte4, byte5} {}

    /**
     * Construct a MAC address from a string. The string must be in the format "00:11:22:33:44:55".
     * @param str The string containing the MAC address.
     */
    explicit MacAddress(const char* str) {
        const auto parts = string_split(str, ':');
        if (parts.size() != 6) {
            RAV_THROW_EXCEPTION("Invalid MAC address format: {}", str);
        }
        for (size_t i = 0; i < 6; ++i) {
            auto number = rav::string_to_int<uint8_t>(std::string_view(parts[i]), true, 16);
            if (!number) {
                RAV_THROW_EXCEPTION("Failed to parse MAC address part: {}", parts[i]);
            }
            address_[i] = number.value();
        }
    }

    /**
     * @returns The MAC address as a byte array.
     */
    [[nodiscard]] const std::array<uint8_t, 6>& bytes() const {
        return address_;
    }

    /**
     * @returns True if the MAC address is valid, false otherwise. A MAC address is considered valid if it is not all
     * zeros.
     */
    [[nodiscard]] bool is_valid() const {
        return std::any_of(address_.begin(), address_.end(), [](const uint8_t byte) {
            return byte != 0;
        });
    }

    /**
     * @return The MAC address as a string.
     */
    [[nodiscard]] std::string to_string(const char* separator = ":") const {
        return fmt::format(
            "{:02x}{}{:02x}{}{:02x}{}{:02x}{}{:02x}{}{:02x}", address_[0], separator, address_[1], separator, address_[2], separator,
            address_[3], separator, address_[4], separator, address_[5]
        );
    }

    friend bool operator==(const MacAddress& lhs, const MacAddress& rhs) {
        return lhs.address_ == rhs.address_;
    }

    friend bool operator!=(const MacAddress& lhs, const MacAddress& rhs) {
        return !(lhs == rhs);
    }

  private:
    std::array<uint8_t, 6> address_ {};
};

}  // namespace rav
