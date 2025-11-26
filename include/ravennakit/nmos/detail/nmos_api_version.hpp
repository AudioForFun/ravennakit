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

#include <string>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace rav::nmos {

/**
 * Represents the version of the NMOS API. Not to be confused with the version of resources.
 */
struct ApiVersion {
    int16_t major {0};
    int16_t minor {0};

    /**
     * @return  True if the version is valid, false otherwise.
     */
    [[nodiscard]] bool is_valid() const {
        return major > 0 && minor >= 0;
    }

    /**
     * @return A string representation of the version in the format "vX.Y".
     */
    [[nodiscard]] std::string to_string() const {
        return fmt::format("v{}.{}", major, minor);
    }

    /**
     * Creates an ApiVersion from a string.
     * @return An optional ApiVersion. If the string is not valid, an empty optional is returned.
     */
    static std::optional<ApiVersion> from_string(const std::string_view str) {
        StringParser parser(str);

        if (!parser.skip('v')) {
            return std::nullopt;
        }

        const auto major = parser.read_int<int16_t>();
        if (!major) {
            return std::nullopt;
        }

        if (!parser.skip('.')) {
            return std::nullopt;
        }

        const auto minor = parser.read_int<int16_t>();
        if (!minor) {
            return std::nullopt;
        }

        if (!parser.exhausted()) {
            return std::nullopt;
        }

        return ApiVersion {*major, *minor};
    }

    friend bool operator==(const ApiVersion& lhs, const ApiVersion& rhs) {
        return lhs.major == rhs.major && lhs.minor == rhs.minor;
    }

    friend bool operator!=(const ApiVersion& lhs, const ApiVersion& rhs) {
        return !(lhs == rhs);
    }
};

inline std::ostream& operator<<(std::ostream& os, const ApiVersion version) {
    os << version.to_string();
    return os;
}

}  // namespace rav::nmos

// Make ApiVersion compatible with fmt
template<>
struct fmt::formatter<rav::nmos::ApiVersion>: ostream_formatter {};
