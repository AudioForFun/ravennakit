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

#include "ptp_clock_identity.hpp"
#include "ravennakit/core/byte_order.hpp"
#include "ravennakit/core/log.hpp"
#include "ravennakit/core/containers/byte_buffer.hpp"
#include "ravennakit/core/streams/output_stream.hpp"
#include "ravennakit/ptp/ptp_error.hpp"

#include "ravennakit/core/expected.hpp"

namespace rav::ptp {

/**
 * Represents a PTP port identity.
 * IEEE1588-2019: 5.3.5
 */
struct PortIdentity {
    constexpr static uint16_t k_port_number_min = 0x1;     // Inclusive
    constexpr static uint16_t k_port_number_max = 0xfffe;  // Inclusive
    constexpr static uint16_t k_port_number_all = 0xffff;  // Means all ports

    ClockIdentity clock_identity;
    uint16_t port_number {};  // Valid range: [k_port_number_min, k_port_number_max]

    /**
     * Construct a PTP port identity from a byte array.
     * @param data The data to construct the port identity from. Must be at least 10 bytes long.
     */
    static tl::expected<PortIdentity, Error> from_data(const BufferView<const uint8_t> data) {
        if (data.size_bytes() < 10) {
            return tl::unexpected(Error::invalid_message_length);
        }
        PortIdentity port_identity;
        port_identity.clock_identity = ClockIdentity::from_data(data);
        port_identity.port_number = rav::read_be<uint16_t>(data.data() + 8);
        return port_identity;
    }

    /**
     * Write the ptp_announce_message to a byte buffer.
     * @param buffer The buffer to write to.
     */
    void write_to(ByteBuffer& buffer) const {
        clock_identity.write_to(buffer);
        buffer.write_be(port_number);
    }

    /**
     * @return A string representation of the port identity.
     */
    [[nodiscard]] std::string to_string() const {
        return fmt::format("clock_identity={} port_number={}", clock_identity.to_string(), port_number);
    }

    /**
     * Checks if the port identity is valid. A port identity is valid if the port number is in the range [1, 0xfffe] and
     * the clock identity is valid.
     * @return True if valid, or false otherwise.
     */
    [[nodiscard]] bool is_valid() const {
        return port_number >= k_port_number_min && port_number <= k_port_number_max && clock_identity.is_valid();
    }

    /**
     * Checks the internal state of this object according to IEEE1588-2019. Asserts when something is wrong.
     */
    void assert_valid_state() const {
        clock_identity.assert_valid_state();
        RAV_ASSERT(port_number >= k_port_number_min, "port_number is below minimum");
        RAV_ASSERT(port_number <= k_port_number_max, "port_number is above maximum");
    }

    friend bool operator==(const PortIdentity& lhs, const PortIdentity& rhs) {
        return std::tie(lhs.clock_identity, lhs.port_number) == std::tie(rhs.clock_identity, rhs.port_number);
    }

    friend bool operator!=(const PortIdentity& lhs, const PortIdentity& rhs) {
        return !(lhs == rhs);
    }
};

}  // namespace rav::ptp
