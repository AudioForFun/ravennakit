/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/sdp/session_description.hpp"

#include "ravennakit/core/assert.hpp"
#include "ravennakit/core/exception.hpp"
#include "ravennakit/core/log.hpp"
#include "ravennakit/core/string_parser.hpp"
#include "ravennakit/core/todo.hpp"

namespace {
constexpr auto k_sdp_ptime = "ptime";
constexpr auto k_sdp_max_ptime = "maxptime";
constexpr auto k_sdp_rtp_map = "rtpmap";
constexpr auto k_sdp_sendrecv = "sendrecv";
constexpr auto k_sdp_sendonly = "sendonly";
constexpr auto k_sdp_recvonly = "recvonly";
constexpr auto k_sdp_inactive = "recvonly";
constexpr auto k_sdp_ts_refclk = "ts-refclk";
constexpr auto k_sdp_inet = "IN";
constexpr auto k_sdp_ipv4 = "IP4";
constexpr auto k_sdp_ipv6 = "IP6";
}  // namespace

rav::session_description::parse_result<rav::session_description::origin_field>
rav::session_description::origin_field::parse_new(const std::string_view line) {
    string_parser parser(line);

    if (!parser.skip("o=")) {
        return result<origin_field, const char*>::err("origin: expecting 'o='");
    }

    origin_field o;

    // Username
    if (const auto username = parser.read_until(' ')) {
        o.username = *username;
    } else {
        return parse_result<origin_field>::err("origin: failed to parse username");
    }

    // Session id
    if (const auto session_id = parser.read_until(' ')) {
        o.session_id = *session_id;
    } else {
        return parse_result<origin_field>::err("origin: failed to parse session id");
    }

    // Session version
    if (const auto version = parser.read_int<int>()) {
        o.session_version = *version;
        parser.skip(' ');
    } else {
        return parse_result<origin_field>::err("origin: failed to parse session version");
    }

    // Network type
    if (const auto network_type = parser.read_until(' ')) {
        if (*network_type != k_sdp_inet) {
            return parse_result<origin_field>::err("origin: invalid network type");
        }
        o.network_type = netw_type::internet;
    } else {
        return parse_result<origin_field>::err("origin: failed to parse network type");
    }

    // Address type
    if (const auto address_type = parser.read_until(' ')) {
        if (*address_type == k_sdp_ipv4) {
            o.address_type = addr_type::ipv4;
        } else if (*address_type == k_sdp_ipv6) {
            o.address_type = addr_type::ipv6;
        } else {
            return parse_result<origin_field>::err("origin: invalid address type");
        }
    } else {
        return parse_result<origin_field>::err("origin: failed to parse address type");
    }

    // Address
    if (const auto address = parser.read_until(' ')) {
        o.unicast_address = *address;
    } else {
        return parse_result<origin_field>::err("origin: failed to parse address");
    }

    return parse_result<origin_field>::ok(std::move(o));
}

