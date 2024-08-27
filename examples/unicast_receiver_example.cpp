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

#include "../include/ravennakit/rtp/RtpPacketView.hpp"
#include "ravennakit/event/IoContextRunner.hpp"
#include "ravennakit/rtp/RtcpPacketView.hpp"

constexpr short port = 5004;

class Receiver {
  public:
    Receiver(asio::io_context& io_context, const asio::ip::address& listen_address) :
        rtp_socket_(io_context), rtcp_socket_(io_context) {
        {
            // RTP
            const asio::ip::udp::endpoint listen_endpoint(listen_address, port);
            rtp_socket_.open(listen_endpoint.protocol());
            rtp_socket_.set_option(asio::ip::udp::socket::reuse_address(true));
            rtp_socket_.bind(listen_endpoint);
        }

        {
            // RTCP
            const asio::ip::udp::endpoint listen_endpoint(listen_address, port + 1);
            rtcp_socket_.open(listen_endpoint.protocol());
            rtcp_socket_.set_option(asio::ip::udp::socket::reuse_address(true));
            rtcp_socket_.bind(listen_endpoint);
        }

        receive_rtp();
        receive_rtcp();
    }

  private:
    void receive_rtp() {
        rtp_socket_.async_receive_from(
            asio::buffer(rtp_data_), rtp_endpoint_,
            [this](std::error_code const ec, const std::size_t length) {
                if (!ec) {
                    const rav::RtpPacketView header(rtp_data_.data(), length);
                    // fmt::println("{}", header.to_string());
                    receive_rtp();
                } else {
                    fmt::println("RTP Receive error. Exit.");
                }
            }
        );
    }

    void receive_rtcp() {
        rtcp_socket_.async_receive_from(
            asio::buffer(rtcp_data_), rtcp_endpoint_,
            [this](std::error_code const ec, const std::size_t length) {
                if (!ec) {
                    const rav::RtcpPacketView packet(rtcp_data_.data(), length);
                    fmt::println("{}", packet.to_string());
                    receive_rtcp();
                } else {
                    fmt::println("RTCP Receive error. Exit.");
                }
            }
        );
    }

    asio::ip::udp::socket rtp_socket_;
    asio::ip::udp::socket rtcp_socket_;
    asio::ip::udp::endpoint rtp_endpoint_;
    asio::ip::udp::endpoint rtcp_endpoint_;
    std::array<uint8_t, 2048> rtp_data_ {};
    std::array<uint8_t, 2048> rtcp_data_ {};
};

int main(int const argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: receiver <listen_address>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0\n";
            return 1;
        }

        IoContextRunner runner;
        Receiver r(runner.io_context(), asio::ip::make_address(argv[1]));
        runner.run_async();

        std::cout << "Hit return to exit\n";
        std::getchar();

        runner.stop();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
