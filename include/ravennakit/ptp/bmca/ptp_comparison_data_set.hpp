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

#pragma once
#include "ravennakit/ptp/datasets/ptp_default_ds.hpp"
#include "ravennakit/ptp/datasets/ptp_port_ds.hpp"
#include "ravennakit/ptp/messages/ptp_announce_message.hpp"
#include "ravennakit/ptp/types/ptp_clock_quality.hpp"
#include "ravennakit/ptp/types/ptp_port_identity.hpp"

#include <cstdint>

namespace rav::ptp {

/**
 * Contains the data needed to compare two PTP data sets.
 */
struct ComparisonDataSet {
    uint8_t grandmaster_priority1 {};
    ClockIdentity grandmaster_identity;
    ClockQuality grandmaster_clock_quality;
    uint8_t grandmaster_priority2 {};
    uint16_t steps_removed {};
    ClockIdentity identity_of_senders;
    PortIdentity identity_of_receiver;

    /**
     * The result of comparing two PTP data sets.
     * Note: the order of the enum values is important for the comparison logic.
     */
    enum class result {
        worse,
        /// The set is worse than the one being compared to.
        /// The set is of equal quality as the one being compared to, but is worse by the topology.
        worse_by_topology,
        /// Both sets are equal. Indicates that one of the PTP messages was transmitted and received on the same PTP
        /// Port.
        error1,
        /// Both sets are equal. Indicates that the PTP messages are duplicates or that they are an earlier and later
        /// PTP message from the
        /// same Grandmaster PTP Instance
        error2,
        /// The set is of equal quality as the one being compared to, but is preferred by the topology.
        better_by_topology,
        /// The set is better than the one being compared to.
        better,
    };

    ComparisonDataSet() = default;

    /**
     * Constructs a set from an announce message and a port data set.
     * @param announce_message The announce message.
     * @param receiver_identity The identity of the receiver.
     */
    ComparisonDataSet(const AnnounceMessage& announce_message, const PortIdentity& receiver_identity);

    /**
     * Constructs a set from an announce message and a port data set.
     * @param announce_message The announce message.
     * @param port_ds The port data set.
     */
    ComparisonDataSet(const AnnounceMessage& announce_message, const PortDs& port_ds);

    /**
     * Constructs a set from a default data set.
     * @param default_ds The default data set.
     */
    explicit ComparisonDataSet(const DefaultDs& default_ds);

    /**
     * Compares this data set to another. The comparison is done according to the rules in IEEE 1588-2019 9.3.4.
     * @param other The other data set to compare to.
     * @return The result of the comparison. See the result enum for more information.
     */
    [[nodiscard]] result compare(const ComparisonDataSet& other) const;

    /**
     * Convenience method for comparing two announce messages.
     * @param a The first announce message.
     * @param b The second announce message.
     * @param receiver_identity The identity of the receiver.
     * @return The result of the comparison. See the result enum for more information.
     */
    static result compare(const AnnounceMessage& a, const AnnounceMessage& b, const PortIdentity& receiver_identity);
};

}  // namespace rav::ptp
