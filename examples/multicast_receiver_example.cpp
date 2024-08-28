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
#include <uvw/udp.h>

#include <iostream>

#include "../include/ravennakit/rtp/RtpPacketView.hpp"

constexpr short port = 5004;

int main(int const argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: receiver <listen_address> <multicast_address> <interface_address>\n";
        std::cerr << "  For IPv4, try:\n";
        std::cerr << "    receiver 0.0.0.0 239.1.15.51 192.168.15.52\n";
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

    int count = 10;
    socket->on<uvw::udp_data_event>([&count](const uvw::udp_data_event& event, uvw::udp_handle& handle) {
        const rav::RtpPacketView header(reinterpret_cast<const uint8_t*>(event.data.get()), event.length);
        fmt::println("{}", header.to_string());
        if (--count <= 0) {
            handle.close();
        }
    });

    socket->on<uvw::error_event>([](const uvw::error_event& event, uvw::udp_handle& handle) {
        fmt::print(stderr, "Error: {}\n", event.what());
        handle.close();
    });

    const auto result = socket->bind(argv[1], port, uvw::details::uvw_udp_flags::REUSEADDR);
    if (result != 0) {
        const uvw::error_event event(result);
        fmt::println(stderr, "{}: {}", event.name(), event.what());
        return result;
    }

    socket->multicast_membership(argv[2], argv[3], uvw::udp_handle::membership::JOIN_GROUP);

    socket->recv();

    return loop->run();
}
