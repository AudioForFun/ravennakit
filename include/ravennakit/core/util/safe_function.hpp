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
#include <functional>

namespace rav {

/**
 * A simple callback function wrapper that can be used to store and call a function with a specific signature.
 * It provides a default no-op function if no function is provided, making it possible to call without checking for
 * null.
 */
template<typename Signature>
class SafeFunction;

template<typename R, typename... Args>
class SafeFunction<R(Args...)> {
  public:
    using FuncType = std::function<R(Args...)>;

    /**
     * Constructs an empty callback function.
     */
    SafeFunction() : function_(noop()) {}

    /**
     * Constructs a callback function with the given function.
     * @param f The function to be called.
     */
    explicit SafeFunction(FuncType f) : function_(f ? std::move(f) : noop()) {}

    /**
     * Calls the stored function with the given arguments.
     * @param args The arguments to be passed to the function.
     * @return The result of the function call.
     */
    R operator()(Args... args) const {
        return function_(std::forward<Args>(args)...);
    }

    /**
     * Sets the function to be called.
     * @param f The function to be called.
     */
    void set(FuncType f) {
        function_ = f ? std::move(f) : noop();
    }

    /**
     * Sets the function to be called.
     * @param f The function to be called.
     */
    SafeFunction& operator=(FuncType f) {
        function_ = f ? std::move(f) : noop();
        return *this;
    }

    /**
     * Resets the function to a no-op function.
     */
    void reset() {
        function_ = noop();
    }

  private:
    static FuncType noop() {
        if constexpr (std::is_void_v<R>) {
            return [](Args...) {};
        } else {
            return [](Args...) -> R {
                return R {};
            };
        }
    }

    FuncType function_;
};

}  // namespace rav
