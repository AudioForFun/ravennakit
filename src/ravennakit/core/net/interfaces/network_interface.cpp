/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/net/interfaces/network_interface.hpp"
#include "ravennakit/core/platform.hpp"
#include "ravennakit/core/subscription.hpp"
#include "ravennakit/core/net/interfaces/mac_address.hpp"

#if RAV_POSIX
#include <ifaddrs.h>
#include <iomanip>
#include <arpa/inet.h>
#include <asio/ip/address.hpp>
#include <netinet/in.h>
#include <sys/socket.h>
#if RAV_APPLE
#include <net/if_dl.h>
#elif RAV_LINUX
#include <linux/if_packet.h>
#endif
#endif

void rav::network_interface::add_address(const asio::ip::address& address) {
    for (auto& addr : addresses_) {
        if (addr == address) {
            return;
        }
    }
    addresses_.push_back(address);
}

void rav::network_interface::set_mac_address(const mac_address& mac_address) {
    mac_address_ = mac_address;
}

void rav::network_interface::set_flags(const flags& flags) {
    flags_ = flags;
}

const std::string& rav::network_interface::bsd_name() const {
    return bsd_name_;
}

std::string rav::network_interface::to_string() {
    std::string output = fmt::format("{}\n", bsd_name_);
    fmt::format_to(std::back_inserter(output), "  mac:\n    {}\n", mac_address_.to_string());
    if (!addresses_.empty()) {
        fmt::format_to(std::back_inserter(output), "  addrs:\n");
        for (const auto& address : addresses_) {
            fmt::format_to(std::back_inserter(output), "    {}\n", address.to_string());
        }
    }

    fmt::format_to(std::back_inserter(output), "  flags:\n   ");
    if (flags_.up) {
        fmt::format_to(std::back_inserter(output), " UP");
    }
    if (flags_.broadcast) {
        fmt::format_to(std::back_inserter(output), " BROADCAST");
    }
    if (flags_.loopback) {
        fmt::format_to(std::back_inserter(output), " LOOPBACK");
    }
    if (flags_.point_to_point) {
        fmt::format_to(std::back_inserter(output), " POINTTOPOINT");
    }
    if (flags_.promiscuous) {
        fmt::format_to(std::back_inserter(output), " PROMISC");
    }
    if (flags_.allmulti) {
        fmt::format_to(std::back_inserter(output), " ALLMULTI");
    }
    if (flags_.multicast) {
        fmt::format_to(std::back_inserter(output), " MULTICAST");
    }
    fmt::format_to(std::back_inserter(output), "\n");

    return output;
}

tl::expected<std::vector<rav::network_interface>, int> rav::get_all_network_interfaces() {
    std::vector<network_interface> network_interfaces;

#if RAV_POSIX
    ifaddrs* ifap = nullptr;

    // Get the list of network interfaces
    if (getifaddrs(&ifap) != 0) {
        return tl::unexpected(errno);
    }

    defer cleanup([&ifap] {
        freeifaddrs(ifap);
    });

    for (const ifaddrs* ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_name == nullptr) {
            RAV_WARNING("Network interface name is null");
            continue;
        }

        auto it = std::find_if(
            network_interfaces.begin(), network_interfaces.end(),
            [&ifa](const network_interface& network_interface) {
                return network_interface.bsd_name() == ifa->ifa_name;
            }
        );

        if (it == network_interfaces.end()) {
            it = network_interfaces.emplace(it, ifa->ifa_name);
        }

        // Address
        if (ifa->ifa_addr) {
            if (ifa->ifa_addr->sa_family == AF_INET) {
                const sockaddr_in* sa = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
                asio::ip::address_v4 addr_v4(ntohl(sa->sin_addr.s_addr));
                it->add_address(addr_v4);
            } else if (ifa->ifa_addr->sa_family == AF_INET6) {
                const sockaddr_in6* sa = reinterpret_cast<struct sockaddr_in6*>(ifa->ifa_addr);
                asio::ip::address_v6::bytes_type bytes;
                std::memcpy(bytes.data(), &sa->sin6_addr, sizeof(bytes));
                asio::ip::address_v6 addr_v6(bytes, sa->sin6_scope_id);
                it->add_address(addr_v6);
                #if RAV_APPLE
            } else if (ifa->ifa_addr->sa_family == AF_LINK) {
                const sockaddr_dl* sdl = reinterpret_cast<struct sockaddr_dl*>(ifa->ifa_addr);
                if (sdl->sdl_alen == 6) {  // MAC addresses are 6 bytes
                    it->set_mac_address(mac_address(reinterpret_cast<uint8_t*>(LLADDR(sdl))));
                }
                #elif RAV_LINUX
            } else if (ifa->ifa_addr->sa_family == AF_PACKET) {
                const sockaddr_ll* sdl = reinterpret_cast<struct sockaddr_ll*>(ifa->ifa_addr);
                it->set_mac_address(mac_address(sdl->sll_addr));
                #endif
            }
        }

        network_interface::flags flags{};
        if (ifa->ifa_flags & IFF_UP) {
            flags.up = true;
        } else if (ifa->ifa_flags & IFF_BROADCAST) {
            flags.broadcast = true;
        } else if (ifa->ifa_flags & IFF_LOOPBACK) {
            flags.loopback = true;
        } else if (ifa->ifa_flags & IFF_POINTOPOINT) {
            flags.point_to_point = true;
        } else if (ifa->ifa_flags & IFF_PROMISC) {
            flags.promiscuous = true;
        } else if (ifa->ifa_flags & IFF_ALLMULTI) {
            flags.allmulti = true;
        } else if (ifa->ifa_flags & IFF_MULTICAST) {
            flags.multicast = true;
        }
        it->set_flags(flags);
    }
#endif

    return network_interfaces;
}
