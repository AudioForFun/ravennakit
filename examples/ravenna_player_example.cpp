/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

/**
 * This examples demonstrates how to create a source and send audio onto the network. It does this by reading audio from
 * a wav file on disk and sending it as multicast audio packets.
 */

#include "ravennakit/core/file.hpp"
#include "ravennakit/core/log.hpp"
#include "ravennakit/core/system.hpp"
#include "ravennakit/core/audio/formats/wav_audio_format.hpp"
#include "ravennakit/core/streams/file_input_stream.hpp"
#include "ravennakit/dnssd/dnssd_advertiser.hpp"
#include "ravennakit/ptp/ptp_instance.hpp"
#include "ravennakit/ravenna/ravenna_transmitter.hpp"

#include <CLI/App.hpp>
#include <asio/io_context.hpp>
#include <utility>

/**
 * This example shows how to send audio data from a wav file onto the network using RAVENNA.
 */
class ravenna_player_example: public rav::ptp_instance::subscriber {
  public:
    explicit ravenna_player_example(
        asio::io_context& io_context, asio::ip::address_v4 interface_address, const uint16_t port_num
    ) :
        io_context_(io_context),
        interface_address_(std::move(interface_address)),
        rtsp_server_(io_context, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)),
        rtp_transmitter_(io_context, interface_address),
        ptp_instance_(io_context) {
        advertiser_ = rav::dnssd::dnssd_advertiser::create(io_context);
        ptp_instance_.add_subscriber(this);
        if (const auto result = ptp_instance_.add_port(interface_address_); !result) {
            RAV_ERROR("Failed to add PTP port: {}", to_string(result.error()));
        }
    }

    ~ravenna_player_example() override {
        ptp_instance_.remove_subscriber(this);
    }

    void add_source(const rav::file& file) {
        if (!file.exists()) {
            throw std::runtime_error("File does not exist: " + file.path().string());
        }
        const auto file_session_name = file.path().filename().string();
        for (const auto& s : sources_) {
            // Test if a source with the same session name already exists
            if (s.transmitter->session_name() == file_session_name) {
                throw std::runtime_error("A source with the same session name already exists: " + file_session_name);
            }
        }
        auto transmitter = std::make_unique<rav::ravenna_transmitter>(
            io_context_, *advertiser_, rtsp_server_, ptp_instance_, rtp_transmitter_, id_generator_.next(),
            file_session_name, interface_address_
        );

        auto file_input_stream = std::make_unique<rav::file_input_stream>(file);
        auto reader = rav::wav_audio_format::reader(std::move(file_input_stream));

        const auto format = reader.get_audio_format();
        if (!format) {
            throw std::runtime_error("Failed to read audio format from file: " + file.path().string());
        }
        if (!transmitter->set_audio_format(*format)) {
            throw std::runtime_error("Unsupported audio format for transmitter: " + file.path().string());
        }

        sources_.push_back({std::move(reader), std::move(transmitter)});
    }

    void on_port_changed_state(const rav::ptp_port& port) override {
        if (port.state() == rav::ptp_state::slave) {
            // Start the transmitters when the port is in slave state (and thus synchronized)
            for (const auto& s : sources_) {
                if (!s.transmitter->is_running()) {
                    s.transmitter->start(ptp_instance_.get_local_ptp_time());
                }
            }
        }
    }

  private:
    struct source {
        rav::wav_audio_format::reader reader;
        std::unique_ptr<rav::ravenna_transmitter> transmitter;
    };

    asio::io_context& io_context_;
    asio::ip::address_v4 interface_address_;
    std::unique_ptr<rav::dnssd::dnssd_advertiser> advertiser_;
    rav::rtsp_server rtsp_server_;
    rav::rtp_transmitter rtp_transmitter_;
    rav::id::generator id_generator_ {};
    std::vector<source> sources_;
    rav::ptp_instance ptp_instance_;
};

int main(int const argc, char* argv[]) {
    rav::log::set_level_from_env();
    rav::system::do_system_checks();

    CLI::App app {"RAVENNA Player example"};
    argv = app.ensure_utf8(argv);

    std::vector<std::string> file_paths;
    app.add_option("files", file_paths, "The files to stream")->required();

    std::string interface_address_string = "0.0.0.0";
    app.add_option("--interface-addr", interface_address_string, "The interface address");

    CLI11_PARSE(app, argc, argv);

    const auto interface_address = asio::ip::make_address_v4(interface_address_string);

    asio::io_context io_context;
    ravenna_player_example player_example(io_context, interface_address, 5005);

    for (auto& file_path : file_paths) {
        player_example.add_source(rav::file(file_path));
    }

    io_context.run();
    return 0;
}
