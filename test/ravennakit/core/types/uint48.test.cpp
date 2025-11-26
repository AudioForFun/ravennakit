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

#include "ravennakit/core/types/uint48.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::uint48_t") {
    SECTION("uint64 to uint48") {
        const rav::uint48_t min(0);
        REQUIRE(static_cast<uint64_t>(min) == 0);

        const rav::uint48_t max(0xffffffffffff);
        REQUIRE(static_cast<uint64_t>(max) == 0xffffffffffff);

        const rav::uint48_t in64max(std::numeric_limits<uint64_t>::max());
        REQUIRE(static_cast<uint64_t>(in64max) == 0xffffffffffff);  // Value is truncated

        const rav::uint48_t forty_eight(48);
        REQUIRE(static_cast<uint64_t>(forty_eight) == 48);
    }
}
