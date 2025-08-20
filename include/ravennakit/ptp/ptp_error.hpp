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

namespace rav::ptp {

enum class Error {
    invalid_data,
    invalid_header_length,
    invalid_message_length,
    only_ordinary_clock_supported,
    only_slave_supported,
    failed_to_get_network_interfaces,
    network_interface_not_found,
    no_mac_address_available,
    invalid_clock_identity,
    port_invalid,
    port_already_exists,
};

inline const char* to_string(const Error error) {
    switch (error) {
        case Error::invalid_data:
            return "invalid data";
        case Error::invalid_header_length:
            return "invalid header length";
        case Error::invalid_message_length:
            return "invalid message length";
        case Error::only_ordinary_clock_supported:
            return "only ordinary clock supported";
        case Error::only_slave_supported:
            return "only slave supported";
        case Error::failed_to_get_network_interfaces:
            return "failed to get network interfaces";
        case Error::network_interface_not_found:
            return "network interface not found";
        case Error::no_mac_address_available:
            return "no MAC address available";
        case Error::invalid_clock_identity:
            return "invalid clock identity";
        case Error::port_invalid:
            return "port invalid";
        case Error::port_already_exists:
            return "port already exists";
        default:
            return "unknown error";
    }
}

}  // namespace rav::ptp
