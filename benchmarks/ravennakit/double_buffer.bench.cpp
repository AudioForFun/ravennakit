/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/core/sync/double_buffer.hpp"

#include <nanobench.h>
#include <catch2/catch_all.hpp>

#include <thread>

TEST_CASE("DoubleBuffer Benchmark") {
    ankerl::nanobench::Bench b;
    b.title("DoubleBuffer Benchmark").warmup(100).relative(false).minEpochIterations(23704458).performanceCounters(true);

    {
        rav::DoubleBuffer<uint64_t> buffer;

        std::atomic_bool keep_going {true};

        std::thread consumer([&] {
            while (keep_going) {
                buffer.get();
            }
        });

        std::thread producer([&] {
            uint64_t i = 0;

            b.run("Producer", [&] {
                buffer.update(i++);
            });

            keep_going = false;
        });

        producer.join();
        consumer.join();
    }

    {
        rav::DoubleBuffer<uint64_t> buffer;

        std::atomic_bool keep_going {true};

        std::thread producer([&] {
            uint64_t i = 0;
            while (keep_going) {
                buffer.update(i++);
            }
        });

        std::thread consumer([&] {
            b.run("Consumer", [&] {
                ankerl::nanobench::doNotOptimizeAway(buffer.get());
            });
            keep_going = false;
        });

        producer.join();
        consumer.join();
    }
}
