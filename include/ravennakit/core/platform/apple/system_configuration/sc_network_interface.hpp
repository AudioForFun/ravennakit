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

#include "ravennakit/core/platform.hpp"

#if RAV_APPLE
    #include "../core_foundation/cf_type.hpp"
    #include "../core_foundation/cf_string.hpp"

    #include <SystemConfiguration/SystemConfiguration.h>

namespace rav {

/**
 * An RAII wrapper around SCNetworkServiceRef.
 */
class ScNetworkInterface: public CfType<SCNetworkInterfaceRef> {
  public:
    ScNetworkInterface(const SCNetworkInterfaceRef interface, const bool retain) : CfType(interface, retain) {}

    /**
     * @return The BSD name of the interface.
     */
    [[nodiscard]] std::string get_bsd_name() const {
        return CfString::to_string(SCNetworkInterfaceGetBSDName(get()));
    }

    /**
     * @return The localized display name of the interface.
     */
    [[nodiscard]] std::string get_localized_display_name() const {
        return CfString::to_string(SCNetworkInterfaceGetLocalizedDisplayName(get()));
    }

    /**
     * @returns The type of the interface.
     */
    [[nodiscard]] std::string get_type() const {
        return CfString::to_string(SCNetworkInterfaceGetInterfaceType(get()));
    }
};

}  // namespace rav

#endif
