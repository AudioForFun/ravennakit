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

#if RAV_POSIX

    #include <unistd.h>

namespace rav::posix {

/**
 * Wrapper around the POSIX pipe() function.
 */
class Pipe {
  public:
    /**
     * Constructs a pipe.
     * @throws rav::Exception if pipe() fails.
     */
    Pipe() {
        if (::pipe(fds_) == -1) {
            RAV_THROW_EXCEPTION("pipe() failed");
        }
    }

    ~Pipe() {
        if (fds_[0] != 0) {
            ::close(fds_[0]);
        }
        if (fds_[1] != 0) {
            ::close(fds_[1]);
        }
    }

    /**
     * Writes data to the pipe.
     * @param data The data to write.
     * @param size The size of the data.
     * @throws rav::Exception if read() fails.
     * @return The number of bytes written.
     */
    ssize_t write(const void* data, const size_t size) const {
        return ::write(fds_[1], data, size);
    }

    /**
     * Reads data from the pipe.
     * @param data The data to read into.
     * @param size The size of the data.
     * @throws rav::Exception if read() fails.
     * @return The number of bytes read.
     */
    ssize_t read(void* data, const size_t size) const {
        return ::read(fds_[0], data, size);
    }

    /**
     * @returns The read file descriptors.
     */
    [[nodiscard]] int read_fd() const {
        return fds_[0];
    }

    /**
     * @returns The write file descriptors.
     */
    [[nodiscard]] int write_fd() const {
        return fds_[1];
    }

  private:
    int fds_[2] = {0, 0};  // read and write file descriptors
};

}  // namespace rav::posix

#endif
