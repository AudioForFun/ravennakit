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

#include "ravennakit/core/containers/byte_buffer.hpp"
#include "ravennakit/core/util/wrapping_uint.hpp"

namespace rav::rtp {

/**
 * This class holds state for an RTP packet and provides methods to encode it into a stream.
 */
class Packet {
  public:
    Packet() = default;

    /**
     * Sets the payload type.
     * @param value The payload type.
     */
    void payload_type(uint8_t value);

    /**
     * Sets the sequence number.
     * @param value The value to set.
     */
    void sequence_number(uint16_t value);

    /**
     * Increases the sequence number by the given value.
     * @param value The value to increment with.
     * @return The new sequence number.
     */
    WrappingUint<uint16_t> sequence_number_inc(uint16_t value);

    /**
     * Sets the timestamp.
     * @param value The timestamp value.
     */
    void set_timestamp(uint32_t value);

    /**
     * Increases the timestamp by the given value.
     * @param value The value to add.
     * @return The new timestamp.
     */
    WrappingUint<uint32_t> inc_timestamp(uint32_t value);

    /**
     * @return The timestamp.
     */
    [[nodiscard]] WrappingUint<uint32_t> get_timestamp() const {
        return timestamp_;
    }

    /**
     * @return The sequence number.
     */
    [[nodiscard]] WrappingUint<uint16_t> get_sequence_number() const {
        return sequence_number_;
    }

    /**
     * Sets the synchronization source identifier.
     * @param value The synchronization source identifier.
     */
    void ssrc(uint32_t value);

    /**
     * Encodes the RTP packet into given stream. This method writes to the stream as-is, the caller is responsible to
     * prepare the stream (reset it after previous calls).
     * @param payload_data The payload to encode.
     * @param payload_size The size of the payload in bytes.
     * @param buffer The buffer to write to.
     * @return true if the packet was successfully encoded and written, or false otherwise.
     */
    void encode(const uint8_t* payload_data, size_t payload_size, ByteBuffer& buffer) const;

  private:
    uint8_t payload_type_ {0};
    WrappingUint<uint16_t> sequence_number_ {0};
    WrappingUint<uint32_t> timestamp_ {0};
    uint32_t ssrc_ {0};
};

}  // namespace rav::rtp
