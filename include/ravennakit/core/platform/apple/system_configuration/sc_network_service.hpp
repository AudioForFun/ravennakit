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
class ScNetworkService: public CfType<SCNetworkServiceRef> {
  public:
    explicit ScNetworkService(const SCNetworkServiceRef service, const bool retain) : CfType(service, retain) {}

    /**
     * @return The name of the service.
     */
    [[nodiscard]] std::string get_name() const {
        return CfString::to_string(SCNetworkServiceGetName(get()));
    }

    /**
     * @return The interface of the service.
     */
    [[nodiscard]] CfArray<SCNetworkProtocolRef> get_protocols() const {
        return CfArray<SCNetworkProtocolRef>(SCNetworkServiceCopyProtocols(get()), false);
    }
};

}  // namespace rav

#endif
