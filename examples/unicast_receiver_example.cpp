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
#include "ravennakit/rtp/rtp_receiver.hpp"

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

    rav::rtp_receiver receiver(loop);
    receiver.on<rav::rtp_packet_event>([](const rav::rtp_packet_event& event,
                                          [[maybe_unused]] rav::rtp_receiver& recv) {
        fmt::println("{}", event.packet.to_string());
    });

    if (auto result = receiver.start_receiving_unicast(argv[1], port); result != 0) {
        fmt::println(stderr, "Error: {}", result);
        return result;
    }

    const auto signal = loop->resource<uvw::signal_handle>();
    signal->on<uvw::signal_event>([&receiver, &signal](const uvw::signal_event&, uvw::signal_handle&) {
        receiver.stop();
        signal->close(); // Need to close ourselves, otherwise the loop will not stop.
    });
    signal->start(SIGTERM);

    return loop->run();
}
