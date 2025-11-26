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

#include "ravennakit/core/platform/posix/pipe.hpp"
#include "catch2/catch_all.hpp"

#if RAV_POSIX

TEST_CASE("rav::posix::Pipe") {
    SECTION("Test default state") {
        rav::posix::Pipe pipe;
        REQUIRE(pipe.read_fd() >= 3);
        REQUIRE(pipe.read_fd() >= 3);
    }

    SECTION("Read and write something") {
        uint64_t in = 0x1234567890abcdef;
        uint64_t out = 0;
        rav::posix::Pipe pipe;
        REQUIRE(pipe.write(&in, sizeof(in)) == sizeof(in));
        REQUIRE(pipe.read(&out, sizeof(out)) == sizeof(out));
        REQUIRE(in == out);
    }

    SECTION("Read a bunch of something") {
        constexpr uint64_t num_elements = 1000; // If this is too large, the test will hang
        rav::posix::Pipe pipe;

        // Write a bunch of data
        for (uint64_t i = 0; i < num_elements; ++i) {
            uint64_t in = i + 0xffff;
            REQUIRE(pipe.write(&in, sizeof(in)) == sizeof(in));
        }

        // Read a bunch of data
        for (uint64_t i = 0; i < num_elements; ++i) {
            uint64_t in = 0;
            REQUIRE(pipe.read(&in, sizeof(in)) == sizeof(in));
            REQUIRE(in == i + 0xffff);
        }
    }
}

#endif
