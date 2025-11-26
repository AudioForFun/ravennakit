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

#include "ravennakit/core/format.hpp"
#include <exception>
#include <string>

/**
 * Define the function name macro if it is not already defined.
 */
#ifndef RAV_FUNCTION
    #if RAV_MACOS
        #define RAV_FUNCTION __PRETTY_FUNCTION__
    #else
        #define RAV_FUNCTION static_cast<const char*>(__FUNCTION__)
    #endif
#endif

#define RAV_THROW_EXCEPTION(...) throw rav::Exception(fmt::format(__VA_ARGS__), __FILE__, __LINE__, RAV_FUNCTION)

namespace rav {

class Exception: public std::exception {
  public:
    explicit Exception(const char* msg, const char* file = nullptr, const int line = -1, const char* function_name = nullptr) :
        error_(msg), file_(file), line_(line), function_name_(function_name) {}

    explicit Exception(std::string msg, const char* file = nullptr, const int line = -1, const char* function_name = nullptr) :
        error_(std::move(msg)), file_(file), line_(line), function_name_(function_name) {}

    /**
     * @returns The error message associated with the error code.
     */
    [[nodiscard]] const char* what() const noexcept override {
        return error_.c_str();
    }

    /**
     * @return The file where the error occurred.
     */
    [[nodiscard]] const char* file() const {
        return file_;
    }

    /**
     * @return The line number where the error occurred.
     */
    [[nodiscard]] int line() const {
        return line_;
    }

    /**
     * @return The name of the function where the error occurred.
     */
    [[nodiscard]] const char* function_name() const {
        return function_name_;
    }

  private:
    std::string error_;
    const char* file_ {};
    int line_ {};
    const char* function_name_ {};
};

}  // namespace rav
