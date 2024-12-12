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

#if RAV_APPLE

    #include "cf_type.hpp"

namespace rav {

/**
 * An RAII wrapper around CFStringRef.
 */
class cf_string: public cf_type<CFStringRef> {
  public:
    /**
     * Constructs a new cf_string from an existing CFStringRef. The string_ref is assumed to be already retained, and
     * this class will assume ownership.
     * @param string_ref The CFStringRef to manage.
     * @param retain True to retain the given string_ref, false otherwise (where it is assumed that the string_ref has
     * been retained before and needs to be placed under RAII).
     */
    explicit cf_string(const CFStringRef string_ref, const bool retain = true) : cf_type(string_ref, retain) {}

    /**
     * Constructs a new std::string from given CFStringRef.
     * @param cf_string_ref The CFStringRef to convert.
     * @return The string as a std::string. If an error occurs, an empty string is returned.
     */
    static std::string to_string(const CFStringRef& cf_string_ref) {
        if (cf_string_ref == nullptr) {
            return {};
        }

        // First try to get the string as a C string which is cheaper because it doesn't allocate.
        // Note: for this particular case there is not really a benefit of using this function, because we're going to
        // allocate a new string anyway. However, I prefer to use the 'best practice' here to educate myself properly in
        // the future.
        const auto* c_string = CFStringGetCStringPtr(cf_string_ref, kCFStringEncodingUTF8);

        if (c_string != nullptr) {
            return c_string;
        }

        // If the above didn't return anything we have to fall back and use CFStringGetCString.
        const CFIndex length = CFStringGetLength(cf_string_ref);
        const CFIndex max_size =
            CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;  // Include \0 termination

        std::string output(static_cast<unsigned long>(max_size), 0);
        if (!CFStringGetCString(cf_string_ref, output.data(), max_size, kCFStringEncodingUTF8)) {
            return {};
        }

        // Remove all null-termination characters from output
        output.erase(std::find(output.begin(), output.end(), '\0'), output.end());

        return output;
    }

    /**
     * @returns The string as a std::string.
     */
    [[nodiscard]] std::string to_string() const {
        return to_string(get());
    }
};

}  // namespace rav

#endif
