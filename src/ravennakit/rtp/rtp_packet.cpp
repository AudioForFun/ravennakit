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

#include "ravennakit/rtp/rtp_packet.hpp"

void rav::rtp::Packet::payload_type(const uint8_t value) {
    payload_type_ = value;
}

void rav::rtp::Packet::sequence_number(const uint16_t value) {
    sequence_number_ = value;
}

rav::WrappingUint<unsigned short> rav::rtp::Packet::sequence_number_inc(const uint16_t value) {
    sequence_number_ += value;
    return sequence_number_;
}

void rav::rtp::Packet::set_timestamp(const uint32_t value) {
    timestamp_ = value;
}

rav::WrappingUint<unsigned> rav::rtp::Packet::inc_timestamp(const uint32_t value) {
    timestamp_ += value;
    return timestamp_;
}

void rav::rtp::Packet::ssrc(const uint32_t value) {
    ssrc_ = value;
}

void rav::rtp::Packet::encode(const uint8_t* payload_data, const size_t payload_size, ByteBuffer& buffer) const {
    uint8_t v_p_x_cc = 0;
    v_p_x_cc |= 0b10000000;  // Version 2.
    v_p_x_cc |= 0b00000000;  // No padding.
    v_p_x_cc |= 0b00000000;  // No extension.
    v_p_x_cc |= 0b00000000;  // CSRC count of 0.
    buffer.write_be(v_p_x_cc);

    uint8_t m_pt = 0;
    m_pt |= 0b00000000;  // No marker bit.
    m_pt |= payload_type_ & 0b01111111;
    buffer.write_be(m_pt);

    // Sequence number
    buffer.write_be(sequence_number_);

    // Timestamp
    buffer.write_be(timestamp_);

    // SSRC
    buffer.write_be(ssrc_);

    // Payload
    buffer.write(payload_data, payload_size);
}
