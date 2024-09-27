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

#include <sstream>
#include <vector>

#include "media_description.hpp"
#include "ravennakit/core/result.hpp"
#include "reference_clock.hpp"

namespace rav {

/**
 * A class that represents an SDP session description as defined in RFC 8866.
 * https://datatracker.ietf.org/doc/html/rfc8866
 */
class session_description {
  public:
    /// A type alias for a parse result.
    template<class T>
    using parse_result = result<T, const char*>;

    /**
     * Parses an SDP session description from a string.
     * @param sdp_text The SDP text to parse.
     * @return A pair containing the parse result and the session description. When parsing fails, the session
     * description is a default-constructed object.
     */
    static parse_result<session_description> parse_new(const std::string& sdp_text);

    /**
     * @returns The version of the SDP session description.
     */
    [[nodiscard]] int version() const;

    /**
     * @returns The origin of the SDP session description.
     */
    [[nodiscard]] const sdp::origin_field& origin() const;

    /**
     * @return The connection information of the SDP session description.
     */
    [[nodiscard]] std::optional<sdp::connection_info_field> connection_info() const;

    /**
     * @returns The session name of the SDP session description.
     */
    [[nodiscard]] std::string session_name() const;

    /**
     * @return The time field of the SDP session description.
     */
    [[nodiscard]] sdp::time_active_field time_active() const;

    /**
     * @returns The media descriptions of the SDP session description.
     */
    [[nodiscard]] const std::vector<sdp::media_description>& media_descriptions() const;

    /**
     * @return The direction of the media description. If the direction is not specified, the return value is sendrecv
     * which is the default as specified in RFC 8866 section 6.7).
     */
    [[nodiscard]] sdp::media_direction direction() const;

    /**
     * @return The reference clock of the session description.
     */
    [[nodiscard]] std::optional<sdp::reference_clock> ref_clock() const;

    /**
     * @return The media clock of the session description.
     */
    [[nodiscard]] const std::optional<sdp::media_clock>& media_clock() const;

  private:
    /// Type to specify which section of the SDP we are parsing
    enum class section { session_description, media_description };

    int version_ {};
    sdp::origin_field origin_;
    std::string session_name_;
    std::optional<sdp::connection_info_field> connection_info_;
    sdp::time_active_field time_active_;
    std::optional<std::string> session_information_;
    std::vector<sdp::media_description> media_descriptions_;
    std::optional<sdp::media_direction> media_direction_;
    std::optional<sdp::reference_clock> reference_clock_;
    std::optional<sdp::media_clock> media_clock_;

    static parse_result<int> parse_version(std::string_view line);
    parse_result<void> parse_attribute(std::string_view line);
};

}  // namespace rav
