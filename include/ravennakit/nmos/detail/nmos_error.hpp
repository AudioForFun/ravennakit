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
