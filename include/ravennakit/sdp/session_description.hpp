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

#include "ravennakit/core/result.hpp"
#include "ravennakit/core/string.hpp"

namespace rav {

/**
 * A class that represents an SDP session description.
 */
class session_description {
  public:
    enum class netw_type { undefined, internet };
    enum class addr_type { undefined, ipv4, ipv6 };

    /// A type alias for a parse result.
    template<class T>
    using parse_result = result<T, const char*>;

    /**
     * A type which represents the origin field (o=*) of an SDP session description.
     * In general, the origin serves as a globally unique identifier for this version of the session description, and
     * the subfields excepting the version, taken together identify the session irrespective of any modifications.
     */
    struct origin {
        /// The user's login on the originating host, or "-" if the originating host does not support the concept of
        /// user IDs.
        std::string username;

        /// Holds a numeric string such that the tuple of <username>, <sess-id>, <nettype>, <addrtype>, and
        /// <unicast-address> forms a globally unique identifier for the session.
        std::string session_id;

        /// The version number for this session description.
        int session_version {};

        /// Specifies the type of network.
        netw_type network_type {netw_type::undefined};

        /// Specifies the type of address.
        addr_type address_type {addr_type::undefined};

        /// The address of the machine from which the session was created.
        std::string unicast_address;

        /**
         * Parses an origin field from a string.
         * @param line The string to parse.
         * @return A result indicating success or failure. When parsing fails, the error message will contain a
         * description of the error.
         */
        static parse_result<origin> parse(const std::string& line);
    };

    /**
     * A type representing the connection information (c=*) of an SDP session description.
     */
    struct connection {
        /// Specifies the type of network.
        netw_type network_type {netw_type::undefined};
        /// Specifies the type of address.
        addr_type address_type {addr_type::undefined};
        /// The address at which the media can be found.
        std::string address;
        /// Optional ttl
        std::optional<int> ttl;
        /// Optional number of addresses
        std::optional<int> number_of_addresses;

        /**
         * Parses a connection info field from a string.
         * @param line The string to parse.
         * @return A pair containing the parse result and the connection info. When parsing fails, the connection info
         * will be a default-constructed object.
         */
        static parse_result<connection> parse(const std::string& line);
    };

    struct address {
        std::string ip_address;
        std::optional<int> ttl;
        std::optional<int> number_of_addresses;
    };

    /**
     * Parses an SDP session description from a string.
     * @param sdp_text The SDP text to parse.
     * @return A pair containing the parse result and the session description. When parsing fails, the session
     * description is a default-constructed object.
     */
    static parse_result<session_description> parse(const std::string& sdp_text);

    /**
     * @returns The version of the SDP session description.
     */
    [[nodiscard]] int version() const;

    /**
     * @returns The origin of the SDP session description.
     */
    [[nodiscard]] const origin& get_origin() const;

    /**
     * @return The connection information of the SDP session description.
     */
    [[nodiscard]] std::optional<connection> get_connection() const;

    /**
     * @returns The session name of the SDP session description.
     */
    [[nodiscard]] std::string session_name() const;

    /**
     * @returns The connection information of the SDP session description.
     */
    [[nodiscard]] std::optional<connection> connection_info() const;

  private:
    int version_ {};
    origin origin_;
    std::string session_name_;
    std::optional<connection> connection_info_;

    static parse_result<int> parse_version(std::string_view line);
};

}  // namespace rav
