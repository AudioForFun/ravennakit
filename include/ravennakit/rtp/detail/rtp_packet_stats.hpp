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
#include "ravennakit/core/containers/ring_buffer.hpp"
#include "ravennakit/core/containers/detail/fifo.hpp"
#include "ravennakit/core/util/wrapping_uint.hpp"
#include "ravennakit/rtp/rtp_packet_view.hpp"

namespace rav {

/**
 * A class that collects statistics about RTP packets.
 */
class rtp_packet_stats {
  public:
    struct counters {
        /// The number of packets which arrived out of order.
        uint32_t out_of_order {};
        /// The number of packets which were duplicates.
        uint32_t duplicates {};
        /// The number of packets which were dropped.
        uint32_t dropped {};
        /// The number of packets which were too late for consumer.
        uint32_t too_late {};
        /// The number of packets which were too old for the window.
        uint32_t too_old {};

        [[nodiscard]] auto tie() const {
            return std::tie(out_of_order, too_late, duplicates, dropped);
        }

        friend bool operator==(const counters& lhs, const counters& rhs) {
            return lhs.tie() == rhs.tie();
        }

        friend bool operator!=(const counters& lhs, const counters& rhs) {
            return lhs.tie() != rhs.tie();
        }
    };

    explicit rtp_packet_stats() = default;

    /**
     * @param window_size The window size in number of packets. Max value is 65535 (0xffff).
     */
    explicit rtp_packet_stats(const size_t window_size) : window_(window_size) {
        RAV_ASSERT(window_size <= std::numeric_limits<uint16_t>::max(), "Window size too large");
    }

    /**
     * Updates the statistics with the given packet.
     * @param sequence_number
     * @return True if packet should be processed, or false if it should be dropped because it is too old.
     */
    void update(const uint16_t sequence_number) {
        const auto packet_sequence_number = wrapping_uint16(sequence_number);

        if (!most_recent_sequence_number_.has_value()) {
            most_recent_sequence_number_ = packet_sequence_number - 1;
        }

        const auto diff = most_recent_sequence_number_->update(sequence_number);
        for (uint16_t i = 0; i < diff; ++i) {
            if (window_.full()) {
                collect_packet_counts();
            }
            window_.push_back({});
        }

        const auto idx = window_.size() - 1 - (*most_recent_sequence_number_ - sequence_number).value();

        if (idx >= window_.size()) {
            total_counts_.too_old++;
            return;  // Too old for the window
        }

        auto& packet = window_[idx];
        packet.times_received++;

        if (packet_sequence_number < *most_recent_sequence_number_) {
            packet.times_out_of_order++;  // Packet out of order
        }
    }

    /**
     * Collects the statistics for the current window.
     * @return The collected statistics.
     */
    [[nodiscard]] counters get_window_counts() const {
        if (window_.empty()) {
            return {};
        }

        counters result {};
        for (const auto& packet : window_) {
            if (packet.times_received == 0) {
                result.dropped++;
            } else if (packet.times_received > 1) {
                result.duplicates += packet.times_received - 1;
            }
            result.out_of_order += packet.times_out_of_order;
            result.too_late += packet.times_too_late;
        }
        return result;
    }

    /**
     * @return The total counts. This will only contain the numbers of packets which are moved out of the window.
     */
    [[nodiscard]] counters get_total_counts() const {
        return total_counts_;
    }

    /**
     * Collects the statistics for the current window, resets the window and returns the collected statistics. After
     * calling this function you probably want to call reset() because subsequent updates will result in wrong numbers.
     * @return The collected statistics.
     */
    counters collect_total_counts() {
        while (collect_packet_counts()) {}
        return total_counts_;
    }

    /**
     * Marks a packet as too late which means it didn't arrive in time for the consumer.
     */
    void mark_packet_too_late(const uint16_t sequence_number) {
        if (!most_recent_sequence_number_) {
            return;  // Can't mark a packet too late which never arrived
        }
        if (sequence_number > most_recent_sequence_number_->value()) {
            return;  // Can't mark a packet too late which is newer than the most recent packet
        }
        const auto offset = (*most_recent_sequence_number_ - sequence_number).value();
        if (offset >= window_.size()) {
            return;  // Too old for the window
        }
        window_[window_.size() - 1 - offset].times_too_late++;
    }

    /**
     * @return The number of packets in the window.
     */
    [[nodiscard]] size_t size() const {
        return window_.size();
    }

    /**
     * Resets to the initial state.
     * @param window_size The window size in number of packets. Max value is 65535 (0xffff).
     */
    void reset(const std::optional<uint16_t> window_size = {}) {
        if (window_size.has_value()) {
            RAV_ASSERT(*window_size <= std::numeric_limits<uint16_t>::max(), "Window size too large");
        }
        window_.reset(window_size);
        most_recent_sequence_number_ = {};
        total_counts_ = {};
    }

  private:
    struct packet {
        uint16_t times_received {};
        uint16_t times_out_of_order {};
        uint16_t times_too_late {};
    };

    std::optional<wrapping_uint16> most_recent_sequence_number_ {};
    counters total_counts_ {};
    ring_buffer<packet> window_ {32};

    bool collect_packet_counts() {
        const auto pkt = window_.pop_front();
        if (!pkt.has_value()) {
            return false;
        }
        if (pkt->times_received == 0) {
            total_counts_.dropped++;
        }
        if (pkt->times_received > 1) {
            total_counts_.duplicates += pkt->times_received - 1;
        }
        if (pkt->times_out_of_order > 0) {
            total_counts_.out_of_order += pkt->times_out_of_order;
        }
        if (pkt->times_too_late > 0) {
            total_counts_.too_late += pkt->times_too_late;
        }
        return true;
    }
};

}  // namespace rav
