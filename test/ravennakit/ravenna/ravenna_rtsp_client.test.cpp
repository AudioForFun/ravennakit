/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/dnssd/mock/mock_browser.hpp"
#include "ravennakit/ravenna/ravenna_rtsp_client.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("ravenna_rtsp_client | Subscribe, discover and announce") {
    asio::io_context io_context;
    rav::dnssd::mock_browser browser(io_context);
    rav::ravenna_rtsp_client client(io_context, browser);

}
