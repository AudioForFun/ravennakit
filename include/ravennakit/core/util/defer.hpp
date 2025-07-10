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

#include "../log.hpp"

#include <functional>
#include <vector>

namespace rav {

/**
 * A class that holds a rollback function, calling it upon destruction unless a commit is made.
 * This serves as an alternative to the 'goto cleanup' pattern in C, providing a mechanism to roll back changes
 * if subsequent operations fail. The class is exception-safe if given function is exception safe.
 */
template<class Fn>
class Defer {
  public:
    /**
     * Constructs a rollback object with an initial rollback function.
     * @param rollback_function The function to execute upon destruction if not committed.
     */
    explicit Defer(Fn&& rollback_function) : rollback_function_(std::move(rollback_function)) {}

    /**
     * Destructor that executes all registered rollback functions if not committed.
     */
    ~Defer() {
        if (call_upon_destruction_) {
            try {
                rollback_function_();
            } catch (const std::exception& e) {
                RAV_ERROR("Exception caught: {}", e.what());
            }
        }
    }

    Defer(const Defer&) = delete;
    Defer& operator=(const Defer&) = delete;

    Defer(Defer&&) = delete;
    Defer& operator=(Defer&&) = delete;

    /**
     * Clears the stored function (if there is one) without calling it.
     */
    void reset() {
        call_upon_destruction_ = false;
    }

  private:
    Fn rollback_function_;
    bool call_upon_destruction_ = true;
};

}  // namespace rav
