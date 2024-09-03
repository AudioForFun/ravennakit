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

#include <uvw.hpp>

#include "errors.hpp"

namespace rav {

/**
 * This class holds the result of an operation that may fail. It can hold an error specific to this library, or an error
 * code as returned by libuv (uvw).
 */
class result {
  public:
    result() = default;

    template<class T>
    result& operator=(const T& error) {
        error_.emplace<T>(error);
        return *this;
    }

    template<class T>
    result& operator=(T&& error) {
        error_.emplace<T>(std::forward<T>(error));
        return *this;
    }

    /**
     * Constructs a result object from a library specific error code.
     * @param error The library specific error code.
     */
    explicit result(const error error) : error_(error) {}

    /**
     * Constructs a result object from a uvw error_event.
     * @param error_event The error code returned by libuv
     */
    explicit result(const uvw::error_event error_event) : error_(error_event) {}

    /**
     * @returns True if the result object represents an error, false otherwise.
     */
    [[nodiscard]] bool holds_error() const {
        return !std::holds_alternative<no_error>(error_);
    }

    /**
     * @returns True if the result object represents a success, false otherwise.
     */
    [[nodiscard]] bool is_ok() const {
        return !holds_error();
    }

    /**
     * @returns The error message associated with the error code.
     */
    [[nodiscard]] const char* what() const;

    /**
     * @return The error name associated with the error code.
     */
    [[nodiscard]] const char* name() const;

    /**
     * Tests whether this result holds an error of type T and if so, whether the error matches the given error.
     * @tparam T The error type.
     * @param error The error to test.
     * @returns True if this result object holds the given error, false otherwise.
     */
    template<class T>
    bool holds_error_of_type(const T& error) const {
        if (auto* e = std::get_if<T>(&error_)) {
            return *e == error;
        }
        return false;
    }

  private:
    struct no_error {};

    std::variant<no_error, error, uvw::error_event> error_ {};
};

inline result ok() {
    return {};
}

}  // namespace rav
