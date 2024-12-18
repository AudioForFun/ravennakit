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

#include "ravennakit/ptp/ptp_definitions.hpp"

#include <cstdint>

namespace rav {

/**
 * Represents the time properties data set as described in IEEE1588-2019: 8.2.4.
 */
struct ptp_time_properties_ds {
    int16_t current_utc_offset{};
    bool current_utc_offset_valid{};
    bool leap59{};
    bool leap61{};
    bool time_traceable{};
    bool frequency_traceable{};
    bool ptp_timescale{};
    ptp_time_source time_source{};

    void update(const ptp_state_decision_code state_decision_code, const ptp_announce_message& announce_message) {
        if (state_decision_code == ptp_state_decision_code::s1) {
            current_utc_offset = announce_message.current_utc_offset;
            current_utc_offset_valid = announce_message.header.flags.current_utc_offset_valid;
            leap59 = announce_message.header.flags.leap59;
            leap61 = announce_message.header.flags.leap61;
            time_traceable = announce_message.header.flags.time_traceable;
            frequency_traceable = announce_message.header.flags.frequency_traceable;
            ptp_timescale = announce_message.header.flags.ptp_timescale;
            time_source = announce_message.time_source;
        } else {
            RAV_ASSERT_FALSE("Unsupported state decision code");
        }
    }
};

}
