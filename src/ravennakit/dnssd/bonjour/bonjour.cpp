/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/dnssd/bonjour/bonjour.hpp"

#if RAV_HAS_APPLE_DNSSD

bool rav::dnssd::is_bonjour_service_running() {
    uint32_t version;
    uint32_t size = sizeof(version);
    const DNSServiceErrorType error = DNSServiceGetProperty(kDNSServiceProperty_DaemonVersion, &version, &size);

    return error == kDNSServiceErr_NoError;
}

#endif
