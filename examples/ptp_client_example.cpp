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

#include "ravennakit/core/system.hpp"
#include "ravennakit/ptp/ptp_instance.hpp"

#include <CLI/App.hpp>
#include <boost/asio/io_context.hpp>

/**
 * This example shows how to create a PTP client.
 */

int main(int const argc, char* argv[]) {
    rav::set_log_level_from_env();
    rav::do_system_checks();

    CLI::App app {"PTP Client example"};
    argv = app.ensure_utf8(argv);

    std::string interface_address = "0.0.0.0";
    app.add_option("--interface-addr", interface_address, "The interface address");

    CLI11_PARSE(app, argc, argv);

    boost::asio::io_context io_context;

    rav::ptp::Instance ptp_instance(io_context);
    if (auto result = ptp_instance.add_port(1, boost::asio::ip::make_address_v4(interface_address)); !result) {
        RAV_LOG_TRACE("PTP Error: {}", static_cast<std::underlying_type_t<rav::ptp::Error>>(result.error()));
        return 1;
    }

    while (!io_context.stopped()) {
        io_context.poll();
    }

    return 0;
}
