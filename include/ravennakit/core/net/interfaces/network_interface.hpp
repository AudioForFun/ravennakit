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

#include "mac_address.hpp"

#include <vector>
#include <asio/ip/address.hpp>
#include <tl/expected.hpp>
#include <optional>

namespace rav {

/**
 * Represents a network interface in the system.
 */
class network_interface {
  public:
    struct flags {
        /// Whether the interface is up.
        bool up;
        /// Whether the broadcast address is valid.
        bool broadcast;
        /// Whether the interface is a loopback interface.
        bool loopback;
        /// Whether the interface is a point-to-point link.
        bool point_to_point;
        /// Whether the interface is receiving all packets.
        bool promiscuous;
        /// Whether the interface is receiving all multicast packets.
        bool allmulti;
        /// Whether the interface supports multicast.
        bool multicast;
    };

    /// The type of the network interface.
    enum class type {
        undefined,
        wired,
        wifi,
        cellular,
        loopback,
        other,
    };

    /**
     * Constructs a network interface with the given bsd name. The name is used to uniquely identify the interface.
     * @param bsd_name The unique BSD name of the interface.
     */
    explicit network_interface(std::string bsd_name) : bsd_name_(std::move(bsd_name)) {}

    /**
     * Adds given address to the list of addresses. If the address is already in the list then this function has no
     * effect.
     * @param address The address to add.
     */
    void add_address(const asio::ip::address& address);

    /**
     * The mac address to set.
     * @param mac_address The mac address to set.
     */
    void set_mac_address(const mac_address& mac_address);

    /**
     * Sets the flags of this interface.
     * @param flags The flags to set.
     */
    void set_flags(const flags& flags);

    /**
     * Sets the display name of the network interface.
     * @param display_name The display name to set.
     */
    void set_display_name(const std::string& display_name);

    /**
     *
     * @return The name of the network interface.
     */
    [[nodiscard]] const std::string& bsd_name() const;

    /**
     * Sets the type of the network interface.
     * @param type The type to set.
     */
    void set_type(type type);

    /**
     * @returns The index of the network interface, or nullopt if the index could not be found.
     * Note: this is the index as defined by the operating system.
     */
    [[nodiscard]] std::optional<uint32_t> interface_index() const;

    /**
     * @returns A description of the network interface as string.
     */
    std::string to_string();

    /**
     * Converts the given type to a string.
     * @param type The type to convert.
     * @returns The string representation of the type.
     */
    static const char* type_to_string(type type);

  private:
    std::string bsd_name_;
    std::string display_name_;
    std::optional<mac_address> mac_address_;
    std::vector<asio::ip::address> addresses_;
    flags flags_{};
    type type_{type::undefined};
};

/**
 *
 * @returns A list of all network interfaces on the system. Only several operating systems are supported: macOS, Windows
 * and Linux. Not Android.
 */
tl::expected<std::vector<network_interface>, int> get_all_network_interfaces();

}  // namespace rav
