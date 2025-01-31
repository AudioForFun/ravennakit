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
#include "ravennakit/core/util/wrapping_uint.hpp"
#include "ravennakit/rtp/rtp_packet_view.hpp"

namespace rav {

/**
 * A class that collects statistics about RTP packets.
 */
class rtp_packet_stats {
  public:
    struct counters {
        uint16_t out_of_order {};
        uint16_t too_old {};
        uint16_t duplicates {};
        uint16_t dropped {};
    };

    explicit rtp_packet_stats() = default;

    /**
     * Updates the statistics with the given packet.
     * @param sequence_number
     * @return True if packet should be processed, or false if it should be dropped because it is too old.
     */
    void update(const uint16_t sequence_number) {
        const auto packet_sequence_number = wrapping_uint16(sequence_number);
        if (first_packet_) {
            last_collected_sequence_number_ = packet_sequence_number - 1;
            first_packet_ = false;
        }
        if (packet_sequence_number <= last_collected_sequence_number_) {
            num_too_old_++;
            return;
        }

        packets_.at(sequence_number % packets_.size()).times_received++;

        const auto diff = most_recent_sequence_number_.update(sequence_number);

        for (uint16_t i = 1; i < diff; i++) {
            packets_.at((sequence_number - i) % packets_.size()).out_of_order = true;
        }
    }

    /**
     * Collects the statistics up to and including the given sequence number.
     * @param until_sequence_number The sequence number to collect statistics up to.
     * @return The collected statistics.
     */
    counters collect(const wrapping_uint16 until_sequence_number) {
        if (until_sequence_number <= last_collected_sequence_number_) {
            RAV_WARNING("Invalid sequence number");
            return {};
        }

        counters result {};
        result.too_old = num_too_old_;
        num_too_old_ = 0;

        for (auto i = last_collected_sequence_number_ + 1; i <= until_sequence_number; i += 1) {
            auto& packet = packets_.at(i.value() % packets_.size());
            if (packet.times_received == 0) {
                result.dropped++;
            } else if (packet.times_received > 1) {
                result.duplicates++;
            } else if (packet.out_of_order) {
                result.out_of_order++;
            }
            packet = {};
        }

        last_collected_sequence_number_ = until_sequence_number;
        return result;
    }

    /**
     * Resets to the initial state.
     */
    void reset() {
        first_packet_ = true;
        std::fill(packets_.begin(), packets_.end(), packet {});
        most_recent_sequence_number_ = {};
        last_collected_sequence_number_ = {};
        num_too_old_ = 0;
    }

  private:
    struct packet {
        uint16_t times_received {};
        bool out_of_order {};
    };

    bool first_packet_ {true};
    wrapping_uint16 most_recent_sequence_number_ {};
    wrapping_uint16 last_collected_sequence_number_ {};
    uint16_t num_too_old_ {};
    std::vector<packet> packets_ {1024};  // TODO: Determine size
};

}  // namespace rav
