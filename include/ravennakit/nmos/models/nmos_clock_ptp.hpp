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
#include <string>
#include <boost/json/conversion.hpp>

namespace rav::nmos {

/**
 * Describes a clock referenced to PTP.
 */
struct ClockPtp {
    /// Name of this refclock (unique for this set of clocks). Must start with "clk".
    std::string name;

    /// Type of external reference used by this clock
    static constexpr auto k_ref_type_ptp = "ptp";

    /// External refclock is synchronized to International Atomic Time (TAI)
    bool traceable {false};

    /// Version of PTP reference used by this clock
    static constexpr auto k_version = "IEEE1588-2008";

    /// ID of the PTP reference used by this clock (e.g. "00-1a-2b-00-00-3c-4d-5e")
    std::string gmid;

    /// Lock-state of this clock to the external reference. If true, this device follows the external reference;
    /// otherwise it has no defined relationship to the external reference
    bool locked {false};
};

inline void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const ClockPtp& clock) {
    jv = {{"name", clock.name},           {"ref_type", ClockPtp::k_ref_type_ptp},
          {"traceable", clock.traceable}, {"version", ClockPtp::k_version},
          {"gmid", clock.gmid},           {"locked", clock.locked}};
}

}  // namespace rav::nmos
