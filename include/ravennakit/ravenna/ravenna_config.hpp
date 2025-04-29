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
#include "ravennakit/core/net/interfaces/network_interface_list.hpp"
#include "ravennakit/core/json.hpp"
#include "ravennakit/core/util/rank.hpp"

namespace rav {

/**
 * Configuration for RAVENNA related classes.
 * This class is used to configure the Ravenna node and its components.
 */
struct RavennaConfig {
    class NetworkInterfaceConfig {
      public:
        friend bool operator==(const NetworkInterfaceConfig& lhs, const NetworkInterfaceConfig& rhs) {
            return lhs.interfaces_ == rhs.interfaces_;
        }

        friend bool operator!=(const NetworkInterfaceConfig& lhs, const NetworkInterfaceConfig& rhs) {
            return lhs.interfaces_ != rhs.interfaces_;
        }

        /**
         * Sets a network interface by its rank.
         * @param rank The rank of the network interface.
         * @param identifier The identifier of the network interface.
         */
        void set_interface(const Rank rank, const NetworkInterface::Identifier& identifier) {
            interfaces_[rank] = identifier;
        }

        /**
         * Gets a network interface by its rank.
         * @param rank The rank of the network interface.
         * @return A pointer to the network interface identifier if found, or nullptr if not found.
         */
        [[nodiscard]] const NetworkInterface::Identifier* get_interface(const Rank rank) const {
            const auto it = interfaces_.find(rank);
            if (it == interfaces_.end()) {
                return nullptr;
            }
            return &it->second;
        }

        /**
         * @return  A map of all network interfaces and their identifiers. The key is the rank of the interface.
         */
        [[nodiscard]] std::map<Rank, NetworkInterface::Identifier> get_interfaces() const {
            return interfaces_;
        }

        /**
         * @return The first IPv4 address of one of the network interfaces. The address will be unspecified if the
         * interface is not found or if it has no IPv4 address.
         */
        [[nodiscard]] asio::ip::address_v4 get_interface_ipv4_address(const Rank rank) const {
            const auto it = interfaces_.find(rank);
            if (it == interfaces_.end()) {
                return asio::ip::address_v4 {};
            }
            if (auto* interface = NetworkInterfaceList::get_system_interfaces().get_interface(it->second)) {
                return interface->get_first_ipv4_address();
            }
            return asio::ip::address_v4 {};
        }

        /**
         * @return A map of all network interfaces and their first IPv4 address. The address will be unspecified if the
         * interface has no IPv4 address.
         */
        [[nodiscard]] std::map<Rank, asio::ip::address_v4> get_interface_ipv4_addresses() const {
            std::map<Rank, asio::ip::address_v4> addresses;
            for (const auto& iface : interfaces_) {
                addresses[iface.first] = get_interface_ipv4_address(iface.first);
            }
            return addresses;
        }

        /**
         * @return A string representation of the network interface configuration.
         */
        [[nodiscard]] std::string to_string() const {
            std::string output = "Network interface configuration: ";
            if (interfaces_.empty()) {
                return output + "none";
            }
            bool first = true;
            for (auto& iface : interfaces_) {
                if (!first) {
                    output += ", ";
                }
                first = false;
                fmt::format_to(std::back_inserter(output), "{}({})", iface.second, iface.first.value());
            }
            return output;
        }

        /**
         * @returns A JSON representation of the network interface configuration.
         */
        [[nodiscard]] nlohmann::json to_json() const {
            auto a = nlohmann::json::array();

            for (auto& iface : interfaces_) {
                nlohmann::json o;
                o["rank"] = iface.first.value();
                o["identifier"] = iface.second;
                a.push_back(o);
            }

            return a;
        }

        /**
         * Creates a NetworkInterfaceConfig from a JSON object.
         * @param json
         * @return A newly constructed NetworkInterfaceConfig object.
         */
        [[nodiscard]] static tl::expected<NetworkInterfaceConfig, std::string> from_json(const nlohmann::json& json) {
            NetworkInterfaceConfig config;
            try {
                for (auto& object : json) {
                    const auto rank = Rank(object.at("rank").get<uint8_t>());
                    const auto identifier = object.at("identifier").get<NetworkInterface::Identifier>();
                    config.interfaces_[rank] = identifier;
                }
            } catch (const std::exception& e) {
                return tl::unexpected(fmt::format("Failed to parse NetworkInterfaceConfig: {}", e.what()));
            }
            return config;
        }

      private:
        std::map<Rank, NetworkInterface::Identifier> interfaces_;
    };

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