rav::result<rav::session_description::connection_info_field, const char*>
rav::session_description::connection_info_field::parse_new(const std::string_view line) {
    string_parser parser(line);

    if (!parser.skip("c=")) {
        return result<connection_info_field, const char*>::err("connection: expecting 'c='");
    }

    connection_info_field info;

    // Network type
    if (const auto network_type = parser.read_until(' ')) {
        if (*network_type == k_sdp_inet) {
            info.network_type = netw_type::internet;
        } else {
            return result<connection_info_field, const char*>::err("connection: invalid network type");
        }
    } else {
        return result<connection_info_field, const char*>::err("connection: failed to parse network type");
    }

    // Address type
    if (const auto address_type = parser.read_until(' ')) {
        if (*address_type == k_sdp_ipv4) {
            info.address_type = addr_type::ipv4;
        } else if (*address_type == k_sdp_ipv6) {
            info.address_type = addr_type::ipv6;
        } else {
            return result<connection_info_field, const char*>::err("connection: invalid address type");
        }
    } else {
        return result<connection_info_field, const char*>::err("connection: failed to parse address type");
    }

    // Address
    if (const auto address = parser.read_until('/')) {
        info.address = *address;
    }

    if (parser.exhausted()) {
        return parse_result<connection_info_field>::ok(std::move(info));
    }

    // Parse optional ttl and number of addresses
    if (info.address_type == addr_type::ipv4) {
        if (auto ttl = parser.read_int<int32_t>()) {
            info.ttl = *ttl;
        } else {
            return parse_result<connection_info_field>::err("connection: failed to parse ttl for ipv4 address");
        }
        if (parser.skip('/')) {
            if (auto num_addresses = parser.read_int<int32_t>()) {
                info.number_of_addresses = *num_addresses;
            } else {
                return parse_result<connection_info_field>::err(
                    "connection: failed to parse number of addresses for ipv4 address"
                );
            }
        }
    } else if (info.address_type == addr_type::ipv6) {
        if (auto num_addresses = parser.read_int<int32_t>()) {
            info.number_of_addresses = *num_addresses;
        } else {
            return parse_result<connection_info_field>::err(
                "connection: failed to parse number of addresses for ipv4 address"
            );
        }
    }

    if (!parser.exhausted()) {
        return parse_result<connection_info_field>::err("connection: unexpected characters at end of line");
    }

    return parse_result<connection_info_field>::ok(std::move(info));
}

