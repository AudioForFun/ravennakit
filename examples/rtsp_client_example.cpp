/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/asio/io_context_runner.hpp"
#include "ravennakit/core/log.hpp"
#include "ravennakit/core/system.hpp"
#include "ravennakit/rtsp/rtsp_client.hpp"

#include <CLI/App.hpp>

int main(int const argc, char* argv[]) {
#if RAV_ENABLE_SPDLOG
    spdlog::set_level(spdlog::level::trace);
#endif

    rav::system::do_system_checks();

    CLI::App app {"RTSP Client example"};
    argv = app.ensure_utf8(argv);

    std::string addr;
    app.add_option("address", addr, "The address to connect to")->required();

    std::string path;
    app.add_option("path", path, "The path of the stream (/by-id/13 or /by-name/stream%20name)")->required();

    CLI11_PARSE(app, argc, argv);

    asio::io_context io_context;

    rav::rtsp_client client(io_context);

    client.on<rav::rtsp::connect_event>([path](const rav::rtsp::connect_event&, rav::rtsp_client& c) {
        RAV_INFO("Connected, send DESCRIBE request");
        c.describe(path);
    });

    client.on<rav::rtsp_request>([](const rav::rtsp_request& request, rav::rtsp_client&) {
        RAV_INFO("{}\n{}", request.to_debug_string(), rav::string_replace(request.data, "\r\n", "\n"));
    });

    client.on<rav::rtsp_response>([](const rav::rtsp_response& response, rav::rtsp_client&) {
        RAV_INFO("{}\n{}", response.to_debug_string(), rav::string_replace(response.data, "\r\n", "\n"));
    });

    client.connect(addr, 80);

    io_context.run();
}
