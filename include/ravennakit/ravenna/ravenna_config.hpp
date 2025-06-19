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

#include "ravennakit/core/net/interfaces/network_interface.hpp"
#include "ravennakit/core/json.hpp"
#include "ravennakit/core/net/interfaces/network_interface_config.hpp"

namespace rav {

/**
 * Configuration for RAVENNA related classes.
 * This class is used to configure the Ravenna node and its components.
 */
struct RavennaConfig {
    /**
     * @return A string representation of the configuration.
     */
    [[nodiscard]] std::string to_string() const {
        return fmt::format("RAVENNA Configuration: {}", network_interfaces.to_string());
    }

    /**
     * @returns A JSON representation of the configuration.
     */
    [[nodiscard]] nlohmann::json to_json() const {
        nlohmann::json j;
        j["network_config"] = network_interfaces.to_json();
        return j;
    }

    /**
     * Creates a RavennaConfig from a JSON object.
     * @param json The JSON object to parse.
     * @return A newly constructed RavennaConfig object.
     */
    static tl::expected<RavennaConfig, std::string> from_json(const nlohmann::json& json) {
        try {
            RavennaConfig config;
            auto result = NetworkInterfaceConfig::from_json(json.at("network_config"));
            if (!result) {
                return tl::unexpected(result.error());
            }
            config.network_interfaces = result.value();
            return config;
        } catch (const std::exception& e) {
            return tl::unexpected(fmt::format("Failed to parse RavennaConfig: {}", e.what()));
        }
    }

    NetworkInterfaceConfig network_interfaces;
};

}  // namespace rav
