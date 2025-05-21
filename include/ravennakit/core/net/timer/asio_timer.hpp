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

#include <boost/asio.hpp>

namespace rav {

/**
 * Wrapper around boost::asio::steady_timer to provide a simple way of creating a timer without thinking about the
 * lifetimes.
 */
class AsioTimer {
  public:
    using Callback = std::function<void(const boost::system::error_code& ec)>;

    explicit AsioTimer(boost::asio::io_context& io_context);
    ~AsioTimer();

    AsioTimer(AsioTimer&& other) noexcept;
    AsioTimer& operator=(AsioTimer&& other) noexcept;

    AsioTimer(const AsioTimer&) = delete;
    AsioTimer& operator=(const AsioTimer&) = delete;

    /**
     * Fires the callback once after the given duration.
     * The timer will be stopped before starting a new one.
     * @param duration The duration to wait before firing the callback.
     * @param callback The callback to fire after the duration.
     */
    void once(std::chrono::milliseconds duration, Callback callback) const;

    /**
     * Fires the callback after the given duration. The callback will be fired repeatedly until stopped.
     * The timer will be stopped before starting a new one.
     * Safe to call from any thread.
     * @param duration The duration to wait before firing the callback.
     * @param callback The callback to fire after the duration.
     */
    void start(std::chrono::milliseconds duration, Callback callback) const;

    /**
     * Stops the timer and cancels any pending callbacks. After this call returns, no more callbacks will be fired.
     * Can be called from any thread. The work is scheduled on the io_context thread.
     */
    void stop() const;

  private:
    class Impl;
    std::shared_ptr<Impl> impl_;
};

}  // namespace rav
