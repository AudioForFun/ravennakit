/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/platform.hpp"
#include "ravennakit/dnssd/dnssd_browser.hpp"
#include "ravennakit/dnssd/bonjour/bonjour_browser.hpp"
#include "ravennakit/dnssd/mock/dnssd_mock_browser.hpp"

std::unique_ptr<rav::dnssd::Browser> rav::dnssd::Browser::create(boost::asio::io_context& io_context) {
#if RAV_APPLE
    return std::make_unique<BonjourBrowser>(io_context);
#elif RAV_WINDOWS
    if (dnssd::is_bonjour_service_running()) {
        return std::make_unique<BonjourBrowser>(io_context);
    } else {
        return {};
    }
#else
    RAV_WARNING("No browser implementation available, falling back to MockBrowser");
    return std::make_unique<MockBrowser>(io_context);
#endif
}
