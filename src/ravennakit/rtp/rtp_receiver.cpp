/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/rtp/rtp_receiver.hpp"

#include <fmt/core.h>

#include "ravennakit/rtp/rtcp_packet_view.hpp"
#include "ravennakit/rtp/rtp_packet_view.hpp"

rav::rtp_receiver::~rtp_receiver() {
    if (rtp_socket_ != nullptr) {
        rtp_socket_->close();
    }

    if (rtcp_socket_ != nullptr) {
        rtcp_socket_->close();
    }
}

int rav::rtp_receiver::start_receiving_unicast(const std::string& address, const uint16_t port, udp_flags opts) {
    auto rtp_socket = loop_->resource<uvw::udp_handle>();
    if (rtp_socket == nullptr) {
        return 1;
    }

    rtp_socket->on<uvw::udp_data_event>(
        [this](const uvw::udp_data_event& event, [[maybe_unused]] uvw::udp_handle& handle) {
            const rav::rtp_packet_view rtp_packet(reinterpret_cast<const uint8_t*>(event.data.get()), event.length);
            publish(rtp_packet_event {rtp_packet});
        }
    );

    rtp_socket->on<uvw::error_event>([](const uvw::error_event& event, uvw::udp_handle& handle) {
        fmt::print(stderr, "Error: {}\n", event.what());
        handle.close();
    });

    {
        const uvw::error_event error(rtp_socket->bind(address, port, opts));
        if (error) {
            return error.code();
        }
    }

    auto rtcp_socket = loop_->resource<uvw::udp_handle>();
    if (rtcp_socket == nullptr) {
        return 1;
    }

    rtcp_socket->on<uvw::udp_data_event>([](const uvw::udp_data_event& event,
                                            [[maybe_unused]] uvw::udp_handle& handle) {
        const rav::rtcp_packet_view rtcp_packet(reinterpret_cast<const uint8_t*>(event.data.get()), event.length);
        fmt::println("{}", rtcp_packet.to_string());
    });

    rtcp_socket->on<uvw::error_event>([](const uvw::error_event& event, uvw::udp_handle& handle) {
        fmt::print(stderr, "Error: {}\n", event.what());
        handle.close();
    });

    {
        const uvw::error_event error(rtcp_socket->bind(address, port + 1, opts));
        if (error) {
            return error.code();
        }
    }

    rtp_socket->recv();
    rtcp_socket->recv();

    rtp_socket_ = std::move(rtp_socket);
    rtcp_socket_ = std::move(rtcp_socket);

    return 0;
}
