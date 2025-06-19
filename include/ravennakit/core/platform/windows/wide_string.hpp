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
std::string wide_string_to_string(PWCHAR wchar_str) {
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
        RAV_ERROR("Failed to convert wide string to UTF-8: {}", GetLastError());
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
        RAV_ERROR("Failed to convert wide string to UTF-8: {}", GetLastError());
        return {};
    }

    return output;
}

}  // namespace rav

#endif
