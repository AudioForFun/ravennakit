/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/core/system.hpp"
#include "ravennakit/nmos/nmos_node.hpp"

#include <boost/asio/io_context.hpp>

int main() {
    rav::set_log_level_from_env();
    rav::do_system_checks();

    boost::asio::io_context io_context;

    rav::nmos::Node node(io_context);
    const auto result = node.start("127.0.0.1", 5555);
    if (result.has_error()) {
        RAV_ERROR("Failed to start NMOS node: {}", result.error().message());
        return 1;
    }

    // Devices
    for (uint32_t i = 0; i < 5; ++i) {
        rav::nmos::Device::Control control;
        control.href = fmt::format("http://localhost:{}", i + 6000);
        control.type = fmt::format("urn:x-manufacturer:control:generic.{}", i + 1);
        control.authorization = i % 2 == 0;
        rav::nmos::Device device;
        device.id = boost::uuids::random_generator()();
        device.description = fmt::format("Device {} desc", i + 1);
        device.label = fmt::format("Device {} label", i + 1);
        device.version = rav::nmos::Version {i + 1, (i + 1) * 1000};
        device.controls.push_back(control);
        std::ignore = node.set_device(device);
    }

    // Flows
    for (uint32_t i = 0; i < 5; ++i) {
        rav::nmos::FlowAudioRaw flow;
        flow.id = boost::uuids::random_generator()();
        flow.label = fmt::format("Flow {} label", i + 1);
        flow.description = fmt::format("Flow {} desc", i + 1);
        flow.version = rav::nmos::Version {i + 1, (i + 1) * 1000};
        flow.bit_depth = 24;
        flow.sample_rate = {48000, 1};
        flow.media_type = "audio/L24";
        flow.source_id = boost::uuids::random_generator()(); // TODO: Assign a valid source ID
        flow.device_id = boost::uuids::random_generator()(); // TODO: Assign a valid device ID
        std::ignore = node.set_flow({flow});
    }

    // Receivers
    for (uint32_t i = 0; i < 5; ++i) {
        rav::nmos::ReceiverAudio receiver;
        receiver.id = boost::uuids::random_generator()();
        receiver.label = fmt::format("Receiver {} label", i + 1);
        receiver.description = fmt::format("Receiver {} desc", i + 1);
        receiver.version = rav::nmos::Version {i + 1, (i + 1) * 1000};
        receiver.device_id = boost::uuids::random_generator()(); // TODO: Assign a valid device ID
        receiver.transport = "urn:x-nmos:transport:rtp";
        receiver.caps.media_types = {"audio/L24", "audio/L20", "audio/L16", "audio/L8", "audio/PCM"};
        std::ignore = node.set_receiver({receiver});
    }

    // Senders
    for (uint32_t i = 0; i < 5; ++i) {
        rav::nmos::Sender sender;
        sender.id = boost::uuids::random_generator()();
        sender.label = fmt::format("Sender {} label", i + 1);
        sender.description = fmt::format("Sender {} desc", i + 1);
        sender.version = rav::nmos::Version {i + 1, (i + 1) * 1000};
        sender.device_id = boost::uuids::random_generator()(); // TODO: Assign a valid device ID
        sender.transport = "urn:x-nmos:transport:rtp";
        std::ignore = node.set_sender(sender);
    }

    std::string url =
        fmt::format("http://{}:{}", node.get_local_endpoint().address().to_string(), node.get_local_endpoint().port());

    RAV_INFO("NMOS node started at {}", url);

    url += "/x-nmos";
    RAV_INFO("{}", url);

    url += "/node";
    RAV_INFO("{}", url);

    url += "/v1.3";
    RAV_INFO("{}", url);

    url += "/";
    RAV_INFO("{}", url);

    io_context.run();

    return 0;
}
