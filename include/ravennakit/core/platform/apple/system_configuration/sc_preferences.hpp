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

#include "ravennakit/core/platform.hpp"

#if RAV_APPLE
    #include "../core_foundation/cf_type.hpp"

    #include <SystemConfiguration/SystemConfiguration.h>

namespace rav {

/**
 * An RAII wrapper around SCNetworkInterfaceRef.
 */
class sc_preferences: public cf_type<SCPreferencesRef> {
  public:
    sc_preferences() : cf_type(SCPreferencesCreate(nullptr, CFSTR("RAVENNAKIT"), nullptr), false) {}

    /**
     * @return All network services.
     */
    [[nodiscard]] cf_array<SCNetworkServiceRef> get_network_services() const {
        return cf_array<SCNetworkServiceRef>(SCNetworkServiceCopyAll(get()), false);
    }

    /**
     * @return All network interfaces.
     */
    static cf_array<SCNetworkInterfaceRef> get_network_interfaces() {
        return cf_array<SCNetworkInterfaceRef>(SCNetworkInterfaceCopyAll(), false);
    }
};

}  // namespace rav

#endif
