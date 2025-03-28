/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/rtp/rtp_stream_sender.hpp"

rav::rtp::StreamSender::StreamSender(asio::io_context& io_context, const asio::ip::address_v4& interface_address) :
    rtp_sender_(io_context, interface_address) {}
