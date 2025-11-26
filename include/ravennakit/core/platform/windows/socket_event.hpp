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

#include "ravennakit/core/exception.hpp"
#include "ravennakit/core/platform.hpp"

#if RAV_WINDOWS

    #include <winsock2.h>

namespace rav::windows {

/**
 * Wrapper around WSACreateEvent and WSACloseEvent.
 */
class socket_event {
  public:
    /**
     * Constructs a socket event.
     * @throws rav::exception if WSACreateEvent fails.
     */
    socket_event() {
        event_ = WSACreateEvent();
        if (event_ == WSA_INVALID_EVENT) {
            RAV_THROW_EXCEPTION("WSACreateEvent failed");
        }
    }

    virtual ~socket_event() {
        if (event_ != WSA_INVALID_EVENT) {
            if (WSACloseEvent(event_) == false) {
                RAV_LOG_ERROR("WSACloseEvent failed");
            }
        }
    }

    /**
     *
     * @returns The underlying WSAEVENT.
     */
    [[nodiscard]] WSAEVENT get() const {
        return event_;
    }

    /**
     * Resets the event (WSAResetEvent).
     */
    void reset_event() {
        if (WSAResetEvent(event_) == SOCKET_ERROR) {
            RAV_THROW_EXCEPTION("WSAResetEvent failed");
        }
    }

    /**
     * Associates the event with a socket (WSAEventSelect).
     * @param socket The socket to associate with.
     */
    void associate(SOCKET socket) {
        if (WSAEventSelect(socket, event_, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR) {
            RAV_THROW_EXCEPTION("WSAEventSelect failed");
        }
    }

  private:
    WSAEVENT event_;
};

}  // namespace rav::windows

#endif
