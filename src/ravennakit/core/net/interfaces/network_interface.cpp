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
        #include "ravennakit/core/platform/apple/core_foundation/cf_array.hpp"
        #include "ravennakit/core/platform/apple/core_foundation/cf_string.hpp"
        #include "ravennakit/core/platform/apple/core_foundation/cf_type.hpp"
        #include "ravennakit/core/platform/apple/system_configuration/sc_network_interface.hpp"
        #include "ravennakit/core/platform/apple/system_configuration/sc_network_service.hpp"
        #include "ravennakit/core/platform/apple/system_configuration/sc_preferences.hpp"
    #elif RAV_LINUX
        #include <linux/if_packet.h>
    #endif
#elif RAV_WINDOWS
    #include <Iphlpapi.h>             // if_nametoindex
    #include <ws2tcpip.h>

    #pragma comment(lib, "Iphlpapi")  // if_nametoindex
    #pragma comment(lib, "Ws2_32")
#endif

#if RAV_APPLE
namespace {

// Get the type of the interface
rav::network_interface::type functional_type_for_interface(const char* name) {
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return rav::network_interface::type::undefined;
    }

    ifreq ifr = {};
    strlcpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));

    const bool success = ioctl(fd, SIOCGIFFUNCTIONALTYPE, &ifr) >= 0;

    const int junk = close(fd);
    assert(junk == 0);

    if (!success) {
        return rav::network_interface::type::undefined;
    }

    switch (ifr.ifr_ifru.ifru_functional_type) {
        case IFRTYPE_FUNCTIONAL_LOOPBACK:
            return rav::network_interface::type::loopback;
        case IFRTYPE_FUNCTIONAL_WIRED:
            return rav::network_interface::type::wired;
        case IFRTYPE_FUNCTIONAL_WIFI_INFRA:
        case IFRTYPE_FUNCTIONAL_WIFI_AWDL:
            return rav::network_interface::type::wifi;
        case IFRTYPE_FUNCTIONAL_CELLULAR:
            return rav::network_interface::type::cellular;
        case IFRTYPE_FUNCTIONAL_UNKNOWN:
        default:
            return rav::network_interface::type::other;
    }
}

}  // namespace
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

void rav::network_interface::set_display_name(const std::string& display_name) {
    display_name_ = display_name;
}

const std::string& rav::network_interface::bsd_name() const {
    return bsd_name_;
}

void rav::network_interface::set_type(const type type) {
    type_ = type;
}

std::optional<uint32_t> rav::network_interface::interface_index() const {
    auto index = if_nametoindex(bsd_name_.c_str());
    if (index == 0) {
        return std::nullopt;
    }
    return index;
}

std::string rav::network_interface::to_string() {
    std::string output = fmt::format("{}\n", bsd_name_);

    if (!display_name_.empty()) {
        fmt::format_to(std::back_inserter(output), "  display_name:\n    {}\n", display_name_);
    }

    if (mac_address_.has_value()) {
        fmt::format_to(std::back_inserter(output), "  mac:\n    {}\n", mac_address_->to_string());
    }

    fmt::format_to(std::back_inserter(output), "  type:\n    {}\n", type_to_string(type_));

    fmt::format_to(std::back_inserter(output), "  index:\n    {}\n", interface_index().value_or(0));

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

const char* rav::network_interface::type_to_string(const type type) {
    switch (type) {
        case type::wired:
            return "wired";
        case type::wifi:
            return "wifi";
        case type::cellular:
            return "cellular";
        case type::loopback:
            return "loopback";
        case type::other:
            return "other";
        case type::undefined:
        default:
            return "undefined";
    }
}

tl::expected<std::vector<rav::network_interface>, int> rav::get_all_network_interfaces() {
    std::vector<network_interface> network_interfaces;

#if RAV_POSIX && !RAV_ANDROID
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

    #if RAV_APPLE
        it->set_type(functional_type_for_interface(ifa->ifa_name));
    #endif

        network_interface::flags flags {};
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

    #if RAV_APPLE
    // Fill in the network display name

    const auto interfaces = sc_preferences::get_network_interfaces();

    if (!interfaces) {
        RAV_ERROR("Failed to get network interfaces");
        return tl::unexpected(-1);
    }

    for (CFIndex i = 0; i < interfaces.count(); ++i) {
        const auto interface = sc_network_interface(interfaces[i], true);
        if (!interface) {
            continue;
        }

        auto bsd_name = interface.get_bsd_name();
        if (bsd_name.empty()) {
            continue;
        }

        auto it = std::find_if(
            network_interfaces.begin(), network_interfaces.end(),
            [&bsd_name](const network_interface& network_interface) {
                return network_interface.bsd_name() == bsd_name;
            }
        );

        if (it == network_interfaces.end()) {
            continue;  // We're only filling in the existing interfaces, skipping interfaces not gotten from getifaddrs.
        }

        it->set_display_name(interface.get_localized_display_name());
    }
    #endif

#elif RAV_WINDOWS
    ULONG bufferSize = 15000; // As per recommendation from Microsoft
    std::vector<uint8_t> buffer(bufferSize);
    auto* addresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());

    auto result = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, addresses, &bufferSize);

    if (result != NO_ERROR) {
        if (result == ERROR_BUFFER_OVERFLOW) {
            // The buffer was not big enough, do a 2nd attempt.
            buffer.resize(bufferSize);
            addresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
            result = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, addresses, &bufferSize);
            if (result != NO_ERROR) {
                RAV_ERROR("Failed to get network interface information ({})", result);
                return {};
            }
        } else {
            RAV_ERROR("Failed to get network interface information ({})", result);
            return {};
        }
    }

    for (IP_ADAPTER_ADDRESSES* adapter = addresses; adapter != nullptr; adapter = adapter->Next) {
        auto* bsd_name = adapter->AdapterName;
        if (bsd_name == nullptr) {
            continue;
        }

        auto it = std::find_if(
            network_interfaces.begin(), network_interfaces.end(),
            [&bsd_name](const network_interface& network_interface) {
                return network_interface.bsd_name() == bsd_name;
            }
        );

        if (it == network_interfaces.end()) {
            it = network_interfaces.emplace(it, bsd_name);
        }

        if (adapter->Description != nullptr) {
            auto length = wcslen(adapter->Description);
            std::string result_str;
            result_str.resize(length * sizeof(wchar_t));

            std::locale loc("C");

            std::use_facet<std::ctype<wchar_t> >(loc).narrow(
                adapter->Description, adapter->Description + length, '?',  &*result_str.begin());

            it->set_display_name(result_str);
        }
    }
#endif

    return network_interfaces;
}
