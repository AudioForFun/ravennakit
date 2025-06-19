/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
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
