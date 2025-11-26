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

#include <catch2/catch_all.hpp>

TEST_CASE("rav::AsioTimer") {
    SECTION("Once") {
        boost::asio::io_context io_context;
        rav::AsioTimer timer(io_context);

        bool callback_called = false;
        timer.once(std::chrono::milliseconds(100), [&] {
            callback_called = true;
        });

        io_context.run();
        CHECK(callback_called);
    }

    SECTION("Repeatedly") {
        boost::asio::io_context io_context;
        rav::AsioTimer timer(io_context);

        int callback_count = 0;
        timer.start(std::chrono::milliseconds(100), [&] {
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
                rav::AsioTimer timer(io_context);
                timer.start(std::chrono::milliseconds(i), [&] {
                    callback_count++;
                });
                creation_count++;
            });
        }

        for (auto i = 0; i < times; ++i) {
            boost::asio::post(io_context, [&io_context, &callback_count, &creation_count, i] {
                rav::AsioTimer timer(io_context);
                timer.once(std::chrono::milliseconds(i), [&] {
                    callback_count++;
                });
                creation_count++;
            });
        }

        io_context.run();

        CHECK(callback_count == 0);
        CHECK(creation_count == times * 2);
    }

    SECTION("Create and destroy multithreaded") {
        static constexpr auto times = 1'000;
        boost::asio::io_context io_context;

        // User a timer to keep the io_context alive, and as a timeout mechanism.
        rav::AsioTimer timer(io_context);
        timer.once(std::chrono::milliseconds(100'000), [&] {
            abort();  // Timeout
        });

        std::thread runner([&io_context] {
            io_context.run();
        });

        int callback_count = 0;
        int creation_count = 0;
        for (auto i = 0; i < times; ++i) {
            boost::asio::post(io_context, [&io_context, &callback_count, &creation_count, i] {
                rav::AsioTimer t(io_context);
                t.start(std::chrono::milliseconds(i), [&] {
                    callback_count++;
                });
                creation_count++;
            });
        }

        for (auto i = 0; i < times; ++i) {
            boost::asio::post(io_context, [&io_context, &callback_count, &creation_count, i] {
                rav::AsioTimer t(io_context);
                t.once(std::chrono::milliseconds(i), [&] {
                    callback_count++;
                });
                creation_count++;
            });
        }

        timer.stop();
        runner.join();

        CHECK(callback_count == 0);
        CHECK(creation_count == times * 2);
    }

    SECTION("Start and stop multithreaded") {
        static constexpr auto times = 1'000;
        boost::asio::io_context io_context;

        // User a timer to keep the io_context alive, and as a timeout mechanism.
        rav::AsioTimer timer(io_context);
        timer.once(std::chrono::milliseconds(100'000), [&] {
            abort();  // Timeout
        });

        std::thread runner([&io_context] {
            io_context.run();
        });

        for (auto i = 0; i < times; ++i) {
            i % 2 == 0 ? timer.start(std::chrono::milliseconds(i), [] {}) : timer.stop();
        }

        timer.stop();
        runner.join();
    }
}
