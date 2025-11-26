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

#include "ravennakit/core/types/int24.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::int24_t") {
    SECTION("int32 to int24") {
        const rav::int24_t min(-8388608);
        REQUIRE(static_cast<int32_t>(min) == -8388608);

        const rav::int24_t max(8388607);
        REQUIRE(static_cast<int32_t>(max) == 8388607);

        const rav::int24_t zero(0);
        REQUIRE(static_cast<int32_t>(zero) == 0);

        const rav::int24_t twenty_four(24);
        REQUIRE(static_cast<int32_t>(twenty_four) == 24);

        const rav::int24_t in32max(std::numeric_limits<int32_t>::max());
        REQUIRE(static_cast<int32_t>(in32max) == 8388607);  // Value is truncated

        const rav::int24_t in32min(std::numeric_limits<int32_t>::min());
        REQUIRE(static_cast<int32_t>(in32min) == -8388608);  // Value is truncated
    }
}
