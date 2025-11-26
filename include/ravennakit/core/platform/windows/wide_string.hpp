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

#include "ravennakit/core/platform.hpp"
#include "ravennakit/core/log.hpp"
#if RAV_WINDOWS

    #include <string>
    #include <windows.h>

namespace rav {

/**
 * @brief Convert a wide string to a UTF-8 string.
 * @param wchar_str The wide string to convert.
 * @return The UTF-8 string.
 */
inline std::string wide_string_to_string(PWCHAR wchar_str) {
    // https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte

    if (wchar_str == nullptr) {
        return {};
    }

    // Determine the length of the wide string
    int wchar_length = lstrlenW(wchar_str);

    // Determine the size of the output string
    int size_needed = WideCharToMultiByte(
        CP_UTF8,       // Code page: UTF-8
        0,             // Conversion flags
        wchar_str,     // Input wide string
        wchar_length,  // Length of the input string
        nullptr,       // Output buffer (nullptr to calculate size)
        0,             // Size of the output buffer
        nullptr,       // Default char (not used for UTF-8)
        nullptr        // UsedDefaultChar flag (not used for UTF-8)
    );

    if (size_needed == 0) {
        RAV_LOG_ERROR("Failed to convert wide string to UTF-8: {}", GetLastError());
        return {};
    }

    // Allocate a buffer for the converted string
    std::string output(size_needed, '\0');

    // Perform the actual conversion
    auto result = WideCharToMultiByte(
        CP_UTF8,        // Code page: UTF-8
        0,              // Conversion flags
        wchar_str,      // Input wide string
        wchar_length,   // Length of the input string
        output.data(),  // Output buffer
        size_needed,    // Size of the output buffer
        nullptr,        // Default char (not used for UTF-8)
        nullptr         // UsedDefaultChar flag (not used for UTF-8)
    );

    if (result == 0) {
        RAV_LOG_ERROR("Failed to convert wide string to UTF-8: {}", GetLastError());
        return {};
    }

    return output;
}

}  // namespace rav

#endif
