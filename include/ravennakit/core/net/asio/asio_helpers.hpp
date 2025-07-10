/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#pragma once

#include <boost/asio.hpp>

namespace rav {

using ip_address = boost::asio::ip::address;
using ip_address_v4 = boost::asio::ip::address_v4;
using ip_address_v6 = boost::asio::ip::address_v6;
using udp_socket = boost::asio::ip::udp::socket;

}
