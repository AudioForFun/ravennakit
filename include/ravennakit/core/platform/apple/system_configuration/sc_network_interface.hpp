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
    #include "../core_foundation/cf_string.hpp"

    #include <SystemConfiguration/SystemConfiguration.h>

namespace rav {

/**
 * An RAII wrapper around SCNetworkServiceRef.
 */
class sc_network_interface : public cf_type<SCNetworkInterfaceRef> {
  public:
    sc_network_interface(const SCNetworkInterfaceRef interface, const bool retain) : cf_type(interface, retain) {}

    /**
     * @return The BSD name of the interface.
     */
    [[nodiscard]] std::string get_bsd_name() const {
        return cf_string::to_string(SCNetworkInterfaceGetBSDName(get()));
    }

    /**
     * @return The localized display name of the interface.
     */
    [[nodiscard]] std::string get_localized_display_name() const {
        return cf_string::to_string(SCNetworkInterfaceGetLocalizedDisplayName(get()));
    }

    /**
     * @returns The type of the interface.
     */
    [[nodiscard]] std::string get_type()const {
        return cf_string::to_string(SCNetworkInterfaceGetInterfaceType(get()));
    }
};

}  // namespace rav

#endif
