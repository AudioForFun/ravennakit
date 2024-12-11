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

#include <algorithm>
#include <array>
#include <cstdint>
#include "ravennakit/core/format.hpp"

namespace rav {

/**
 * Represents a MAC address.
 */
class mac_address {
  public:
    mac_address() = default;

    /**
     * Construct a MAC address from a byte array, assuming the array is (at least) 6 bytes long.
     * @param bytes The byte array containing the MAC address.
     */
    explicit mac_address(const uint8_t* bytes) {
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
    mac_address(
        const uint8_t byte0, const uint8_t byte1, const uint8_t byte2, const uint8_t byte3, const uint8_t byte4,
        const uint8_t byte5
    ) :
        address_ {byte0, byte1, byte2, byte3, byte4, byte5} {}

    /**
     * @returns The MAC address as a byte array.
     */
    [[nodiscard]] const std::array<uint8_t, 6>& to_bytes() const {
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
    std::string to_string() const {
        return fmt::format(
            "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}", address_[0], address_[1], address_[2], address_[3],
            address_[4], address_[5]
        );
    }

  private:
    std::array<uint8_t, 6> address_ {};
};

}  // namespace rav
