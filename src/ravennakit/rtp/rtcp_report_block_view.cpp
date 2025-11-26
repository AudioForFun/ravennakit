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

#include "ravennakit/rtp/rtcp_report_block_view.hpp"
#include "ravennakit/core/byte_order.hpp"

#include <array>

rav::rtcp::ReportBlockView::ReportBlockView(const uint8_t* data, const size_t size_bytes) : data_(data), size_bytes_(size_bytes) {}

bool rav::rtcp::ReportBlockView::validate() const {
    if (data_ == nullptr) {
        return false;
    }

    if (size_bytes_ < k_report_block_length_length) {
        return false;
    }

    if (size_bytes_ > k_report_block_length_length) {
        return false;
    }

    return true;
}

uint32_t rav::rtcp::ReportBlockView::ssrc() const {
    return read_be<uint32_t>(data_);
}

uint8_t rav::rtcp::ReportBlockView::fraction_lost() const {
    return data_[4];
}

uint32_t rav::rtcp::ReportBlockView::number_of_packets_lost() const {
    const std::array<uint8_t, 4> packets_lost {0, data_[5], data_[6], data_[7]};
    return read_be<uint32_t>(packets_lost.data());
}

uint32_t rav::rtcp::ReportBlockView::extended_highest_sequence_number_received() const {
    return read_be<uint32_t>(data_ + 8);
}

uint32_t rav::rtcp::ReportBlockView::inter_arrival_jitter() const {
    return read_be<uint32_t>(data_ + 12);
}

rav::ntp::Timestamp rav::rtcp::ReportBlockView::last_sr_timestamp() const {
    return ntp::Timestamp::from_compact(read_be<uint32_t>(data_ + 16));
}

uint32_t rav::rtcp::ReportBlockView::delay_since_last_sr() const {
    return read_be<uint32_t>(data_ + 20);
}

const uint8_t* rav::rtcp::ReportBlockView::data() const {
    return data_;
}

size_t rav::rtcp::ReportBlockView::size() const {
    return size_bytes_;
}
