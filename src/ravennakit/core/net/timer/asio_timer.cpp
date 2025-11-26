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

#include "ravennakit/core/net/timer/asio_timer.hpp"

#include "ravennakit/core/log.hpp"

rav::AsioTimer::AsioTimer(boost::asio::io_context& io_context) : timer_(io_context) {}

rav::AsioTimer::~AsioTimer() {
    stop();
}

void rav::AsioTimer::once(const std::chrono::milliseconds duration, TimerCallback cb) {
    start(duration, std::move(cb), false);
}

void rav::AsioTimer::start(const std::chrono::milliseconds duration, TimerCallback cb, const bool repeating) {
    timer_.cancel();
    callback_ = std::move(cb);
    duration_ = duration;
    repeating_ = repeating;
    wait();
}

void rav::AsioTimer::stop() {
    timer_.cancel();
    callback_ = nullptr;
    repeating_ = false;
}

void rav::AsioTimer::wait() {
    timer_.expires_after(duration_);
    timer_.async_wait([this](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
            return;
        }

        if (ec) {
            RAV_LOG_ERROR("Timer error: {}", ec.message());
            return;
        }

        if (!callback_) {
            return;
        }
        callback_();
        if (repeating_) {
            wait();
            return;
        }
        callback_ = nullptr;
    });
}
