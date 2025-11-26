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

#include "ravennakit/dnssd/dnssd_service_description.hpp"

#include <sstream>

bool rav::dnssd::ServiceDescription::resolved() const {
    return host_target.empty() == false && port != 0;
}

std::string rav::dnssd::ServiceDescription::to_string() const noexcept {
    std::string txtRecordDescription;

    for (auto& kv : txt) {
        txtRecordDescription += kv.first;
        txtRecordDescription += "=";
        txtRecordDescription += kv.second;
        txtRecordDescription += ", ";
    }

    std::string addressesDescription;

    for (auto& interface : interfaces) {
        addressesDescription += "interface ";
        addressesDescription += std::to_string(interface.first);
        addressesDescription += ": ";

        for (auto& addr : interface.second) {
            addressesDescription += addr;
            addressesDescription += ", ";
        }
    }

    std::stringstream output;

    output << "fullname: " << fullname << ", name: " << name << ", type: " << reg_type << ", domain: " << domain
           << ", host_target: " << host_target << ", port: " << port << ", txt_record: " << txtRecordDescription
           << "addresses: " << addressesDescription;

    return output.str();
}
