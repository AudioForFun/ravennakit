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

#include "ravennakit/core/json.hpp"

#include <fmt/ostream.h>

namespace rav::nmos {

/**
 * The mode of operation for the NMOS node.
 */
enum class OperationMode {
    /// Discovers registries via mDNS, falling back to p2p if no registry is available.
    mdns_p2p,
    /// Connects to a registry via a manually specified address.
    manual,
    /// The node does not register with a registry and only does peer-to-peer discovery.
    p2p,
};

inline const char* to_string(const OperationMode operation_mode) {
    switch (operation_mode) {
        case OperationMode::mdns_p2p:
            return "mdns_p2p";
        case OperationMode::manual:
            return "manual";
        case OperationMode::p2p:
            return "p2p";
    }
    return "unknown";
}

inline std::optional<OperationMode> operation_mode_from_string(const std::string_view str) {
    if (str == "mdns_p2p") {
        return OperationMode::mdns_p2p;
    }
    if (str == "manual") {
        return OperationMode::manual;
    }
    if (str == "p2p") {
        return OperationMode::p2p;
    }
    return std::nullopt;
}

/// Overload the output stream operator for the Node::Error enum class
inline std::ostream& operator<<(std::ostream& os, const OperationMode operation_mode) {
    os << to_string(operation_mode);
    return os;
}

}  // namespace rav::nmos

/// Make Node::OperationMode printable with fmt
template<>
struct fmt::formatter<rav::nmos::OperationMode>: ostream_formatter {};
