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

#include "ravennakit/core/assert.hpp"

#include <boost/asio.hpp>

namespace rav {

/**
 * A timer that uses boost::asio::steady_timer to provide a simple way of creating a timer without thinking about the
 * lifetimes.
 */
class AsioTimer {
  public:
    using TimerCallback = std::function<void()>;

    explicit AsioTimer(boost::asio::io_context& io_context);
    ~AsioTimer();

    /**
     * Fires the callback once after the given duration.
     * The timer will be stopped before starting a new one.
     * @param duration The duration to wait before firing the callback.
     * @param cb The callback to fire after the duration.
     */
    void once(std::chrono::milliseconds duration, TimerCallback cb);

    /**
     * Fires the callback after the given duration. The callback will be fired repeatedly until stopped.
     * The timer will be stopped before starting a new one.
     * Safe to call from any thread.
     * @param duration The duration to wait before firing the callback.
     * @param cb The callback to fire after the duration.
     * @param repeating If true, the timer will repeat the callback after each duration.
     */
    void start(std::chrono::milliseconds duration, TimerCallback cb, bool repeating = true);

    /**
     * Stops the timer and cancels any pending callbacks. After this call returns, no more callbacks will be fired.
     * Can be called from any thread. The work is scheduled on the io_context thread.
     */
    void stop();

  private:
    boost::asio::steady_timer timer_;
    TimerCallback callback_;
    bool repeating_ = false;
    std::chrono::milliseconds duration_ = std::chrono::seconds(0);

    void wait();
};

}  // namespace rav
