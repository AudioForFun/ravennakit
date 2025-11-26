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

#include <catch2/catch_all.hpp>

#include <ravennakit/core/containers/buffer_view.hpp>
#include <ravennakit/core/util.hpp>

static_assert(std::is_trivially_copyable_v<rav::BufferView<double>> == true);
static_assert(std::is_trivially_copyable_v<rav::BufferView<float>> == true);
static_assert(std::is_trivially_copyable_v<rav::BufferView<int>> == true);
static_assert(std::is_trivially_copyable_v<rav::BufferView<char>> == true);

TEST_CASE("rav::BufferView") {
    SECTION("Test int buffer") {
        int data[] = {1, 2, 3, 4, 5};
        const rav::BufferView buffer_view(data, rav::num_elements_in_array(data));

        REQUIRE(buffer_view.size() == 5);
        REQUIRE(buffer_view.size_bytes() == 5 * sizeof(int));
        REQUIRE(buffer_view.data() == data);
        REQUIRE(buffer_view.empty() == false);
    }

    SECTION("Test char buffer") {
        char data[] = {1, 2, 3, 4, 5};
        const rav::BufferView buffer_view(data, rav::num_elements_in_array(data));

        REQUIRE(buffer_view.size() == 5);
        REQUIRE(buffer_view.size_bytes() == 5 * sizeof(char));
        REQUIRE(buffer_view.data() == data);
        REQUIRE(buffer_view.empty() == false);
    }

    SECTION("Test empty buffer") {
        int data = 5;
        const rav::BufferView buffer_view(&data, 0);

        REQUIRE(buffer_view.size() == 0);  // NOLINT
        REQUIRE(buffer_view.size_bytes() == 0);
        REQUIRE(buffer_view.data() == &data);
        REQUIRE(buffer_view.empty() == true);
    }

    SECTION("Test invalid buffer") {
        const rav::BufferView<int> buffer_view(nullptr, 1);

        REQUIRE(buffer_view.size() == 0);  // NOLINT
        REQUIRE(buffer_view.size_bytes() == 0);
        REQUIRE(buffer_view.data() == nullptr);
        REQUIRE(buffer_view.empty() == true);
    }

    SECTION("buffer_view can be copied") {
        int data[] = {1, 2, 3, 4, 5};
        const rav::BufferView buffer_view(data, rav::num_elements_in_array(data));
        const rav::BufferView buffer_view_copy(buffer_view);

        REQUIRE(buffer_view.data() == buffer_view_copy.data());
        REQUIRE(buffer_view.size() == buffer_view_copy.size());
        REQUIRE(buffer_view.size_bytes() == buffer_view_copy.size_bytes());
        REQUIRE(buffer_view.empty() == buffer_view_copy.empty());
    }

    SECTION("reinterpret()") {
        std::array<int16_t, 4> data {};
        const rav::BufferView buffer_view(data.data(), data.size());

        const auto reinterpreted = buffer_view.reinterpret<int32_t>();
        REQUIRE(reinterpreted.data() == reinterpret_cast<const int32_t*>(data.data()));
        REQUIRE(reinterpreted.size() == 2);
        REQUIRE(reinterpreted.size_bytes() == 2 * sizeof(int32_t));
    }
}
