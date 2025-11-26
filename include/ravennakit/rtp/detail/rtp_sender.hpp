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

#pragma once

#include "ravennakit/core/containers/buffer_view.hpp"

namespace rav::rtp {

/**
 * This class is responsible for sending RTP packets.
 * - Maintains a socket to send RTP packets.
 * - Maintains a socket to send RTCP packets (maybe the same socket).
 */
class Sender {
  public:
    Sender(boost::asio::io_context& io_context, const boost::asio::ip::address_v4& interface_address) :
        socket_(io_context), interface_address_(interface_address) {
        socket_.open(boost::asio::ip::udp::v4());
        socket_.set_option(boost::asio::ip::multicast::outbound_interface(interface_address));
        socket_.set_option(boost::asio::ip::multicast::enable_loopback(false));
        socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    }

    /**
     * Sends given data as an RTP packet.
     * @param packet Encoded RTP packet.
     * @param endpoint The endpoint to send the packet to.
     */
    void send_to(const ByteBuffer& packet, const boost::asio::ip::udp::endpoint& endpoint) {
        RAV_ASSERT(packet.data() != nullptr, "Packet data is null");
        RAV_ASSERT(packet.size() > 0, "Packet size is 0");
        boost::system::error_code ec;
        socket_.send_to(boost::asio::buffer(packet.data(), packet.size()), endpoint, 0, ec);
        if (set_error(ec)) {
            RAV_LOG_ERROR("Failed to send RTP packet: {}", ec.message());
        }
    }

    /**
     * Sends given data as an RTP packet.
     * @param packet Encoded RTP packet.
     * @param endpoint The endpoint to send the packet to.
     */
    void send_to(const BufferView<const uint8_t>& packet, const boost::asio::ip::udp::endpoint& endpoint) {
        RAV_ASSERT(packet.data() != nullptr, "Packet data is null");
        RAV_ASSERT(!packet.empty(), "Packet is empty");
        boost::system::error_code ec;
        socket_.send_to(boost::asio::buffer(packet.data(), packet.size()), endpoint, 0, ec);
        if (set_error(ec)) {
            RAV_LOG_ERROR("Failed to send RTP packet: {}", ec.message());
        }
    }

    /**
     * Sends given data as an RTP packet.
     * @param data Pointer to the data to send.
     * @param data_size The size of the data to send.
     * @param endpoint The endpoint to send the packet to.
     */
    void send_to(const uint8_t* data, const size_t data_size, const boost::asio::ip::udp::endpoint& endpoint) {
        RAV_ASSERT(data != nullptr, "Packet data is null");
        RAV_ASSERT(data_size != 0, "Packet is empty");
        boost::system::error_code ec;
        socket_.send_to(boost::asio::buffer(data, data_size), endpoint, 0, ec);
        if (set_error(ec)) {
            RAV_LOG_ERROR("Failed to send RTP packet: {}", ec.message());
        }
    }

    /**
     * Sets the interface to use for this sender.
     * @param interface_address The address of the interface to use.
     */
    void set_interface(const boost::asio::ip::address_v4& interface_address) {
        boost::system::error_code ec;
        socket_.set_option(boost::asio::ip::multicast::outbound_interface(interface_address), ec);
        if (ec) {
            RAV_LOG_ERROR("Failed to set interface: {}", ec.message());
        }
        interface_address_ = interface_address;
    }

    /**
     * @return The interface address used by the sender.
     */
    [[nodiscard]] const boost::asio::ip::address_v4& get_interface_address() const {
        // TODO: Get the interface address from the socket (local endpoint)
        return interface_address_;
    }

  private:
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::address_v4 interface_address_;
    boost::system::error_code last_error_;  // Used to avoid log spamming

    /**
     * Convenience function to set the error code and prevent log spamming.
     * @param ec The error code to set.
     * @return The error code if it's not the same as the last error, otherwise an empty error code.
     */
    boost::system::error_code set_error(const boost::system::error_code& ec) {
        if (ec == last_error_) {
            return {};
        }
        last_error_ = ec;
        return last_error_;
    }
};

}  // namespace rav::rtp
