/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/asio/io_context_runner.hpp"
#include "ravennakit/core/log.hpp"

#include <catch2/catch_all.hpp>
#include <thread>

#include "ravennakit/util/chrono/timeout.hpp"

namespace {
constexpr auto k_default_timeout_seconds_seconds = std::chrono::seconds(5);
}

TEST_CASE("io_context_runner", "[io_context_runner]") {
    SECTION("Run tasks to completion asynchronously") {
        rav::io_context_runner runner;
        size_t expected_total = 0;
        std::atomic<size_t> total = 0;

        for (size_t i = 0; i < 10000; i++) {
            expected_total += i;
            asio::post(runner.io_context(), [&total, i] {
                total.fetch_add(i);
            });
        }

        const rav::util::chrono::timeout timeout(k_default_timeout_seconds_seconds);
        const auto result = timeout.wait_until([&total, &expected_total] {
            return total == expected_total && total != 0;
        });

        if (!result) {
            FAIL("Timeout expired");
        }

        runner.join();

        REQUIRE(expected_total == total);
    }

    SECTION("When calling run_async, the io_context should not stop when no work is posted") {
        rav::io_context_runner runner;

        // Give runner some time to idle
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        asio::post(runner.io_context(), [p = std::move(promise)]() mutable {
            p.set_value();
        });

        if (future.wait_for(std::chrono::seconds(1)) == std::future_status::timeout) {
            FAIL("Timeout expired");
        }

        runner.join();
    }
}
