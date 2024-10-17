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
#include "ravennakit/rtsp/rtsp_client.hpp"
#include "ravennakit/rtsp/rtsp_server.hpp"

#include <catch2/catch_all.hpp>
#include <thread>

TEST_CASE("rtsp_server", "[rtsp_server]") {
    SECTION("Port") {
        asio::io_context io_context;
        std::vector<std::thread> threads;

        for (auto i = 0; i < 10; i++) {
            threads.emplace_back([&io_context] {
                io_context.run();
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        SECTION("Any port") {
            rav::rtsp_server server(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v6(), 0));
            REQUIRE(server.port() != 0);
            server.close();
        }

        SECTION("Specific port") {
            rav::rtsp_server server(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v6(), 555));
            REQUIRE(server.port() == 555);
            server.close();
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    SECTION("Create and destroy using io_context_runner") {
        rav::io_context_runner runner(8);

        for (int i = 0; i < 10; i++) {
            rav::rtsp_server server(runner.io_context(), asio::ip::tcp::endpoint(asio::ip::tcp::v6(), 0));
            server.close();
        }

        runner.join();
    }

    SECTION("Create and destroy using io_context") {
        asio::io_context io_context;
        std::vector<std::thread> threads;

        for (auto i = 0; i < 8; i++) {
            threads.emplace_back([&io_context] {
                io_context.run();
            });
        }

        for (int i = 0; i < 10; i++) {
            rav::rtsp_server server(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v6(), 0));
            server.close();
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }
}

TEST_CASE("rtsp_server | send with single client", "[rtsp_server]") {
#if RAV_ENABLE_SPDLOG
    spdlog::set_level(spdlog::level::trace);
#endif

    rav::io_context_runner runner(8);
    rav::rtsp_server server(runner.io_context(), asio::ip::tcp::endpoint(asio::ip::tcp::v6(), 0));

    const auto port = server.port();
    REQUIRE(port != 0);

    rav::rtsp_client client(runner.io_context());
    client.on<rav::rtsp::connect_event>([](const rav::rtsp::connect_event&, rav::rtsp_client& c) {
        RAV_INFO("Connected, send DESCRIBE request");
        c.describe("/by-id/13");
    });
    client.on<rav::rtsp_request>([](const rav::rtsp_request& request, rav::rtsp_client&) {
        RAV_INFO("{}\n{}", request.to_debug_string(), rav::string_replace(request.data, "\r\n", "\n"));
    });
    client.on<rav::rtsp_response>([](const rav::rtsp_response& response, rav::rtsp_client&) {
        RAV_INFO("{}\n{}", response.to_debug_string(), rav::string_replace(response.data, "\r\n", "\n"));
    });
    client.connect("::1", port);

    runner.join();
}
