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

#include "ravennakit/ptp/messages/ptp_announce_message.hpp"
#include "ravennakit/ptp/types/ptp_port_identity.hpp"

#include <vector>

namespace rav {

class ptp_foreign_master_list {
  public:
    static constexpr size_t k_foreign_master_time_window = 4;  // 4 announce intervals
    static constexpr size_t k_foreign_master_threshold = 2;    // 2 announce messages within the time window

    struct entry {
        /// The identity of the foreign master.
        ptp_port_identity foreign_master_port_identity;
        /// number of messages received within k_foreign_master_time_window.
        size_t foreign_master_announce_messages {};
        /// The most recent announce message received from the foreign master.
        std::optional<ptp_announce_message> most_recent_announce_message;
    };

    explicit ptp_foreign_master_list(const ptp_port_identity& port_identity) : port_identity_(port_identity) {}

    [[nodiscard]] bool is_announce_message_qualified(const ptp_announce_message& announce_message) const {
        const auto foreign_port_identity = announce_message.header.source_port_identity;

        // IEEE 1588-2019: 9.3.2.5
        // a) Message must not come from the same PTP instance. Since every port has the same clock identity, we can
        // just compare that.
        if (foreign_port_identity.clock_identity == port_identity_.clock_identity) {
            return false;
        }

        // b) If message is not the most recent one from the foreign master, discard it.
        if (auto* entry = find_entry(foreign_port_identity)) {
            if (auto msg = entry->most_recent_announce_message) {
                if (msg->header.sequence_id >= announce_message.header.sequence_id) {
                    return false;
                }
            }
        }

        return true;
    }

  private:
    std::vector<entry> entries_;
    ptp_port_identity port_identity_;

    [[nodiscard]] const entry* find_entry(const ptp_port_identity& foreign_master_port_identity) const {
        for (auto& entry : entries_) {
            if (entry.foreign_master_port_identity == foreign_master_port_identity) {
                return &entry;
            }
        }

        return nullptr;
    }
};

}  // namespace rav
