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

namespace rav {

/**
 * Configuration for RAVENNA related classes.
 * This class is used to configure the Ravenna node and its components.
 */
class RavennaConfig {
  public:
    struct NetworkInterfaceConfig {
        std::optional<NetworkInterface> primary_interface;
        std::optional<NetworkInterface> secondary_interface;

        /**
         * @return A string representation of the network interface configuration.
         */
        [[nodiscard]] std::string to_string() const {
            return fmt::format(
                R"(Network interface configuration: primary: "{}", secondary: "{}")",
                primary_interface ? primary_interface->get_extended_display_name() : "none",
                secondary_interface ? secondary_interface->get_extended_display_name() : "none"
            );
        }
    };

    /**
     * Sets the network interface configuration.
     * @param config The new network interface configuration.
     * @returns True if the configuration was changed, false otherwise.
     */
    bool set_network_interface_config(NetworkInterfaceConfig config) {
        if (network_interface_config_.primary_interface == config.primary_interface
            && network_interface_config_.secondary_interface == config.secondary_interface) {
            return false;
        }
        network_interface_config_ = std::move(config);
        return true;
    }

    /**
     * @returns Gets the current network interface configuration.
     */
    [[nodiscard]] const NetworkInterfaceConfig& get_network_interface_config() const {
        return network_interface_config_;
    }

    /**
     * @return A string representation of the configuration.
     */
    [[nodiscard]] std::string to_string() const {
        return fmt::format("RAVENNA Configuration: {}", network_interface_config_.to_string());
    }

  private:
    NetworkInterfaceConfig network_interface_config_;
};

}  // namespace rav
