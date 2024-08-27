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

#include <chrono>

namespace rav::util::chrono {

/**
 * Simple timeout class which can be probed for expiration.
 */
template<class Rep, class Period>
class Timeout {
  public:
    explicit Timeout(const std::chrono::duration<Rep, Period>& duration) : duration_(duration) {}

    /**
     * @returns True if the timeout has expired.
     */
    [[nodiscard]] bool expired() const {
        return std::chrono::steady_clock::now() - start_point_ > duration_;
    }

  private:
    const std::chrono::time_point<std::chrono::steady_clock> start_point_ {std::chrono::steady_clock::now()};
    const std::chrono::duration<Rep, Period>& duration_;
};

}  // namespace rav::util::chrono
