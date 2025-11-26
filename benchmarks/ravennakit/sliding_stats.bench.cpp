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

#include "ravennakit/core/math/sliding_stats.hpp"

#include <catch2/catch_all.hpp>
#include <nanobench.h>

TEST_CASE("SlidingStats Benchmark") {
    ankerl::nanobench::Bench b;
    b.title("SlidingStats Benchmark")
        .warmup(100)
        .relative(false)
        .minEpochIterations(20800)
        .performanceCounters(true);

    rav::SlidingStats stats(1024);
    double i = 0.0;
    b.run("Add", [&] {
        stats.add(i);
        i += 1.0;
    });
}
