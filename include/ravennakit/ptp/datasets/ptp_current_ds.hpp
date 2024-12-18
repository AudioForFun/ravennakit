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

#include "ravennakit/ptp/types/ptp_timestamp.hpp"

#include <cstdint>

namespace rav {

/**
 * Represents the data set as described in IEEE1588-2019: 8.2.2.
 */
struct ptp_current_ds {
    uint16_t steps_removed {0};
    ptp_time_interval offset_from_master {0};
    ptp_time_interval mean_delay {0};

    void update(const ptp_state_decision_code state_decision_code, const ptp_announce_message& announce_message) {
        if (state_decision_code == ptp_state_decision_code::s1) {
            steps_removed = 1 + announce_message.steps_removed;
        } else {
            RAV_ASSERT_FALSE("Unsupported state decision code");
        }
    }
};

}  // namespace rav
