/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/core/net/timer/asio_timer.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("AsioTimer") {
    SECTION("once") {
        boost::asio::io_context io_context;
        const rav::AsioTimer timer(io_context);

        bool callback_called = false;
        timer.once(std::chrono::milliseconds(100), [&](const boost::system::error_code&) {
            callback_called = true;
        });

        io_context.run();
        CHECK(callback_called);
    }

    SECTION("Repeatedly") {
        boost::asio::io_context io_context;
        const rav::AsioTimer timer(io_context);

        int callback_count = 0;
        timer.start(std::chrono::milliseconds(100), [&](const boost::system::error_code&) {
            ++callback_count;
            if (callback_count == 3) {
                timer.stop();
            }
        });

        io_context.run();
        CHECK(callback_count == 3);
    }

    SECTION("Create and destroy") {
        static constexpr auto times = 1'000;
        boost::asio::io_context io_context;

        int callback_count = 0;
        int creation_count = 0;
        for (auto i = 0; i < times; ++i) {
            boost::asio::post(io_context, [&io_context, &callback_count, &creation_count, i] {
                const rav::AsioTimer timer(io_context);
                timer.start(std::chrono::milliseconds(i), [&](const boost::system::error_code&) {
                    callback_count++;
                });
                creation_count++;
            });
        }

        for (auto i = 0; i < times; ++i) {
            boost::asio::post(io_context, [&io_context, &callback_count, &creation_count, i] {
                const rav::AsioTimer timer(io_context);
                timer.once(std::chrono::milliseconds(i), [&](const boost::system::error_code&) {
                    callback_count++;
                });
                creation_count++;
            });
        }

        io_context.run();

        CHECK(callback_count == 0);
        CHECK(creation_count == times * 2);
    }
}
