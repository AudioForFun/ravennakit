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

#include "ravennakit/core/math/fraction.hpp"
#include "ravennakit/core/json.hpp"

#include <cstdint>
#include <cmath>
#include <tuple>

namespace rav::aes67 {

/**
 * Represents packet time as specified in AES67-2023 Section 7.2.
 */
class PacketTime {
  public:
    Fraction<uint8_t> fraction {};

    PacketTime() = default;

    PacketTime(const uint8_t numerator, const uint8_t denominator) : fraction {numerator, denominator} {}

    /**
     * @param sample_rate The sample rate of the audio.
     * @return The signaled packet time as used in SDP.
     */
    [[nodiscard]] float signaled_ptime(const uint32_t sample_rate) const {
        if (sample_rate % 48000 > 0) {
            return static_cast<float>(fraction.numerator) * static_cast<float>(sample_rate / 48000 + 1)  // NOLINT
                * 48000 / static_cast<float>(sample_rate) / static_cast<float>(fraction.denominator);
        }

        return static_cast<float>(fraction.numerator) / static_cast<float>(fraction.denominator);
    }

    /**
     * @param sample_rate The sample rate of the audio.
     * @return The number of frames in a packet.
     */
    [[nodiscard]] uint32_t framecount(const uint32_t sample_rate) const {
        return framecount(signaled_ptime(sample_rate), sample_rate);
    }

    /**
     * @returns True if the packet time is valid, false otherwise.
     */
    [[nodiscard]] bool is_valid() const {
        if (fraction.denominator == 0) {
            return false;
        }
        if (fraction.numerator == 0) {
            return false;
        }
        return true;
    }

    /**
     * Calculates the amount of frames for a given signaled packet time.
     * @param signaled_ptime The signaled packet time in milliseconds.
     * @param sample_rate The sample rate of the audio.
     * @return The number of frames in a packet.
     */
    static uint16_t framecount(const float signaled_ptime, const uint32_t sample_rate) {
        return static_cast<uint16_t>(std::round(signaled_ptime * static_cast<float>(sample_rate) / 1000.0f));
    }

    /**
     * @return A packet time of 125 microseconds.
     */
    static PacketTime us_125() {
        return PacketTime {1, 8};
    }

    /**
     * @return A packet time of 250 microseconds.
     */
    static PacketTime us_250() {
        return PacketTime {1, 4};
    }

    /**
     * @return A packet time of 333 microseconds.
     */
    static PacketTime us_333() {
        return PacketTime {1, 3};
    }

    /**
     * @return A packet time of 1 millisecond.
     */
    static PacketTime ms_1() {
        return PacketTime {1, 1};
    }

    /**
     * @return A packet time of 4 milliseconds.
     */
    static PacketTime ms_4() {
        return PacketTime {4, 1};
    }

    friend bool operator==(const PacketTime& lhs, const PacketTime& rhs) {
        return lhs.fraction == rhs.fraction;
    }

    friend bool operator!=(const PacketTime& lhs, const PacketTime& rhs) {
        return !(lhs == rhs);
    }
};

#if RAV_HAS_BOOST_JSON

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const PacketTime& packet_time) {
    jv = {packet_time.fraction.numerator, packet_time.fraction.denominator};
}

inline PacketTime tag_invoke(const boost::json::value_to_tag<PacketTime>&, const boost::json::value& jv) {
    const auto& array = jv.as_array();
    PacketTime pt;
    pt.fraction.numerator = array.at(0).to_number<uint8_t>();
    pt.fraction.denominator = array.at(1).to_number<uint8_t>();
    return pt;
}

#endif

}  // namespace rav::aes67
