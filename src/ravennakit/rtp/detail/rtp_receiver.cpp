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
#include "ravennakit/rtp/detail/rtp_receiver.hpp"

#include "ravennakit/core/subscriber_list.hpp"
#include "ravennakit/rtp/rtp_packet_view.hpp"
#include "ravennakit/core/tracy.hpp"
#include "ravennakit/core/subscription.hpp"

#include <fmt/core.h>
#include "ravennakit/core/expected.hpp"

#include <utility>

#if RAV_APPLE
    #define IP_RECVDSTADDR_PKTINFO IP_RECVDSTADDR
#else
    #define IP_RECVDSTADDR_PKTINFO IP_PKTINFO
#endif

#if RAV_WINDOWS
typedef BOOL(PASCAL* LPFN_WSARECVMSG)(
    SOCKET s, LPWSAMSG lpMsg, LPDWORD lpNumberOfBytesRecvd, LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);
#endif

rav::rtp::Receiver::Receiver(asio::io_context& io_context, UdpReceiver& udp_receiver) :
    io_context_(io_context), udp_receiver_(udp_receiver) {}

asio::io_context& rav::rtp::Receiver::get_io_context() const {
    return io_context_;
}

bool rav::rtp::Receiver::subscribe(Subscriber* subscriber, const Session& session) {
    auto* context = find_or_create_session_context(session);

    if (context == nullptr) {
        RAV_WARNING("Failed to find or create new session context");
        return false;
    }

    RAV_ASSERT(context != nullptr, "Expecting valid session at this point");

    return context->subscribers.add(subscriber);
}

bool rav::rtp::Receiver::unsubscribe(const Subscriber* subscriber) {
    size_t count = 0;
    for (auto it = sessions_contexts_.begin(); it != sessions_contexts_.end();) {
        if ((*it)->remove_subscriber(subscriber)) {
            count++;
        }
        if ((*it)->empty()) {
            it = sessions_contexts_.erase(it);
        } else {
            ++it;
        }
    }
    return count > 0;
}

void rav::rtp::Receiver::set_interface(const asio::ip::address& interface_address) {
    config_.interface_address = interface_address;

    // TODO: Reimplement or remove

    // for (auto& session : sessions_contexts_) {
    //     if (!interface_address.is_unspecified() && session.session.connection_address.is_multicast()) {
    //         session.rtp_sender_receiver->join_multicast_group(
    //             session.session.connection_address.to_v4(), interface_address.to_v4()
    //         );
    //         session.rtcp_sender_receiver->join_multicast_group(
    //             session.session.connection_address.to_v4(), interface_address.to_v4()
    //         );
    //     } else {
    //         // TODO: Leave multicast group(s)
    //     }
    // }
}

rav::rtp::Receiver::SessionContext::SessionContext(
    UdpReceiver& udp_receiver, Session session, const asio::ip::address_v4& interface_address
) :
    udp_receiver_(udp_receiver), session_(std::move(session)) {
    if (session.connection_address.is_multicast()) {
        if (!udp_receiver_.subscribe(this, session.connection_address.to_v4(), interface_address, session.rtp_port)) {
            RAV_ERROR("Failed to subscribe to multicast RTP session {}", session.to_string());
        }
        if (!udp_receiver_.subscribe(this, session.connection_address.to_v4(), interface_address, session.rtcp_port)) {
            RAV_ERROR("Failed to subscribe to multicast RTP session {}", session.to_string());
        }
    } else {
        if (!udp_receiver_.subscribe(this, interface_address, session.rtp_port)) {
            RAV_ERROR("Failed to subscribe to unicast RTP session {}", session.to_string());
        }
        if (!udp_receiver_.subscribe(this, interface_address, session.rtcp_port)) {
            RAV_ERROR("Failed to subscribe to unicast RTP session {}", session.to_string());
        }
    }
}

rav::rtp::Receiver::SessionContext::~SessionContext() {
    udp_receiver_.unsubscribe(this);
}

bool rav::rtp::Receiver::SessionContext::remove_subscriber(const Receiver::Subscriber* subscriber) {
    return subscribers.remove(subscriber);
}

bool rav::rtp::Receiver::SessionContext::empty() const {
    return subscribers.empty();
}

const rav::rtp::Session& rav::rtp::Receiver::SessionContext::get_session() const {
    return session_;
}

void rav::rtp::Receiver::SessionContext::on_receive(const ExtendedUdpSocket::RecvEvent& event) {
    if (event.dst_endpoint.port() == session_.rtp_port) {
        handle_incoming_rtp_data(event);
    } else if (event.dst_endpoint.port() == session_.rtcp_port) {
        // TODO: Handle RTCP data
    } else {
        RAV_WARNING("Received data on unknown port: {}", event.dst_endpoint.port());
    }
}

void rav::rtp::Receiver::SessionContext::handle_incoming_rtp_data(const ExtendedUdpSocket::RecvEvent& event) {
    TRACY_ZONE_SCOPED;

    const PacketView packet(event.data, event.size);
    if (!packet.validate()) {
        RAV_WARNING("Invalid RTP packet received");
        return;
    }

    const RtpPacketEvent rtp_event {packet, session_, event.src_endpoint, event.dst_endpoint, event.recv_time};

    bool did_find_stream = false;

    for (auto& state : synchronization_sources) {
        if (state.get_ssrc() == packet.ssrc()) {
            did_find_stream = true;
        }
    }

    if (!did_find_stream) {
        const auto& it = synchronization_sources.emplace_back(packet.ssrc());
        RAV_TRACE(
            "Added new stream with SSRC {} from {}:{}", it.get_ssrc(), event.src_endpoint.address().to_string(),
            event.src_endpoint.port()
        );
    }

    for (auto* s : subscribers) {
        s->on_rtp_packet(rtp_event);
    }
}

rav::rtp::Receiver::SessionContext* rav::rtp::Receiver::find_session_context(const Session& session) const {
    for (const auto& context : sessions_contexts_) {
        if (context->get_session() == session) {
            return context.get();
        }
    }
    return nullptr;
}

rav::rtp::Receiver::SessionContext* rav::rtp::Receiver::create_new_session_context(const Session& session) {
    // TODO: Disallow a port to be used in multiple sessions because when receiving RTP data we don't know which session
    // it belongs to.

    auto new_session = std::make_unique<SessionContext>(udp_receiver_, session, config_.interface_address.to_v4());
    const auto& it = sessions_contexts_.emplace_back(std::move(new_session));
    RAV_TRACE("New RTP session context created for: {}", session.to_string());
    return it.get();
}

rav::rtp::Receiver::SessionContext* rav::rtp::Receiver::find_or_create_session_context(const Session& session) {
    auto context = find_session_context(session);
    if (context == nullptr) {
        context = create_new_session_context(session);
    }
    return context;
}
