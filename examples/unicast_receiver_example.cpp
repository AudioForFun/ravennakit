/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include <fmt/core.h>

#include <iostream>
#include <uvw.hpp>

#include "../include/ravennakit/rtp/rtp_packet_view.hpp"

constexpr short port = 5004;

int main(int const argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: receiver <listen_address>\n";
        std::cerr << "  For IPv4, try:\n";
        std::cerr << "    receiver 0.0.0.0\n";
        return 1;
    }

    const auto loop = uvw::loop::create();
    if (loop == nullptr) {
        return 1;
    }

    const auto socket = loop->resource<uvw::udp_handle>();

    if (socket == nullptr) {
        return 1;
    }

    int count = 100;
    socket->on<uvw::udp_data_event>([&count](const uvw::udp_data_event& event, uvw::udp_handle& handle) {
        const rav::rtp_packet_view header(reinterpret_cast<const uint8_t*>(event.data.get()), event.length);
        fmt::println("{}", header.to_string());
        if (--count <= 0) {
            handle.close();
        }
    });

    socket->on<uvw::error_event>([](const uvw::error_event& event, uvw::udp_handle& handle) {
        fmt::print(stderr, "Error: {}\n", event.what());
        handle.close();
    });

    if (socket->bind(argv[1], port) != 0) {
        return 1;
    }

    socket->recv();

    return loop->run();
}
