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

#if RAV_WINDOWS

    #include "ravennakit/core/util/defer.hpp"

    #include <winsock2.h>
    #include <mswsock.h>

typedef BOOL(PASCAL* LPFN_WSARECVMSG)(
    SOCKET s, LPWSAMSG lpMsg, LPDWORD lpNumberOfBytesRecvd, LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

namespace rav::windows {

/**
 * A wrapper around the WSARecvMsg function which is retrieved dynamically at runtime.
 */
class wsa_recv_msg_function {
  public:
    /**
     * Constructor which retrieves the WSARecvMsg function.
     */
    wsa_recv_msg_function() {
        SOCKET temp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        Defer close_socket([&temp_sock] {
            closesocket(temp_sock);
        });
        DWORD bytes_returned = 0;
        GUID WSARecvMsg_GUID = WSAID_WSARECVMSG;

        if (WSAIoctl(
                temp_sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &WSARecvMsg_GUID, sizeof(WSARecvMsg_GUID), &wsa_recv_msg_func_,
                sizeof(wsa_recv_msg_func_), &bytes_returned, nullptr, nullptr
            )
            == SOCKET_ERROR) {
            RAV_THROW_EXCEPTION(fmt::format("Failed to get WSARecvMsg function: {}", WSAGetLastError()));
        }
    }

    /**
     * Get the WSARecvMsg function.
     * @return The WSARecvMsg function.
     */
    [[nodiscard]] LPFN_WSARECVMSG get() const {
        return wsa_recv_msg_func_;
    }

    /**
     * Get the global instance of the WSARecvMsg function.
     * @throws std::exception If the function could not be retrieved.
     * @return The global instance of the WSARecvMsg function.
     */
    static LPFN_WSARECVMSG get_global() {
        static wsa_recv_msg_function instance;
        return instance.wsa_recv_msg_func_;
    }

  private:
    LPFN_WSARECVMSG wsa_recv_msg_func_ {};
};

}  // namespace rav::windows

#endif
