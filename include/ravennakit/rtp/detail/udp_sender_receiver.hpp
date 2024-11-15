/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#pragma once

#include "ravennakit/core/subscription.hpp"

#include <asio.hpp>

namespace rav {

/**
 * A custom UDP sender and receiver class which extends usual UDP socket functionality by adding the ability to receive
 * the destination address of a received packet. This is useful for RTP where sessions are defined by the source and
 * destination endpoints. Also in cases where a single receiver is receiving from multiple senders, the destination
 * address is needed to determine the source of the packet.
 */
class udp_sender_receiver {
  public:
    struct recv_event {
        const uint8_t* data;
        size_t size;
        const asio::ip::udp::endpoint& src_endpoint;
        const asio::ip::udp::endpoint& dst_endpoint;
    };

    using handler_type = std::function<void(const recv_event& event)>;

    /**
     * Construct a new instance of the class. Private to force the use of the factory methods.
     * @param io_context The asio io_context to use.
     * @param endpoint The endpoint to bind to.
     */
    udp_sender_receiver(asio::io_context& io_context, const asio::ip::udp::endpoint& endpoint);

    /**
     * Construct a new instance of the class. Private to force the use of the factory methods.
     * @param io_context The asio io_context to use.
     * @param interface_address The address to bind to.
     * @param port The port to bind to.
     */
    udp_sender_receiver(asio::io_context& io_context, const asio::ip::address& interface_address, uint16_t port);

    ~udp_sender_receiver();

    /**
     * Start the receiver.
     * @param handler The handler to receive incoming packets.
     */
    void start(handler_type handler) const;

    /**
     * Join a multicast group.
     * @param multicast_address The multicast address to join.
     * @param interface_address The interface address to join the multicast group on.
     */
    void join_multicast_group(const asio::ip::address& multicast_address, const asio::ip::address& interface_address);

  private:
    class impl;
    std::shared_ptr<impl> impl_;
};

}  // namespace rav
