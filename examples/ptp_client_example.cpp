/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/system.hpp"
#include "ravennakit/ptp/messages/ptp_announce_message.hpp"
#include "ravennakit/ptp/messages/ptp_message.hpp"
#include "ravennakit/ptp/messages/ptp_message_header.hpp"
#include "ravennakit/rtp/detail/udp_sender_receiver.hpp"

#include <CLI/App.hpp>
#include <asio/io_context.hpp>

int main(int const argc, char* argv[]) {
    rav::log::set_level_from_env();
    rav::system::do_system_checks();

    CLI::App app {"RAVENNA Receiver example"};
    argv = app.ensure_utf8(argv);

    std::string interface_address = "0.0.0.0";
    app.add_option("--interface-addr", interface_address, "The interface address");

    CLI11_PARSE(app, argc, argv);

    std::vector<rav::subscription> subscriptions;
    asio::io_context io_context;

    const auto ptp_multicast_address = asio::ip::make_address("224.0.1.129");
    const rav::udp_sender_receiver ptp_event_socket(io_context, asio::ip::address_v4(), 319);
    const rav::udp_sender_receiver ptp_general_socket(io_context, asio::ip::address_v4(), 320);

    subscriptions.push_back(
        ptp_event_socket.join_multicast_group(ptp_multicast_address, asio::ip::make_address(interface_address))
    );
    subscriptions.push_back(
        ptp_general_socket.join_multicast_group(ptp_multicast_address, asio::ip::make_address(interface_address))
    );

    auto handler = [](const rav::udp_sender_receiver::recv_event& event) {
        const rav::buffer_view data(event.data, event.size);

        auto message = rav::ptp_message::from_data(data);
        if (!message.has_value()) {
            RAV_TRACE("PTP Error: {}", static_cast<std::underlying_type_t<rav::ptp_error>>(message.error()));
            return;
        }
        RAV_TRACE("{}", message->to_string());
    };

    ptp_event_socket.start(handler);
    ptp_general_socket.start(handler);

    io_context.run();

    return 0;
}
