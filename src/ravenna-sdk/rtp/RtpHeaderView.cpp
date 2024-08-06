/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravenna-sdk/rtp/RtpHeaderView.hpp"

#include <fmt/core.h>

#include <asio/detail/socket_ops.hpp>

#include "ravenna-sdk/platform/ByteOrder.hpp"

namespace {
constexpr size_t kHeaderBaseLength = 12;
}

rav::RtpHeaderView::RtpHeaderView(const uint8_t* data, const size_t data_length) : data_(data), data_length_(data_length) {}

rav::RtpHeaderView::ValidationResult rav::RtpHeaderView::validate() const {
    if (data_ == nullptr) {
        return ValidationResult::InvalidPointer;
    }

    if (data_length_ < kHeaderBaseLength) {
        return ValidationResult::InvalidHeaderLength;
    }

    if (data_length_ < kHeaderBaseLength + csrc_count() * sizeof(uint32_t)) {
        return ValidationResult::InvalidHeaderLength;
    }

    if (version() > 2) {
        return ValidationResult::InvalidVersion;
    }

    return ValidationResult::Ok;
}

bool rav::RtpHeaderView::marker_bit() const {
    if (data_length_ < 1) {
        return false;
    }
    return (data_[1] & 0b10000000) >> 7 != 0;
}

uint8_t rav::RtpHeaderView::payload_type() const {
    if (data_length_ < 1) {
        return false;
    }
    return data_[1] & 0b01111111;
}

uint16_t rav::RtpHeaderView::sequence_number() const {
    constexpr auto kOffset = 2;
    if (data_length_ < kOffset + sizeof(uint16_t)) {
        return 0;
    }

    return byte_order::read_be<uint16_t>(&data_[kOffset]);
}

uint32_t rav::RtpHeaderView::timestamp() const {
    constexpr auto kOffset = 4;
    if (data_length_ < kOffset + sizeof(uint32_t)) {
        return 0;
    }
    return byte_order::read_be<uint32_t>(&data_[kOffset]);
}

uint32_t rav::RtpHeaderView::ssrc() const {
    constexpr auto kOffset = 8;
    if (data_length_ < kOffset + sizeof(uint32_t)) {
        return 0;
    }
    return byte_order::read_be<uint32_t>(&data_[kOffset]);
}

uint8_t rav::RtpHeaderView::version() const {
    if (data_length_ < 1) {
        return 0;
    }
    return (data_[0] & 0b11000000) >> 6;
}

bool rav::RtpHeaderView::padding() const {
    if (data_length_ < 1) {
        return false;
    }
    return (data_[0] & 0b00100000) >> 5 != 0;
}

bool rav::RtpHeaderView::extension() const {
    if (data_length_ < 1) {
        return false;
    }
    return (data_[0] & 0b00010000) >> 4 != 0;
}

uint32_t rav::RtpHeaderView::csrc_count() const {
    if (data_length_ < 1) {
        return 0;
    }
    return data_[0] & 0b00001111;
}

uint32_t rav::RtpHeaderView::csrc(const uint32_t index) const {
    if (index >= csrc_count()) {
        return 0;
    }
    return byte_order::read_be<uint32_t>(&data_[kHeaderBaseLength + index * sizeof(uint32_t)]);
}

uint16_t rav::RtpHeaderView::get_header_extension_defined_by_profile() const {
    if (!extension()) {
        return 0;
    }

    const auto header_extension_start_index = kHeaderBaseLength + csrc_count() * sizeof(uint32_t);
    uint16_t data;
    std::memcpy(&data, &data_[header_extension_start_index], sizeof(data));
    return data;
}

rav::BufferView<const uint8_t> rav::RtpHeaderView::get_header_extension_data() const {
    if (!extension()) {
        return {};
    }

    const auto header_extension_start_index = kHeaderBaseLength + csrc_count() * sizeof(uint32_t);

    auto length = byte_order::read_be<uint16_t>(&data_[header_extension_start_index + sizeof(uint16_t)]);
    const auto data_start_index = header_extension_start_index + sizeof(uint16_t) * 2;
    return {&data_[data_start_index], length};
}

std::string rav::RtpHeaderView::to_string() const {
    return fmt::format(
        "RTP Header: valid={} version={} padding={} extension={} csrc_count={} market_bit={} payload_type={} sequence_number={} timestamp={} ssrc={}",
        validate() == ValidationResult::Ok,
        version(),
        padding(),
        extension(),
        csrc_count(),
        marker_bit(),
        payload_type(),
        sequence_number(),
        timestamp(),
        ssrc()
    );
}
