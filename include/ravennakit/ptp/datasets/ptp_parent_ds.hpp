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

#include "ptp_default_ds.hpp"
#include "ravennakit/ptp/messages/ptp_announce_message.hpp"
#include "ravennakit/ptp/types/ptp_clock_quality.hpp"
#include "ravennakit/ptp/types/ptp_port_identity.hpp"

namespace rav {

/**
 * Represents the parent data set as described in IEEE1588-2019: 8.2.3.
 */
struct ptp_parent_ds {
    ptp_port_identity parent_port_identity;
    bool parent_stats {};
    ptp_clock_identity grandmaster_identity;
    ptp_clock_quality grandmaster_clock_quality;
    uint16_t grandmaster_priority1 {};
    uint8_t grandmaster_priority2 {};

    ptp_parent_ds() = default;

    explicit ptp_parent_ds(const ptp_default_ds& default_ds) {
        parent_port_identity.clock_identity = default_ds.clock_identity;  // IEEE1588-2019: 8.2.3.2
        parent_stats = false;                                             // IEEE1588-2019: 8.2.3.3
        grandmaster_identity = default_ds.clock_identity;                 // IEEE1588-2019: 8.2.3.6
        grandmaster_clock_quality = default_ds.clock_quality;             // IEEE1588-2019: 8.2.3.7
        grandmaster_priority1 = default_ds.priority1;                     // IEEE1588-2019: 8.2.3.8
        grandmaster_priority2 = default_ds.priority2;                     // IEEE1588-2019: 8.2.3.9
    }

    void update(const ptp_state_decision_code state_decision_code, const ptp_announce_message& announce_message) {
        if (state_decision_code == ptp_state_decision_code::s1) {
            parent_port_identity = announce_message.header.source_port_identity;
            grandmaster_identity = announce_message.grandmaster_identity;
            grandmaster_clock_quality = announce_message.grandmaster_clock_quality;
            grandmaster_priority1 = announce_message.grandmaster_priority1;
            grandmaster_priority2 = announce_message.grandmaster_priority2;
        } else {
            RAV_ASSERT_FALSE("Unsupported state decision code");
        }
    }
};

}  // namespace rav
