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

#include "ravennakit/core/util.hpp"
#include "ravennakit/core/streams/input_stream_view.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::InputStreamView") {
    SECTION("raw data") {
        constexpr uint8_t data[] = {0x11, 0x22, 0x33, 0x44};
        rav::InputStreamView stream(data, rav::num_elements_in_array(data));
        REQUIRE(stream.read_be<uint32_t>() == 0x11223344);
    }

    SECTION("vector") {
        const std::vector<uint8_t> data = {0x11, 0x22, 0x33, 0x44};
        rav::InputStreamView stream(data);
        REQUIRE(stream.read_be<uint32_t>() == 0x11223344);
    }

    SECTION("array") {
        constexpr std::array<uint8_t, 4> data = {0x11, 0x22, 0x33, 0x44};
        rav::InputStreamView stream(data);
        REQUIRE(stream.read_be<uint32_t>() == 0x11223344);
    }

    SECTION("Other functions") {
        const std::vector<uint8_t> data = {0x11, 0x22, 0x33, 0x44};
        rav::InputStreamView stream(data);
        REQUIRE(stream.size().has_value());
        REQUIRE(stream.size().value() == 4);
        REQUIRE_FALSE(stream.exhausted());
        REQUIRE(stream.get_read_position() == 0);
        REQUIRE(stream.read_be<uint32_t>() == 0x11223344);
        REQUIRE(stream.get_read_position() == 4);
        REQUIRE(stream.exhausted());
        stream.reset();
        REQUIRE(stream.get_read_position() == 0);
        REQUIRE_FALSE(stream.exhausted());
        REQUIRE(stream.read_be<uint32_t>() == 0x11223344);
        REQUIRE(stream.exhausted());
        REQUIRE(stream.set_read_position(1));
        REQUIRE_FALSE(stream.read_be<uint32_t>().has_value());
    }
}
