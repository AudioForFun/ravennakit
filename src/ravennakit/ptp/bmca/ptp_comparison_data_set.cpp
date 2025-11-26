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

#include "ravennakit/ptp/bmca/ptp_comparison_data_set.hpp"

rav::ptp::ComparisonDataSet::ComparisonDataSet(const AnnounceMessage& announce_message, const PortIdentity& receiver_identity) {
    grandmaster_priority1 = announce_message.grandmaster_priority1;
    grandmaster_identity = announce_message.grandmaster_identity;
    grandmaster_clock_quality = announce_message.grandmaster_clock_quality;
    grandmaster_priority2 = announce_message.grandmaster_priority2;
    steps_removed = announce_message.steps_removed;
    identity_of_senders = announce_message.header.source_port_identity.clock_identity;
    identity_of_receiver = receiver_identity;
}

rav::ptp::ComparisonDataSet::ComparisonDataSet(const AnnounceMessage& announce_message, const PortDs& port_ds) {
    grandmaster_priority1 = announce_message.grandmaster_priority1;
    grandmaster_identity = announce_message.grandmaster_identity;
    grandmaster_clock_quality = announce_message.grandmaster_clock_quality;
    grandmaster_priority2 = announce_message.grandmaster_priority2;
    steps_removed = announce_message.steps_removed;
    identity_of_senders = announce_message.header.source_port_identity.clock_identity;
    identity_of_receiver = port_ds.port_identity;
}

rav::ptp::ComparisonDataSet::ComparisonDataSet(const DefaultDs& default_ds) {
    grandmaster_priority1 = default_ds.priority1;
    grandmaster_identity = default_ds.clock_identity;
    grandmaster_clock_quality = default_ds.clock_quality;
    grandmaster_priority2 = default_ds.priority2;
    steps_removed = 0;
    identity_of_senders = default_ds.clock_identity;
    identity_of_receiver = {default_ds.clock_identity, 0};
}

rav::ptp::ComparisonDataSet::result rav::ptp::ComparisonDataSet::compare(const ComparisonDataSet& other) const {
    if (grandmaster_identity == other.grandmaster_identity) {
        // Compare Steps Removed of A and B:

        if (steps_removed > other.steps_removed + 1) {
            return result::worse;
        }

        if (steps_removed + 1 < other.steps_removed) {
            return result::better;
        }

        // Compare Steps Removed of A and B:

        if (steps_removed > other.steps_removed) {
            if (identity_of_receiver.clock_identity < identity_of_senders) {
                return result::worse;
            }
            if (identity_of_receiver.clock_identity > identity_of_senders) {
                return result::worse_by_topology;
            }
            return result::error1;
        }

        if (steps_removed < other.steps_removed) {
            if (other.identity_of_receiver.clock_identity < other.identity_of_senders) {
                return result::better;
            }
            if (other.identity_of_receiver.clock_identity > other.identity_of_senders) {
                return result::better_by_topology;
            }
            return result::error1;
        }

        // Compare Identities of Senders of A and B:

        if (identity_of_senders > other.identity_of_senders) {
            return result::worse_by_topology;
        }

        if (identity_of_senders < other.identity_of_senders) {
            return result::better_by_topology;
        }

        // Compare Port Numbers of Receivers of A and B:

        if (identity_of_receiver.port_number > other.identity_of_receiver.port_number) {
            return result::worse_by_topology;
        }

        if (identity_of_receiver.port_number < other.identity_of_receiver.port_number) {
            return result::better_by_topology;
        }

        return result::error2;
    }

    // GM Priority1
    if (grandmaster_priority1 < other.grandmaster_priority1) {
        return result::better;
    }
    if (grandmaster_priority1 > other.grandmaster_priority1) {
        return result::worse;
    }

    // GM Clock class
    if (grandmaster_clock_quality.clock_class < other.grandmaster_clock_quality.clock_class) {
        return result::better;
    }
    if (grandmaster_clock_quality.clock_class > other.grandmaster_clock_quality.clock_class) {
        return result::worse;
    }

    // GM Accuracy
    if (grandmaster_clock_quality.clock_accuracy < other.grandmaster_clock_quality.clock_accuracy) {
        return result::better;
    }
    if (grandmaster_clock_quality.clock_accuracy > other.grandmaster_clock_quality.clock_accuracy) {
        return result::worse;
    }

    // GM Offset scaled log variance
    if (grandmaster_clock_quality.offset_scaled_log_variance < other.grandmaster_clock_quality.offset_scaled_log_variance) {
        return result::better;
    }
    if (grandmaster_clock_quality.offset_scaled_log_variance > other.grandmaster_clock_quality.offset_scaled_log_variance) {
        return result::worse;
    }

    // GM Priority 2
    if (grandmaster_priority2 < other.grandmaster_priority2) {
        return result::better;
    }
    if (grandmaster_priority2 > other.grandmaster_priority2) {
        return result::worse;
    }

    // IEEE1588-2019: 7.5.2.4 Ordering of clockIdentity and portIdentity values
    if (grandmaster_identity.data > other.grandmaster_identity.data) {
        return result::better;
    }
    if (grandmaster_identity.data < other.grandmaster_identity.data) {
        return result::worse;
    }

    RAV_ASSERT(identity_of_senders != other.identity_of_senders, "Clock identity senders must not be equal at this point");

    return result::error1;
}

rav::ptp::ComparisonDataSet::result
rav::ptp::ComparisonDataSet::compare(const AnnounceMessage& a, const AnnounceMessage& b, const PortIdentity& receiver_identity) {
    const ComparisonDataSet set_a(a, receiver_identity);
    const ComparisonDataSet set_b(b, receiver_identity);
    return set_a.compare(set_b);
}
