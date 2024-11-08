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

#include "ravennakit/sdp/constants.hpp"

#include <asio.hpp>
#include <tl/expected.hpp>

namespace rav {

/**
 * Implements logic for filtering RTP packets.
 */
class rtp_filter {
  public:
    rtp_filter() = default;

    explicit rtp_filter(asio::ip::address connection_address) : connection_address_(std::move(connection_address)) {}

    rtp_filter(const rtp_filter&) = default;
    rtp_filter& operator=(const rtp_filter&) = default;

    rtp_filter(rtp_filter&&) noexcept = default;
    rtp_filter& operator=(rtp_filter&&) noexcept = default;

    void add_filter(asio::ip::address address, const sdp::filter_mode mode) {
        RAV_TRACE(
            "Added source filter: {} {} {}", sdp::to_string(mode), connection_address_.to_string(), address.to_string()
        );
        filters_.push_back({mode, std::move(address)});
    }

    size_t add_filter(const sdp::source_filter& filter) {
        size_t total = 0;
        const auto dest_address = asio::ip::make_address(filter.dest_address());
        if (dest_address != connection_address_) {
            return 0;
        }
        for (auto& src : filter.src_list()) {
            add_filter(asio::ip::make_address(src), filter.mode());
            total++;
        }
        return total;
    }

    size_t add_filters(const std::vector<sdp::source_filter>& filters) {
        size_t total = 0;
        for (auto& filter : filters) {
            total += add_filter(filter);
        }
        return total;
    }

    [[nodiscard]] bool matches(const asio::ip::address& connection_address) const {
        return connection_address_ == connection_address;
    }

    [[nodiscard]] bool
    matches(const asio::ip::address& connection_address, const asio::ip::address& src_address) const {
        if (connection_address_ != connection_address) {
            return false;
        }

        if (filters_.empty()) {
            return true;
        }

        bool is_address_included = false;
        bool has_include_filters = false;

        for (auto& filter : filters_) {
            if (filter.mode == sdp::filter_mode::exclude && filter.address == src_address) {
                return false;  // This prioritizes exclude filters over include filters
            }
            if (filter.mode == sdp::filter_mode::include) {
                has_include_filters = true;
                if (filter.address == src_address) {
                    is_address_included = true;
                }
            }
        }

        return has_include_filters ? is_address_included : true;
    }

    [[nodiscard]] bool empty() const {
        return filters_.empty();
    }

  private:
    struct filter {
        sdp::filter_mode mode {sdp::filter_mode::undefined};
        asio::ip::address address;
    };

    asio::ip::address connection_address_;
    std::vector<filter> filters_;
};

}  // namespace rav
