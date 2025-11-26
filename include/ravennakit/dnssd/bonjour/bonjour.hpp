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
#include "ravennakit/core/exception.hpp"

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

    #define DNSSD_THROW_IF_ERROR(result, msg)                                                                                      \
        if (result != kDNSServiceErr_NoError) {                                                                                    \
            throw rav::Exception(std::string(msg) + ": " + dns_service_error_to_string(result), __FILE__, __LINE__, RAV_FUNCTION); \
        }

    #define DNSSD_LOG_IF_ERROR(error)                                             \
        if (error != kDNSServiceErr_NoError) {                                    \
            RAV_LOG_ERROR("DNSServiceError: {}", dns_service_error_to_string(error)); \
        }

namespace rav::dnssd {

bool is_bonjour_service_running();
const char* dns_service_error_to_string(DNSServiceErrorType error) noexcept;

}  // namespace rav::dnssd

#endif
