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
#include "ravennakit/core/result.hpp"

#include <cstdint>
#include <string_view>

namespace rav::sdp {

/**
 * A type representing the time field (t=*) of an SDP session description.
 * Defined as seconds since January 1, 1900, UTC.
 */
struct time_active_field {
    /// The start time of the session.
    int64_t start_time {-1};
    /// The stop time of the session.
    int64_t stop_time {-1};

    /// A type alias for a parse result.
    template<class T>
    using parse_result = result<T, std::string>;

    /**
     * Parses a time field from a string.
     * @param line The string to parse.
     * @return A pair containing the parse result and the time field.
     */
    static parse_result<time_active_field> parse_new(std::string_view line);
};

}
