/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/log.hpp"
#include "ravennakit/rtp/rtcp_packet_view.hpp"
#include "ravennakit/rtp/rtp_receiver.hpp"

#include "ravennakit/core/subscriber_list.hpp"
#include "ravennakit/rtp/rtp_packet_view.hpp"
#include "ravennakit/core/tracy.hpp"

#include <fmt/core.h>

#include <utility>

class rav::rtp_receiver::impl: public std::enable_shared_from_this<impl> {
  public:
    explicit impl(
        asio::io_context& io_context, const asio::ip::address& interface_address, const uint16_t rtp_port,
        const uint16_t rtcp_port
    ) :
        rtp_socket_(io_context), rtcp_socket_(io_context) {
        // RTP
        asio::ip::udp::endpoint endpoint(interface_address, rtp_port);
        asio::error_code ec;
        rtp_socket_.open(endpoint.protocol(), ec);
        if (ec) {
            RAV_ERROR("Failed to open RTP socket: {}", ec.message());
        }
        rtp_socket_.set_option(asio::ip::udp::socket::reuse_address(true), ec);
        if (ec) {
            RAV_ERROR("Failed to set reuse address option on RTP socket: {}", ec.message());
        }
        rtp_socket_.bind(endpoint, ec);
        if (ec) {
            RAV_ERROR("Failed to bind RTP socket: {}", ec.message());
        }

        // RTCP
        endpoint.port(rtcp_port);
        rtcp_socket_.open(endpoint.protocol(), ec);
        if (ec) {
            RAV_ERROR("Failed to open RTCP socket: {}", ec.message());
        }
        rtcp_socket_.set_option(asio::ip::udp::socket::reuse_address(true), ec);
        if (ec) {
            RAV_ERROR("Failed to set reuse address option on RTCP socket: {}", ec.message());
        }
        rtcp_socket_.bind(endpoint, ec);
        if (ec) {
            RAV_ERROR("Failed to bind RTCP socket: {}", ec.message());
        }
    }

    void start(rtp_receiver& owner) {
        TRACY_ZONE_SCOPED;

        if (&owner == owner_) {
            RAV_WARNING("RTP receiver is already running with the same handler");
            return;
        }

        if (owner_ != nullptr) {
            RAV_WARNING("RTP receiver is already running");
            return;
        }

        owner_ = &owner;

        receive_rtp();
        receive_rtcp();

        RAV_TRACE(
            "RTP Receiver impl started. RTP on {}:{}, RTCP on {}:{}",
            rtp_socket_.local_endpoint().address().to_string(), rtp_socket_.local_endpoint().port(),
            rtcp_socket_.local_endpoint().address().to_string(), rtcp_socket_.local_endpoint().port()
        );
    }

    void stop() {
        owner_ = nullptr;

        // (No need to call shutdown on the sockets as they are datagram sockets).

        asio::error_code ec;
        rtp_socket_.close(ec);
        if (ec) {
            RAV_ERROR("Failed to close RTP socket: {}", ec.message());
        }
        rtcp_socket_.close(ec);
        if (ec) {
            RAV_ERROR("Failed to close RTCP socket: {}", ec.message());
        }

        RAV_TRACE("Endpoint stopped.");
    }

  private:
    rtp_receiver* owner_ = nullptr;
    asio::ip::udp::socket rtp_socket_;
    asio::ip::udp::socket rtcp_socket_;
    asio::ip::udp::endpoint rtp_sender_endpoint_;   // For receiving the senders address.
    asio::ip::udp::endpoint rtcp_sender_endpoint_;  // For receiving the senders address.
    std::array<uint8_t, 1500> rtp_data_ {};
    std::array<uint8_t, 1500> rtcp_data_ {};

    void receive_rtp() {
        auto self = shared_from_this();
        rtp_socket_.async_receive_from(
            asio::buffer(rtp_data_), rtp_sender_endpoint_,
            [self](const std::error_code& ec, const std::size_t length) {
                TRACY_ZONE_SCOPED;
                if (ec) {
                    if (ec == asio::error::operation_aborted) {
                        RAV_TRACE("Operation aborted");
                        return;
                    }
                    if (ec == asio::error::eof) {
                        RAV_TRACE("EOF");
                        return;
                    }
                    RAV_ERROR("Read error: {}. Closing connection.", ec.message());
                    return;
                }
                if (self->owner_ == nullptr) {
                    RAV_ERROR("Owner is null. Closing connection.");
                    return;
                }
                const rtp_packet_view rtp_packet(self->rtp_data_.data(), length);
                if (rtp_packet.validate()) {
                    const rtp_packet_event event {rtp_packet};
                    self->owner_->on(event);
                } else {
                    RAV_WARNING("Invalid RTP packet received. Ignoring.");
                }
                self->receive_rtp();  // Schedule another round of receiving.
            }
        );
    }

    void receive_rtcp() {
        auto self = shared_from_this();
        rtcp_socket_.async_receive_from(
            asio::buffer(rtcp_data_), rtcp_sender_endpoint_,
            [self](const std::error_code& ec, const std::size_t length) {
                if (ec) {
                    if (ec == asio::error::operation_aborted) {
                        RAV_TRACE("Operation aborted");
                        return;
                    }
                    if (ec == asio::error::eof) {
                        RAV_TRACE("EOF");
                        return;
                    }
                    RAV_ERROR("Read error: {}. Closing connection.", ec.message());
                    return;
                }
                if (self->owner_ == nullptr) {
                    RAV_ERROR("Owner is null. Closing connection.");
                    return;
                }
                const rtcp_packet_view rtcp_packet(self->rtcp_data_.data(), length);
                if (rtcp_packet.validate()) {
                    const rtcp_packet_event event {rtcp_packet};
                    self->owner_->on(event);
                } else {
                    RAV_WARNING("Invalid RTCP packet received. Ignoring.");
                }
                self->receive_rtcp();  // Schedule another round of receiving.
            }
        );
    }
};

rav::rtp_receiver::rtp_receiver(asio::io_context& io_context) : io_context_(io_context) {}

rav::rtp_receiver::~rtp_receiver() {
    stop();
}

void rav::rtp_receiver::start(const asio::ip::address& bind_addr, uint16_t rtp_port, uint16_t rtcp_port) {
    if (impl_) {
        RAV_WARNING("RTP receiver already running");
        return;
    }
    impl_ = std::make_shared<impl>(io_context_, bind_addr, rtp_port, rtcp_port);
    impl_->start(*this);

    RAV_TRACE(
        "RTP Receiver started. RTP on {}:{}, RTCP on {}:{}", bind_addr.to_string(), rtp_port, bind_addr.to_string(),
        rtcp_port
    );
}

void rav::rtp_receiver::stop() {
    impl_->stop();
    impl_.reset();

    RAV_TRACE("RTP Receiver stopped.");
}

void rav::rtp_receiver::subscribe(subscriber& subscriber) {
    subscribers_.add(&subscriber);
}

void rav::rtp_receiver::unsubscribe(subscriber& subscriber) {
    subscribers_.remove(&subscriber);
}

void rav::rtp_receiver::on(const rtp_packet_event& rtp_event) {
    RAV_INFO("{}", rtp_event.packet.to_string());
}

void rav::rtp_receiver::on(const rtcp_packet_event& rtcp_event) {
    RAV_INFO("{}", rtcp_event.packet.to_string());
}
