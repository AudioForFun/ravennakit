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
    #define RAV_HAS_APPLE_DNSSD 1
#elif RAV_WINDOWS
    #define RAV_HAS_APPLE_DNSSD 1
#else
    #define RAV_HAS_APPLE_DNSSD 0
#endif

#if RAV_HAS_APPLE_DNSSD

    #ifdef _WIN32
        #define _WINSOCKAPI_  // Prevents inclusion of winsock.h in windows.h
        #include <Ws2tcpip.h>
        #include <winsock2.h>
    #else
        #include <arpa/inet.h>
    #endif

    #include <dns_sd.h>

namespace rav::dnssd {

bool is_bonjour_service_running();

}  // namespace rav::dnssd

#endif
