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
#include "ravennakit/core/log.hpp"

#if RAV_WINDOWS

    #include <synchapi.h>
    #include <handleapi.h>

namespace rav::windows {

/**
 * Wrapper around CreateEvent and CloseHandle.
 */
class event {
  public:
    /**
     * Constructs an event.
     * @throws rav::Exception if CreateEvent fails.
     */
    event() {
        HANDLE event = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (event == nullptr) {
            RAV_THROW_EXCEPTION("Failed to create event");
        }
        event_ = event;
    }

    ~event() {
        if (event_ != nullptr) {
            if (CloseHandle(event_) == false) {
                RAV_LOG_ERROR("Failed to close event");
            }
        }
    }

    /**
     * @returns The underlying WSAEVENT.
     */
    [[nodiscard]] WSAEVENT get() const {
        return event_;
    }

    /**
     * Signals the event (SetEvent).
     * @throws rav::Exception if SetEvent fails.
     */
    void signal() {
        if (!SetEvent(event_)) {
            RAV_THROW_EXCEPTION("Failed to signal event");
        }
    }

  private:
    WSAEVENT event_ {};
};

}  // namespace rav::windows

#endif
