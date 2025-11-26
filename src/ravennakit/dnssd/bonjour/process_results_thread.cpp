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

#include "ravennakit/dnssd/bonjour/bonjour_process_results_thread.hpp"

#include "ravennakit/core/assert.hpp"
#include "ravennakit/core/log.hpp"
#include "ravennakit/core/platform/windows/socket_event.hpp"

#include <chrono>

#if RAV_HAS_APPLE_DNSSD

rav::dnssd::ProcessResultsThread::~ProcessResultsThread() {
    stop();
}

void rav::dnssd::ProcessResultsThread::start(DNSServiceRef service_ref) {
    if (is_running()) {
        RAV_LOG_ERROR("Thread is already running");
        return;
    }

    const int service_fd = DNSServiceRefSockFD(service_ref);

    if (service_fd < 0) {
        RAV_THROW_EXCEPTION("Invalid file descriptor");
    }

    future_ = std::async(std::launch::async, [this, service_ref, service_fd] {
        run(service_ref, service_fd);
    });
}

void rav::dnssd::ProcessResultsThread::stop() {
    using namespace std::chrono_literals;

    if (future_.valid()) {
        std::lock_guard guard(lock_);

    #if RAV_POSIX
        constexpr char x = 'x';
        if (pipe_.write(&x, 1) != 1) {
            RAV_LOG_ERROR("Failed to signal thread to stop");
        }
    #else
        event_.signal();
    #endif

        const auto status = future_.wait_for(1000ms);
        if (status == std::future_status::ready) {
        } else if (status == std::future_status::timeout) {
            RAV_LOG_ERROR("Failed to stop thread, proceeding anyway.");
        } else {
            RAV_LOG_ERROR("Failed to stop thread, proceeding anyway.");
        }

        future_ = {};
    }
}

bool rav::dnssd::ProcessResultsThread::is_running() {
    using namespace std::chrono_literals;
    std::lock_guard guard(lock_);
    if (future_.valid()) {
        return future_.wait_for(0s) != std::future_status::ready;
    }
    return false;
}

std::lock_guard<std::mutex> rav::dnssd::ProcessResultsThread::lock() {
    return std::lock_guard(lock_);
}

void rav::dnssd::ProcessResultsThread::run(DNSServiceRef service_ref, const int service_fd) {
    RAV_LOG_TRACE("Start DNS-SD processing thread");

    #if RAV_POSIX

    const auto signal_fd = pipe_.read_fd();
    const auto max_fd = std::max(service_fd_, signal_fd);

    constexpr auto max_attempts = 10;
    auto failed_attempts = 0;

    while (true) {
        try {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(signal_fd, &readfds);
            FD_SET(service_fd, &readfds);

            const int result = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);

            if (result < 0) {
                if (++failed_attempts >= max_attempts) {
                    RAV_LOG_ERROR("Select error: {}. Max failed attempts reached, exiting thread.", strerror(errno));
                    break;
                }
                RAV_LOG_ERROR("Select error: {}", strerror(errno));
            } else {
                failed_attempts = 0;
            }

            if (result == 0) {
                RAV_LOG_ERROR("Unexpected timeout. Continue processing.");
                continue;
            }

            if (FD_ISSET(signal_fd, &readfds)) {
                char x;
                pipe_.read(&x, 1);
                if (x == 'x') {
                    RAV_LOG_TRACE("Received signal to stop, exiting thread.");
                    break;  // Stop the thread.
                }
                RAV_LOG_TRACE("Received signal to stop, but with unexpected data.");
                break;  // Stop the thread.
            }

            // Check if the DNS-SD fd is ready
            if (FD_ISSET(service_fd, &readfds)) {
                // Locking here will make sure that all callbacks are synchronised because they are called in
                // response to DNSServiceProcessResult.
                std::lock_guard guard(lock_);
                DNSSD_THROW_IF_ERROR(DNSServiceProcessResult(service_ref), "Failed to process dns service results");
            }
        } catch (const std::exception& e) {
            RAV_LOG_CRITICAL("Uncaught exception on process_results_thread: {}", e.what());
        }
    }

    #elif RAV_WINDOWS

    windows::socket_event socket_event;
    socket_event.associate(service_fd);

    while (true) {
        try {
            HANDLE events[2];
            events[0] = socket_event.get();
            events[1] = event_.get();

            DWORD result = WSAWaitForMultipleEvents(2, events, FALSE, WSA_INFINITE, FALSE);

            if (result == WSA_WAIT_EVENT_0) {
                // Handle the socket event
                socket_event.reset_event();
                // Locking here will make sure that all callbacks are synchronised because they are called in
                // response to DNSServiceProcessResult.
                std::lock_guard guard(lock_);

                DNSSD_THROW_IF_ERROR(DNSServiceProcessResult(service_ref), "Failed to process dns service results");
            } else if (result == WSA_WAIT_EVENT_0 + 1) {
                RAV_LOG_TRACE("Received signal to stop, exiting thread.");
                break;  // Stop the thread.
            } else if (result == WSA_WAIT_FAILED) {
                RAV_LOG_ERROR("WSAWaitForMultipleEvents failed: {}", WSAGetLastError());
                break;
            } else {
                RAV_LOG_ERROR("WSAWaitForMultipleEvents returned unexpected result: {}", result);
                break;
            }
        } catch (const std::exception& e) {
            RAV_LOG_CRITICAL("Uncaught exception on process_results_thread: {}", e.what());
        }
    }

    #endif

    RAV_LOG_TRACE("Stop DNS-SD processing thread");
}

#endif
