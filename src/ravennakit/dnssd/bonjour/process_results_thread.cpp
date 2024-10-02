/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/dnssd/bonjour/process_results_thread.hpp"

#include "ravennakit/core/assert.hpp"
#include "ravennakit/core/log.hpp"

#if RAV_HAS_APPLE_DNSSD

void rav::dnssd::process_results_thread::start(DNSServiceRef service_ref) {
    if (is_running()) {
        RAV_ERROR("Thread is already running");
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

void rav::dnssd::process_results_thread::stop() {
#if RAV_POSIX
    using namespace std::chrono_literals;

    if (future_.valid()) {
        std::lock_guard guard(lock_);

        constexpr char x = 'x';
        if (pipe_.write(&x, 1) != 1) {
            RAV_ERROR("Failed to signal thread to stop");
        }
        const auto status = future_.wait_for(1000ms);
        if (status == std::future_status::ready) {
            RAV_TRACE("Thread stopped");
        } else if (status == std::future_status::timeout) {
            RAV_ERROR("Failed to stop thread, proceeding anyway.");
        } else {
            RAV_ERROR("Failed to stop thread, proceeding anyway.");
        }

        future_ = {};
    }
#endif
}

bool rav::dnssd::process_results_thread::is_running() {
    using namespace std::chrono_literals;
    std::lock_guard guard(lock_);
    if (future_.valid()) {
        return future_.wait_for(0s) != std::future_status::ready;
    }
    return false;
}

std::lock_guard<std::mutex> rav::dnssd::process_results_thread::lock() {
    return std::lock_guard(lock_);
}

void rav::dnssd::process_results_thread::run(DNSServiceRef service_ref, const int service_fd) {
#if RAV_POSIX
    const auto signal_fd = pipe_.read_fd();
    const auto max_fd = std::max(service_fd_, signal_fd);

    RAV_TRACE("Start DNS-SD processing thread");

    constexpr auto max_attempts = 10;
    auto failed_attempts = 0;

    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(signal_fd, &readfds);
        FD_SET(service_fd, &readfds);

        const int result = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);

        if (result < 0) {
            if (++failed_attempts >= max_attempts) {
                RAV_ERROR("Select error: {}. Max failed attempts reached, exiting thread.", strerror(errno));
            } else {
                RAV_ERROR("Select error: {}", strerror(errno));
            }
        } else {
            failed_attempts = 0;
        }

        if (result == 0) {
            RAV_ERROR("Unexpected timeout. Continue processing.");
            continue;
        }

        if (FD_ISSET(signal_fd, &readfds)) {
            char x;
            pipe_.read(&x, 1);
            if (x == 'x') {
                RAV_TRACE("Received signal to stop, exiting thread.");
                break;  // Stop the thread.
            }
            RAV_TRACE("Received signal to stop, but with unexpected data.");
            break;  // Stop the thread.
        }

        // Check if the DNS-SD fd is ready
        if (FD_ISSET(service_fd, &readfds)) {
            // Locking here will make sure that all callbacks are synchronised because they are called in
            // response to DNSServiceProcessResult.
            std::lock_guard guard(lock_);
            DNSSD_LOG_IF_ERROR(DNSServiceProcessResult(service_ref));
        }
    }

    RAV_TRACE("Stop DNS-SD processing thread");
#endif
}

#endif
