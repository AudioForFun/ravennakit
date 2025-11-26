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

#include "bonjour.hpp"
#include "ravennakit/core/platform/posix/pipe.hpp"
#include "ravennakit/core/platform/windows/event.hpp"

#include <future>
#include <thread>

#if RAV_HAS_APPLE_DNSSD

namespace rav::dnssd {

/**
 * Class which processes the results of a DNSServiceRef in a separate thread.
 * Note: at the moment this class is not used because the browser and advertiser classes process the results on an asio
 * io_context.
 */
class ProcessResultsThread {
  public:
    ~ProcessResultsThread();

    /**
     * Starts the thread to process the results of a DNSServiceRef. Thread must not already be running.
     * @param service_ref The DNSServiceRef to process.
     */
    void start(DNSServiceRef service_ref);

    /**
     * Stops the thread. If the thread is not running, nothing happens.
     */
    void stop();

    /**
     * @return True if the thread is running, false otherwise.
     */
    bool is_running();

    /**
     * Locks part of the thread. Used for synchronization of callbacks and the main thread.
     * @return A lock_guard for the mutex.
     */
    std::lock_guard<std::mutex> lock();

  private:
    int service_fd_ {};
    #if RAV_POSIX
    posix::Pipe pipe_;
    #elif RAV_WINDOWS
    windows::event event_;
    #endif
    std::mutex lock_;
    std::future<void> future_;

    void run(DNSServiceRef service_ref, int service_fd);
};

}  // namespace rav::dnssd

#endif
