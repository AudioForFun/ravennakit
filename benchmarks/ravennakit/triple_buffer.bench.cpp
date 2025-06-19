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
#include <boost/lockfree/spsc_value.hpp>

TEST_CASE("TripleBuffer Benchmark") {
    ankerl::nanobench::Bench b;
    b.title("TripleBuffer Benchmark")
        .warmup(100)
        .relative(false)
        .minEpochIterations(1'000'000)
        .performanceCounters(true);

    {
        rav::TripleBuffer<uint64_t> buffer;

        std::atomic_bool keep_going {true};

        std::thread consumer([&] {
            while (keep_going) {
                buffer.get();
            }
        });

        uint64_t i = 0;

        b.run("Producer", [&] {
            buffer.update(i++);
        });

        keep_going = false;

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

        b.run("Consumer", [&] {
            ankerl::nanobench::doNotOptimizeAway(buffer.get());
        });
        keep_going = false;

        producer.join();
    }
}

TEST_CASE("spsc_value benchmark") {
    ankerl::nanobench::Bench b;
    b.title("spsc_value Benchmark")
        .warmup(100)
        .relative(false)
        .minEpochIterations(1'000'000)
        .performanceCounters(true);

    {
        boost::lockfree::spsc_value<uint64_t> spsc_value;

        std::atomic_bool keep_going {true};

        std::thread consumer([&] {
            while (keep_going) {
                spsc_value.read(boost::lockfree::uses_optional);
            }
        });

        uint64_t i = 0;

        b.run("Producer", [&] {
            spsc_value.write(i);
        });

        keep_going = false;

        consumer.join();
    }

    {
        boost::lockfree::spsc_value<uint64_t> spsc_value;

        std::atomic_bool keep_going {true};

        std::thread producer([&] {
            uint64_t i = 0;
            while (keep_going) {
                spsc_value.write(i++);
            }
        });

        b.run("Consumer", [&] {
            ankerl::nanobench::doNotOptimizeAway(spsc_value.read(boost::lockfree::uses_optional));
        });
        keep_going = false;

        producer.join();
    }
}

