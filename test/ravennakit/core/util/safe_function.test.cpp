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

#include "ravennakit/core/util/safe_function.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("rav::SafeFunction") {
    rav::SafeFunction<void(const std::string& a, const std::string& b)> callback_function;
    callback_function("a", "b");
    int times_called = 0;
    callback_function.set([&](const std::string& a, const std::string& b) {
        REQUIRE(a == "a");
        REQUIRE(b == "b");
        times_called++;
    });
    callback_function("a", "b");
    REQUIRE(times_called == 1);
    callback_function.set(nullptr);
    callback_function("a", "b");
    REQUIRE(times_called == 1);
    callback_function = [&](const std::string& a, const std::string& b) {
        REQUIRE(a == "c");
        REQUIRE(b == "d");
        times_called++;
    };
    callback_function("c", "d");
    REQUIRE(times_called == 2);
}
