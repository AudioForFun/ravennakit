/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#pragma once
#include <fmt/ostream.h>

namespace rav::nmos {

/**
 * Errors used in the NMOS node.
 */
enum class Error {
    no_registry_address_given,
    invalid_registry_address,
    invalid_api_version,
    invalid_id,
    failed_to_start_http_server,
};

/// Overload the output stream operator for the Node::Error enum class
inline std::ostream& operator<<(std::ostream& os, const Error error) {
    {
        switch (error) {
            case Error::invalid_registry_address:
                os << "invalid_registry_address";
                break;
            case Error::invalid_api_version:
                os << "invalid_api_version";
                break;
            case Error::failed_to_start_http_server:
                os << "failed_to_start_http_server";
                break;
            case Error::no_registry_address_given:
                os << "no_registry_address_given";
                break;
            case Error::invalid_id:
                os << "invalid_id";
                break;
        }
        return os;
    }
}

}  // namespace rav::nmos

/// Make Node::Error printable with fmt
template<>
struct fmt::formatter<rav::nmos::Error>: ostream_formatter {};
