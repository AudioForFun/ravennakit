/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#pragma once
#include "ravennakit/dnssd/dnssd_browser.hpp"

namespace rav::dnssd {

class mock_browser: public dnssd_browser {
  public:
    mock_browser() = default;
    ~mock_browser() override = default;

    // dnssd_browser overrides
    void browse_for(const std::string& service_type) override;
    [[nodiscard]] const service_description* find_service(const std::string& service_name) const override;
    [[nodiscard]] std::vector<service_description> get_services() const override;
    void subscribe(subscriber& s) override;
};

}  // namespace rav::dnssd
