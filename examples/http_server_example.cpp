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
#include "ravennakit/core/net/http/http_server.hpp"

int main() {
    rav::set_log_level_from_env();
    rav::do_system_checks();

    boost::asio::io_context io_context;

    // Create a server instance
    rav::HttpServer server(io_context);

    server.get("/", [](const rav::HttpServer::Request&, rav::HttpServer::Response& response, rav::PathMatcher::Parameters&) {
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = "Hello, World!";
        response.prepare_payload();
    });

    server.get(
        "/shutdown",
        [&io_context, &server](const rav::HttpServer::Request&, rav::HttpServer::Response& response, rav::PathMatcher::Parameters&) {
            response.result(boost::beast::http::status::ok);
            response.set(boost::beast::http::field::content_type, "text/plain");
            response.body() = "Shutting down server...";
            response.prepare_payload();

            boost::asio::post(io_context, [&server] {
                server.stop();
            });
        }
    );

    // Start the server
    const auto result = server.start("127.0.0.1", 8080);
    if (result.has_error()) {
        RAV_LOG_ERROR("Error starting server: {}", result.error().message());
        return 1;
    }

    RAV_LOG_INFO("Server started at http://{}", server.get_address_string());
    RAV_LOG_INFO("Visit http://{}/shutdown to stop the server", server.get_address_string());

    // Run the io_context to start accepting connections
    io_context.run();

    return 0;
}
