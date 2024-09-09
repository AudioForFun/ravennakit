/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include <catch2/catch_all.hpp>
#include <ravennakit/containers/circular_buffer.hpp>
#include <ravennakit/containers/fifo.hpp>
#include <thread>

TEST_CASE("circular_buffer<int, rav::fifo::single>") {
    rav::circular_buffer<int, rav::fifo::single> buffer(10);

    const std::array<int, 8> src = {1, 2, 3, 4, 5, 6, 7, 8};
    REQUIRE(buffer.write(src.data(), src.size()));
    REQUIRE(buffer.write(src.data(), 2));
    REQUIRE_FALSE(buffer.write(&src[0], 1));

    std::array<int, 10> dst = {};

    REQUIRE(buffer.read(dst.data(), dst.size()));
    REQUIRE(dst == std::array {1, 2, 3, 4, 5, 6, 7, 8, 1, 2});
    REQUIRE_FALSE(buffer.read(dst.data(), 1));

    dst = {};

    REQUIRE(buffer.write(src.data(), 4));
    REQUIRE(buffer.read(dst.data(), 2));
    REQUIRE(buffer.read(dst.data() + 2, 2));

    REQUIRE(dst == std::array {1, 2, 3, 4, 0, 0, 0, 0, 0, 0});
}

TEST_CASE("circular_buffer<int, rav::sync_strategy::spsc>") {
    rav::circular_buffer<int, rav::fifo::spsc> buffer(10);

    SECTION("Test basic reading writing") {
        const std::array<int, 8> src = {1, 2, 3, 4, 5, 6, 7, 8};
        REQUIRE(buffer.write(src.data(), src.size()));
        REQUIRE(buffer.write(src.data(), 2));
        REQUIRE_FALSE(buffer.write(&src[0], 1));

        std::array<int, 10> dst = {};

        REQUIRE(buffer.read(dst.data(), dst.size()));
        REQUIRE(dst == std::array {1, 2, 3, 4, 5, 6, 7, 8, 1, 2});
        REQUIRE_FALSE(buffer.read(dst.data(), 1));

        dst = {};

        REQUIRE(buffer.write(src.data(), 4));
        REQUIRE(buffer.read(dst.data(), 2));
        REQUIRE(buffer.read(dst.data() + 2, 2));

        REQUIRE(dst == std::array {1, 2, 3, 4, 0, 0, 0, 0, 0, 0});
    }

    SECTION("Reading and writing should be without data races") {
        constexpr int num_elements = 1'000'000;
        int expected_total = 0;
        int total = 0;

        std::thread writer([&] {
            constexpr std::array<int, 3> src = {1,2,3};
            for (int i = 0; i < num_elements; ++i) {
                while (!buffer.write(src.data(), src.size())) {}

                for (const auto n : src) {
                    expected_total += n;
                }
            }
        });

        std::thread reader([&] {
            std::array<int, 3> dst = {};

            for (int i = 0; i < num_elements; ++i) {
                while (!buffer.read(dst.data(), dst.size())) {}

                for (const auto n : dst) {
                    total += n;
                }
            }
        });

        writer.join();
        reader.join();

        REQUIRE(total == expected_total);
    }
}

// TEST_CASE("circular_buffer<int, rav::sync_strategy::mpsc>") {
//     rav::circular_buffer<int, rav::sync_strategy::mpsc> buffer(10);
//
//     for (int i = 0; i < 10; ++i) {
//         REQUIRE(buffer.push(&i, 1));
//     }
// }
//
// TEST_CASE("circular_buffer<int, rav::sync_strategy::spmc>") {
//     rav::circular_buffer<int, rav::sync_strategy::spmc> buffer(10);
//
//     for (int i = 0; i < 10; ++i) {
//         REQUIRE(buffer.push(&i, 1));
//     }
// }
//
// TEST_CASE("circular_buffer<int, rav::sync_strategy::mpmc>") {
//     rav::circular_buffer<int, rav::sync_strategy::mpmc> buffer(10);
//
//     for (int i = 0; i < 10; ++i) {
//         REQUIRE(buffer.push(&i, 1));
//     }
// }
