/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/sdp/detail/sdp_time_active.hpp"

#include "ravennakit/core/string_parser.hpp"

tl::expected<void, std::string> rav::sdp::time_active_field::validate() const {
    if (start_time < 0) {
        return tl::unexpected("time: start time must be greater than or equal to 0");
    }
    if (stop_time < 0) {
        return tl::unexpected("time: stop time must be greater than or equal to 0");
    }
    return {};
}

tl::expected<std::string, std::string> rav::sdp::time_active_field::to_string() const {
    auto validated = validate();
    if (!validate()) {
        return tl::unexpected(validated.error());
    }
    return fmt::format("t={} {}", start_time, stop_time);
}

rav::sdp::time_active_field::parse_result<rav::sdp::time_active_field>
rav::sdp::time_active_field::parse_new(const std::string_view line) {
    string_parser parser(line);

    if (!parser.skip("t=")) {
        return parse_result<time_active_field>::err("time: expecting 't='");
    }

    time_active_field time;

    if (const auto start_time = parser.read_int<int64_t>()) {
        time.start_time = *start_time;
    } else {
        return parse_result<time_active_field>::err("time: failed to parse start time as integer");
    }

    if (!parser.skip(' ')) {
        return parse_result<time_active_field>::err("time: expecting space after start time");
    }

    if (const auto stop_time = parser.read_int<int64_t>()) {
        time.stop_time = *stop_time;
    } else {
        return parse_result<time_active_field>::err("time: failed to parse stop time as integer");
    }

    return parse_result<time_active_field>::ok(time);
}
