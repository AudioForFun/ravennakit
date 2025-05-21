/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include <catch2/catch_all.hpp>
#include <nanobench.h>
#include <boost/signals2.hpp>

TEST_CASE("Boost::signals2 Benchmark") {
    ankerl::nanobench::Bench b;
    b.title("Boost::signals2 Benchmark").warmup(100).relative(false).minEpochIterations(100'000).performanceCounters(true);

    {
        boost::signals2::signal<void(const std::string&)> signal;

        signal.connect([](const std::string& s) {
            ankerl::nanobench::doNotOptimizeAway(s);
        });

        signal.connect([](const std::string& s) {
            ankerl::nanobench::doNotOptimizeAway(s);
        });

        int i = 0;
        b.run("String", [&] {
            signal(std::to_string(i++));
        });
    }

    {
        boost::signals2::signal<void(int)> signal;

        signal.connect([](int s) {
            ankerl::nanobench::doNotOptimizeAway(s);
        });

        signal.connect([](int s) {
            ankerl::nanobench::doNotOptimizeAway(s);
        });

        int i = 0;
        b.run("Int", [&] {
            signal(i++);
        });
    }
}
