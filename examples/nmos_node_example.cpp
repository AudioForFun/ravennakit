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


    return 0;
}