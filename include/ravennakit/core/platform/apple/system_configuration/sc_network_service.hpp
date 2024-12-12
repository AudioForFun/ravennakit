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
class sc_network_service: public cf_type<SCNetworkServiceRef> {
  public:
    explicit sc_network_service(const SCNetworkServiceRef service, const bool retain) : cf_type(service, retain) {}

    /**
     * @return The name of the service.
     */
    [[nodiscard]] std::string get_name() const {
        return cf_string::to_string(SCNetworkServiceGetName(get()));
    }

    /**
     * @return The interface of the service.
     */
    [[nodiscard]] cf_array<SCNetworkProtocolRef> get_protocols() const {
        return cf_array<SCNetworkProtocolRef>(SCNetworkServiceCopyProtocols(get()), false);
    }
};

}  // namespace rav

#endif
