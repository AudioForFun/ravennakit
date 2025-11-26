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

#include "ravennakit/core/containers/buffer_view.hpp"
#include "ravennakit/core/containers/byte_buffer.hpp"
#include "ravennakit/core/streams/output_stream.hpp"
#include "ravennakit/core/util/wrapping_uint.hpp"
#include "ravennakit/ptp/ptp_definitions.hpp"
#include "ravennakit/ptp/ptp_error.hpp"
#include "ravennakit/ptp/types/ptp_port_identity.hpp"
#include "ravennakit/ptp/types/ptp_sdo_id.hpp"
#include "ravennakit/ptp/types/ptp_time_interval.hpp"

#include <cstdint>
#include "ravennakit/core/expected.hpp"

namespace rav::ptp {

struct Version {
    uint8_t major {};
    uint8_t minor {};
};

bool operator==(const Version& lhs, const Version& rhs);
bool operator!=(const Version& lhs, const Version& rhs);

/**
 * Provides a view over given data, interpreting it as a PTP message header.
 */
struct MessageHeader {
    constexpr static size_t k_header_size = 34;

    struct FlagField {
        bool alternate_master_flag {};      // Announce, Sync, Follow_Up, Delay_Resp
        bool two_step_flag {};              // Sync, Pdelay_resp
        bool unicast_flag {};               // All
        bool profile_specific_1 {};         // All
        bool profile_specific_2 {};         // All
        bool leap61 {};                     // Announce
        bool leap59 {};                     // Announce
        bool current_utc_offset_valid {};   // Announce
        bool ptp_timescale {};              // Announce
        bool time_traceable {};             // Announce
        bool frequency_traceable {};        // Announce
        bool synchronization_uncertain {};  // Announce

        static FlagField from_octets(uint8_t octet1, uint8_t octet2);
        [[nodiscard]] uint16_t to_octets() const;

        [[nodiscard]] auto tie_members() const;
    };

    SdoId sdo_id;
    MessageType message_type {};
    Version version;
    uint16_t message_length {};
    uint8_t domain_number {};
    FlagField flags;
    int64_t correction_field {};
    PortIdentity source_port_identity;
    WrappingUint<uint16_t> sequence_id {};
    int8_t log_message_interval {};

    /**
     * Creates a PTP message header from the given data.
     * @param data The data to interpret as a PTP message header.
     * @return A PTP message header if the data is valid, otherwise an error.
     */
    static tl::expected<MessageHeader, Error> from_data(BufferView<const uint8_t> data);

    /**
     * Write the ptp_announce_message to a byte buffer.
     * @param buffer The buffer to write to.
     */
    void write_to(ByteBuffer& buffer) const;

    /**
     * Converts the PTP message header to a human-readable string.
     */
    [[nodiscard]] std::string to_string() const;

    /**
     * Returns a tuple of the members of the PTP message header.
     */
    [[nodiscard]] auto tie() const;

    /**
     * Tests if this header matches the other header. A header is said to be matching when the source port identity and
     * sequence ID are equal (IEEE 1588-2019: 9.5.5).
     * @param other The other header to compare to.
     * @returns True if the two headers match, false otherwise.
     */
    [[nodiscard]] bool matches(const MessageHeader& other) const;
};

bool operator==(const MessageHeader::FlagField& lhs, const MessageHeader::FlagField& rhs);
bool operator!=(const MessageHeader::FlagField& lhs, const MessageHeader::FlagField& rhs);

bool operator==(const MessageHeader& lhs, const MessageHeader& rhs);
bool operator!=(const MessageHeader& lhs, const MessageHeader& rhs);

}  // namespace rav::ptp