rav::session_description::parse_result<rav::session_description::time_active_field>
rav::session_description::time_active_field::parse_new(const std::string_view line) {
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

rav::session_description::parse_result<rav::session_description::media_description>
rav::session_description::media_description::parse_new(std::string_view line) {
    string_parser parser(line);

    if (!parser.skip("m=")) {
        return parse_result<media_description>::err("media: expecting 'm='");
    }

    media_description media;

    // Media type
    if (const auto media_type = parser.read_until(' ')) {
        media.media_type_ = *media_type;
    } else {
        return parse_result<media_description>::err("media: failed to parse media type");
    }

    // Port
    if (const auto port = parser.read_int<uint16_t>()) {
        media.port_ = *port;
        if (parser.skip('/')) {
            if (const auto num_ports = parser.read_int<uint16_t>()) {
                media.number_of_ports_ = *num_ports;
            } else {
                return parse_result<media_description>::err("media: failed to parse number of ports as integer");
            }
        } else {
            media.number_of_ports_ = 1;
        }
        parser.skip(' ');
    } else {
        return parse_result<media_description>::err("media: failed to parse port as integer");
    }

    // Protocol
    if (const auto protocol = parser.read_until(' ')) {
        media.protocol_ = *protocol;
    } else {
        return parse_result<media_description>::err("media: failed to parse protocol");
    }

    // Formats
    while (const auto format_str = parser.read_until(' ')) {
        if (const auto value = rav::ston<int8_t>(*format_str)) {
            media.formats_.push_back({*value, {}, {}, {}});
        } else {
            return parse_result<media_description>::err("media: format integer parsing failed");
        }
    }

    return parse_result<media_description>::ok(std::move(media));
}

rav::session_description::parse_result<void>
rav::session_description::media_description::parse_attribute(const std::string_view line) {
    string_parser parser(line);

    if (!parser.skip("a=")) {
        return parse_result<void>::err("attribute: expecting 'a='");
    }

    auto key = parser.read_until(':');

    if (!key) {
        return parse_result<void>::err("attribute: expecting key");
    }

    if (key == k_sdp_rtp_map) {
        if (const auto value = parser.read_until_end()) {
            auto format_result = format::parse_new(*value);
            if (format_result.is_err()) {
                return parse_result<void>::err(format_result.get_err());
            }

            auto format = format_result.move_ok();

            bool found = false;
            for (auto& fmt : formats_) {
                if (fmt.payload_type == format.payload_type) {
                    fmt = format;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return parse_result<void>::err("media: rtpmap attribute for unknown payload type");
            }
        } else {
            return parse_result<void>::err("media: failed to parse rtpmap value");
        }
    } else if (key == k_sdp_ptime) {
        if (const auto value = parser.read_until_end()) {
            if (const auto ptime = rav::stod(value->data())) {
                if (*ptime < 0) {
                    return parse_result<void>::err("media: ptime must be a positive number");
                }
                ptime_ = *ptime;
            } else {
                return parse_result<void>::err("media: failed to parse ptime as double");
            }
        }
    } else if (key == k_sdp_max_ptime) {
        if (const auto value = parser.read_until_end()) {
            if (const auto maxptime = rav::stod(value->data())) {
                if (*maxptime < 0) {
                    return parse_result<void>::err("media: maxptime must be a positive number");
                }
                max_ptime_ = *maxptime;
            } else {
                return parse_result<void>::err("media: failed to parse ptime as double");
            }
        }
    } else if (key == k_sdp_sendrecv) {
        media_direction_ = media_direction::sendrecv;
    } else if (key == k_sdp_sendonly) {
        media_direction_ = media_direction::sendonly;
    } else if (key == k_sdp_recvonly) {
        media_direction_ = media_direction::recvonly;
    } else if (key == k_sdp_inactive) {
        media_direction_ = media_direction::inactive;
    } else {
        RAV_WARNING("Ignoring unknown attribute on media: {}", *key);
    }

    return parse_result<void>::ok();
}

const std::string& rav::session_description::media_description::media_type() const {
    return media_type_;
}

uint16_t rav::session_description::media_description::port() const {
    return port_;
}

uint16_t rav::session_description::media_description::number_of_ports() const {
    return number_of_ports_;
}

const std::string& rav::session_description::media_description::protocol() const {
    return protocol_;
}

const std::vector<rav::session_description::format>& rav::session_description::media_description::formats() const {
    return formats_;
}

const std::vector<rav::session_description::connection_info_field>&
rav::session_description::media_description::connection_infos() const {
    return connection_infos_;
}

void rav::session_description::media_description::add_connection_info(connection_info_field connection_info) {
    connection_infos_.push_back(std::move(connection_info));
}

std::optional<double> rav::session_description::media_description::ptime() const {
    return ptime_;
}

std::optional<double> rav::session_description::media_description::max_ptime() const {
    return max_ptime_;
}

std::optional<rav::session_description::media_direction>
rav::session_description::media_description::direction() const {
    return media_direction_;
}

rav::session_description::parse_result<rav::session_description::format>
rav::session_description::format::parse_new(const std::string_view line) {
    string_parser parser(line);

    format map;

    if (const auto payload_type = parser.read_int<int8_t>()) {
        map.payload_type = *payload_type;
        if (!parser.skip(' ')) {
            return parse_result<format>::err("rtpmap: expecting space after payload type");
        }
    } else {
        return parse_result<format>::err("rtpmap: invalid payload type");
    }

    if (const auto encoding_name = parser.read_until('/')) {
        map.encoding_name = *encoding_name;
    } else {
        return parse_result<format>::err("rtpmap: failed to parse encoding name");
    }

    if (const auto clock_rate = parser.read_int<int>()) {
        map.clock_rate = *clock_rate;
    } else {
        return parse_result<format>::err("rtpmap: invalid clock rate");
    }

    if (parser.skip('/')) {
        if (const auto num_channels = parser.read_int<int32_t>()) {
            // Note: strictly speaking the encoding parameters can be anything, but as of now it's only used for channels.
            map.num_channels = *num_channels;
        } else {
            return parse_result<format>::err("rtpmap: failed to parse number of channels");
        }
    } else {
        map.num_channels = 1;
    }

    return parse_result<format>::ok(map);
}

rav::session_description::parse_result<rav::session_description>
rav::session_description::parse_new(const std::string& sdp_text) {
    session_description sd;
    string_parser parser(sdp_text);

    for (auto line = parser.read_line(); line.has_value(); line = parser.read_line()) {
        if (line->empty()) {
            continue;
        }

        switch (line->front()) {
            case 'v': {
                auto result = parse_version(*line);
                if (result.is_err()) {
                    return parse_result<session_description>::err(result.get_err());
                }
                sd.version_ = result.move_ok();
                break;
            }
            case 'o': {
                auto result = origin_field::parse_new(*line);
                if (result.is_err()) {
                    return parse_result<session_description>::err(result.get_err());
                }
                sd.origin_ = result.move_ok();
                break;
            }
            case 's': {
                sd.session_name_ = line->substr(2);
                break;
            }
            case 'c': {
                auto result = connection_info_field::parse_new(*line);
                if (result.is_err()) {
                    return parse_result<session_description>::err(result.get_err());
                }
                if (!sd.media_descriptions_.empty()) {
                    sd.media_descriptions_.back().add_connection_info(result.move_ok());
                } else {
                    sd.connection_info_ = result.move_ok();
                }
                break;
            }
            case 't': {
                auto result = time_active_field::parse_new(*line);
                if (result.is_err()) {
                    return parse_result<session_description>::err(result.get_err());
                }
                sd.time_active_ = result.move_ok();
                break;
            }
            case 'm': {
                auto result = media_description::parse_new(*line);
                if (result.is_err()) {
                    return parse_result<session_description>::err(result.get_err());
                }
                sd.media_descriptions_.push_back(result.move_ok());
                break;
            }
            case 'a': {
                if (!sd.media_descriptions_.empty()) {
                    auto result = sd.media_descriptions_.back().parse_attribute(*line);
                    if (result.is_err()) {
                        return parse_result<session_description>::err(result.get_err());
                    }
                } else {
                    auto result = sd.parse_attribute(*line);
                    if (result.is_err()) {
                        return parse_result<session_description>::err(result.get_err());
                    }
                }
                break;
            }
            default:
                continue;
        }
    }

    return parse_result<session_description>::ok(std::move(sd));
}

int rav::session_description::version() const {
    return version_;
}

const rav::session_description::origin_field& rav::session_description::origin() const {
    return origin_;
}

std::optional<rav::session_description::connection_info_field> rav::session_description::connection_info() const {
    return connection_info_;
}

std::string rav::session_description::session_name() const {
    return session_name_;
}

rav::session_description::time_active_field rav::session_description::time_active() const {
    return time_active_;
}

const std::vector<rav::session_description::media_description>& rav::session_description::media_descriptions() const {
    return media_descriptions_;
}

rav::session_description::media_direction rav::session_description::direction() const {
    if (media_direction_.has_value()) {
        return *media_direction_;
    }
    return media_direction::sendrecv;
}

rav::session_description::parse_result<int> rav::session_description::parse_version(const std::string_view line) {
    if (!starts_with(line, "v=")) {
        return parse_result<int>::err("expecting line to start with 'v='");
    }

    if (const auto v = rav::ston<int>(line.substr(2)); v.has_value()) {
        if (*v != 0) {
            return parse_result<int>::err("invalid version");
        }
        return parse_result<int>::ok(*v);
    }

    return parse_result<int>::err("failed to parse integer from string");
}

rav::session_description::parse_result<void> rav::session_description::parse_attribute(const std::string_view line) {
    string_parser parser(line);

    if (!parser.skip("a=")) {
        return parse_result<void>::err("attribute: expecting 'a='");
    }

    const auto key = parser.read_until(':');

    if (!key) {
        return parse_result<void>::err("attribute: expecting key");
    }

    if (key == k_sdp_sendrecv) {
        media_direction_ = media_direction::sendrecv;
    } else if (key == k_sdp_sendonly) {
        media_direction_ = media_direction::sendonly;
    } else if (key == k_sdp_recvonly) {
        media_direction_ = media_direction::recvonly;
    } else if (key == k_sdp_inactive) {
        media_direction_ = media_direction::inactive;
    } else if (key == k_sdp_ts_refclk) {
        // TODO("Implement!");
    } else {
        RAV_WARNING("Ignoring unknown attribute on session: {}", *key);
    }

    return parse_result<void>::ok();
}
