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

#include "ravennakit/core/string_parser.hpp"
#include "ravennakit/ptp/types/ptp_timestamp.hpp"
#include "ravennakit/core/json.hpp"

#include <cstdint>
#include <string>
#include <fmt/format.h>

namespace rav::nmos {

/**
 * Represents a timestamp in the format of seconds and nanoseconds.
 *
 * The Timestamp class encapsulates the concept of a specific point in time,
 * allowing for comparisons and operations such as timestamp validity and ordering.
 */
struct Timestamp {
    uint64_t seconds = 0;      // seconds since epoch
    uint32_t nanoseconds = 0;  // nanoseconds since the last second

    Timestamp() = default;

    Timestamp(const uint64_t seconds_, const uint32_t nanoseconds_) : seconds(seconds_), nanoseconds(nanoseconds_) {
        RAV_ASSERT(nanoseconds < 1000000000, "Nanoseconds must be less than 1 billion.");
    }

    explicit Timestamp(const ptp::Timestamp timestamp) : seconds(timestamp.raw_seconds()), nanoseconds(timestamp.raw_nanoseconds()) {}

    friend bool operator<(const Timestamp& lhs, const Timestamp& rhs) {
        return lhs.seconds < rhs.seconds || (lhs.seconds == rhs.seconds && lhs.nanoseconds < rhs.nanoseconds);
    }

    friend bool operator<=(const Timestamp& lhs, const Timestamp& rhs) {
        return rhs >= lhs;
    }

    friend bool operator>(const Timestamp& lhs, const Timestamp& rhs) {
        return rhs < lhs;
    }

    friend bool operator>=(const Timestamp& lhs, const Timestamp& rhs) {
        return !(lhs < rhs);
    }

    /**
     * Increases the timestamp by one nanosecond.
     */
    void inc() {
        if (nanoseconds < 999999999) {
            ++nanoseconds;
        } else {
            nanoseconds = 0;
            ++seconds;
        }
    }

    /**
     * Updates the timestamp with a new timestamp.
     * If the new timestamp is greater than the current timestamp, it updates the timestamp.
     * Otherwise, it increments the timestamp by one nanosecond.
     * @param timestamp The new timestamp to update the timestamp with.
     */
    void update(const ptp::Timestamp timestamp) {
        if (timestamp > ptp::Timestamp(seconds, nanoseconds)) {
            seconds = timestamp.raw_seconds();
            nanoseconds = timestamp.raw_nanoseconds();
        } else {
            inc();
        }
    }

    /**
     * Checks whether the NMOS resource timestamp is valid.
     * A timestamp is considered valid if either the `seconds` or `nanoseconds` component is non-zero.
     * @return true if the timestamp is valid, false otherwise.
     */
    [[nodiscard]] bool is_valid() const {
        return seconds != 0 || nanoseconds != 0;
    }

    /**
     * @return A string representation of the timestamp in the format "seconds.nanoseconds".
     */
    [[nodiscard]] std::string to_string() const {
        return fmt::format("{}:{}", seconds, nanoseconds);
    }

    /**
     * Parses a given string to a Timestamp object.
     * @param input The string to be parsed into an object.
     * @return An optional Timestamp object if parsing is successful, otherwise std::nullopt.
     */
    static std::optional<Timestamp> from_string(const std::string_view input) {
        StringParser parser(input);
        if (parser.skip(' ')) {
            return std::nullopt;
        }
        const auto seconds = parser.read_int<uint64_t>();
        if (!seconds) {
            return std::nullopt;
        }
        if (!parser.skip(':')) {
            return std::nullopt;
        }
        const auto nanoseconds = parser.read_int<uint32_t>();
        if (!nanoseconds) {
            return std::nullopt;
        }
        if (!parser.exhausted()) {
            return std::nullopt;
        }
        return Timestamp {*seconds, *nanoseconds};
    }
};

/// An nmos version is represented as TAI timestamp
using Version = Timestamp;

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const Timestamp& timestamp) {
    jv = timestamp.to_string();
}

inline Timestamp tag_invoke(const boost::json::value_to_tag<Timestamp>&, const boost::json::value& jv) {
    return Timestamp::from_string(jv.as_string()).value();
}

}  // namespace rav::nmos
