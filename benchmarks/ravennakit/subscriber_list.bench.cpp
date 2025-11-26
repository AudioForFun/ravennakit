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

#include "ravennakit/core/util/subscriber_list.hpp"

#include <catch2/catch_all.hpp>
#include <nanobench.h>

namespace {

class Subscriber {
  public:
    virtual ~Subscriber() = default;
    virtual void on_event(const std::string& event) {
        ankerl::nanobench::doNotOptimizeAway(event);
    }
};

class ConcreteSubscriber: public Subscriber {
  public:
    void on_event(const std::string& event) override {
        ankerl::nanobench::doNotOptimizeAway(event);
    }
};

}  // namespace

TEST_CASE("SubscriberList Benchmark") {
    ankerl::nanobench::Bench b;
    b.title("SubscriberList Benchmark")
        .warmup(100)
        .relative(false)
        .minEpochIterations(1'000'000)
        .performanceCounters(true);

    rav::SubscriberList<Subscriber> subscriber_list;
    ConcreteSubscriber subscriber;
    std::ignore = subscriber_list.add(&subscriber);

    int i_int = 0;
    b.run("Using foreach", [&] {
        subscriber_list.foreach([&](Subscriber* s) {
            s->on_event(std::to_string(i_int++));
        });
    });

    b.run("Using range based for", [&] {
        for (auto* s: subscriber_list) {
            s->on_event(std::to_string(i_int++));
        }
    });
}
