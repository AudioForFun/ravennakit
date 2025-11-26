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

#include "ravennakit/core/log.hpp"
#include "ravennakit/core/system.hpp"
#include "ravennakit/rtsp/rtsp_client.hpp"

#include <CLI/App.hpp>

/**
 * This example shows how to create a RTSP client.
 */

int main(int const argc, char* argv[]) {
    rav::set_log_level_from_env();
    rav::do_system_checks();

    CLI::App app {"RTSP Client example"};
    argv = app.ensure_utf8(argv);

    std::string host;
    app.add_option("host", host, "The host to connect to")->required();

    std::string port;
    app.add_option("port", port, "The port to connect to")->required();

    std::string path;
    app.add_option("path", path, "The path of the stream (/by-id/13 or /by-name/stream%20name)")->required();

    CLI11_PARSE(app, argc, argv);

    boost::asio::io_context io_context;

    rav::rtsp::Client client(io_context);

    client.on_connect_event = [path, &client](const rav::rtsp::Connection::ConnectEvent&) {
        RAV_LOG_INFO("Connected, send DESCRIBE request");
        client.async_describe(path);
    };

    client.on_request_event = [](const rav::rtsp::Connection::RequestEvent& event) {
        RAV_LOG_INFO("{}\n{}", event.rtsp_request.to_debug_string(true), rav::string_replace(event.rtsp_request.data, "\r\n", "\n"));
    };

    client.on_response_event = [](const rav::rtsp::Connection::ResponseEvent& event) {
        RAV_LOG_INFO("{}\n{}", event.rtsp_response.to_debug_string(true), rav::string_replace(event.rtsp_response.data, "\r\n", "\n"));
    };

    client.async_connect(host, port);

    io_context.run();
}
