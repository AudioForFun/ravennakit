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

#include "platform.hpp"

#if RAV_WINDOWS
    #include <windows.h>
    #include <processenv.h>
#endif

#include <string>
#include <optional>
#include <cassert>

namespace rav {

/**
 * Gets the value of an environment variable.
 * @param name The name of the variable to retrieve. Pointer must be non-null and \0 terminated.
 * @return The environment variable value, or an empty optional if the variable was not found.
 */
inline std::optional<std::string> get_env(const char* name) {
#if RAV_WINDOWS
    std::string value(64, 0);
    auto length = GetEnvironmentVariableA(name, value.data(), static_cast<DWORD>(value.size()));
    if (length == 0) {
        return {};
    }
    if (length > value.size() - 1) {
        value.resize(length, 0);
        length = GetEnvironmentVariableA(name, value.data(), static_cast<DWORD>(value.size()));
        assert(length == value.size() - 1);
    }
    value.resize(length);
    return value;
#else
    if (auto* value = std::getenv(name)) {
        return value;
    }
    return {};
#endif
}

}  // namespace rav
