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

#include <asio.hpp>
#include <iostream>

#include "../include/ravenna-sdk/rtp/RtpHeaderView.hpp"

constexpr short multicast_port = 5004;

class Receiver {
  public:
    Receiver(
        asio::io_context& io_context,
        const asio::ip::address& listen_address,
        const asio::ip::address& multicast_address,
        const asio::ip::address& interface_address
    ) : socket_(io_context) {
        // Create the socket so that multiple may be bound to the same address.
        const asio::ip::udp::endpoint listen_endpoint(listen_address, multicast_port);
        socket_.open(listen_endpoint.protocol());
        socket_.set_option(asio::ip::udp::socket::reuse_address(true));
        socket_.bind(listen_endpoint);

        // Join the multicast group.
        socket_.set_option(asio::ip::multicast::join_group(multicast_address.to_v4(), interface_address.to_v4()));

        do_receive();
    }

  private:
    void do_receive() {
        socket_.async_receive_from(
            asio::buffer(data_),
            sender_endpoint_,
            [this](std::error_code const ec, const std::size_t length) {
                if (!ec) {
                    const rav::RtpHeaderView header(data_.data(), length);
                    fmt::println("{}", header.to_string());
                    do_receive();
                }
            }
        );
    }

    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint sender_endpoint_;
    std::array<uint8_t, 2048> data_ {};
};

int main(int const argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: receiver <listen_address> <multicast_address> <interface_address>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 239.1.15.51 192.168.15.52\n";
            return 1;
        }

        asio::io_context io_context;
        Receiver r(
            io_context,
            asio::ip::make_address(argv[1]),
            asio::ip::make_address(argv[2]),
            asio::ip::make_address(argv[3])
        );
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
