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
#include "ravennakit/core/system.hpp"
#include "ravennakit/dnssd/bonjour/bonjour_browser.hpp"
#include "ravennakit/ravenna/ravenna_rtsp_client.hpp"
#include "ravennakit/ravenna/ravenna_sink.hpp"

#include <CLI/App.hpp>
#include <asio/io_context.hpp>

/**
 * This examples demonstrates how to receive audio streams from a RAVENNA device. It sets up a RAVENNA sink that listens
 * for announcements from a RAVENNA device and starts receiving audio data. It will play the audio to the selected audio
 * device using portaudio.
 */
class ravenna_recorder_example {
  public:
    explicit ravenna_recorder_example(const std::string& stream_name, const std::string& interface_address) {
        node_browser_ = rav::dnssd::dnssd_browser::create(io_context_);

        if (node_browser_ == nullptr) {
            RAV_THROW_EXCEPTION("No dnssd browser available");
        }

        node_browser_->browse_for("_rtsp._tcp,_ravenna_session");

        rtsp_client_ = std::make_unique<rav::ravenna_rtsp_client>(io_context_, *node_browser_);

        rav::rtp_receiver::configuration config;
        config.interface_address = asio::ip::make_address(interface_address);
        rtp_receiver_ = std::make_unique<rav::rtp_receiver>(io_context_, config);

        ravenna_sink_ = std::make_unique<rav::ravenna_sink>(*rtsp_client_, *rtp_receiver_, stream_name);
    }

    ~ravenna_recorder_example() = default;

    void start() {
        io_context_.run();
    }

    void stop() {
        io_context_.stop();
    }

  private:
    asio::io_context io_context_;
    std::unique_ptr<rav::dnssd::dnssd_browser> node_browser_;
    std::unique_ptr<rav::ravenna_rtsp_client> rtsp_client_;
    std::unique_ptr<rav::rtp_receiver> rtp_receiver_;
    std::unique_ptr<rav::ravenna_sink> ravenna_sink_;
};

int main(int const argc, char* argv[]) {
    rav::log::set_level_from_env();
    rav::system::do_system_checks();

    CLI::App app {"RAVENNA Receiver example"};
    argv = app.ensure_utf8(argv);

    std::string stream_name;
    app.add_option("stream_name", stream_name, "The name of the stream to receive")->required();

    std::string interface_address = "0.0.0.0";
    app.add_option("--interface-addr", interface_address, "The interface address");

    CLI11_PARSE(app, argc, argv);

    ravenna_recorder_example receiver_example(stream_name, interface_address);

    std::thread cin_thread([&receiver_example] {
        fmt::println("Press return key to stop...");
        std::cin.get();
        receiver_example.stop();
    });

    receiver_example.start();
    cin_thread.join();

    return 0;
}
