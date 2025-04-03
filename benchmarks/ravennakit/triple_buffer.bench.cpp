/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/core/sync/triple_buffer.hpp"

#include <catch2/catch_all.hpp>
#include <nanobench.h>

#include <thread>

TEST_CASE("TripleBuffer Benchmark") {
    ankerl::nanobench::Bench b;
    b.title("TripleBuffer Benchmark").warmup(100).relative(false).minEpochIterations(2154287).performanceCounters(true);

    {
        rav::TripleBuffer<uint64_t> buffer;

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
        rav::TripleBuffer<uint64_t> buffer;

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
