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

#include "ravennakit/dnssd/dnssd_advertiser.hpp"

namespace rav::dnssd {

class mock_advertiser: public dnssd_advertiser {
  public:
    mock_advertiser() = default;
    ~mock_advertiser() override = default;

    // dnssd_advertiser overrides
    util::id register_service(
        const std::string& reg_type, const char* name, const char* domain, uint16_t port, const txt_record& txt_record,
        bool auto_rename, bool local_only
    ) override;
    void update_txt_record(util::id id, const txt_record& txt_record) override;
    void unregister_service(util::id id) override;
    void subscribe(subscriber& s) override;
};

}  // namespace rav::dnssd
