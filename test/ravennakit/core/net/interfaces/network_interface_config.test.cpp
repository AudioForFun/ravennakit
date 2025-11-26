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

#include "ravennakit/core/net/interfaces/network_interface_config.hpp"
#include "network_interface_config.test.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::NetworkInterfaceConfig") {
    rav::NetworkInterfaceConfig config;
    config.set_interface(0, "1");
    config.set_interface(1, "2");
    test_network_interface_config_json(config, config.to_boost_json());
}

void rav::test_network_interface_config_json(const NetworkInterfaceConfig& config, const boost::json::value& json) {
    REQUIRE(json.is_array());
    REQUIRE(json.as_array().size() == config.interfaces.size());

    for (const auto& i : json.as_array()) {
        REQUIRE(i.is_object());
        auto rank = i.at("rank").to_number<uint8_t>();
        REQUIRE(i.at("identifier").as_string() == config.interfaces.at(rank));
    }
}
