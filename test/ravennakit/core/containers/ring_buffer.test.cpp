/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/containers/ring_buffer.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("ring_buffer") {
    SECTION("Basic operations") {
        rav::RingBuffer<uint8_t> buffer(3);

        REQUIRE(buffer.empty() == true);
        REQUIRE(buffer.full() == false);
        REQUIRE(buffer.size() == 0);  // NOLINT
        REQUIRE(buffer.capacity() == 3);

        buffer.push_back(1);

        REQUIRE(buffer.empty() == false);
        REQUIRE(buffer.full() == false);
        REQUIRE(buffer.size() == 1);
        REQUIRE(buffer.capacity() == 3);

        buffer.push_back(2);
        buffer.push_back(3);

        REQUIRE(buffer.empty() == false);
        REQUIRE(buffer.full() == true);
        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer.capacity() == 3);

        REQUIRE(buffer[0] == 1);
        REQUIRE(buffer[1] == 2);
        REQUIRE(buffer[2] == 3);
        REQUIRE(buffer[3] == 1);

        REQUIRE(buffer.pop_front().value() == 1);
        REQUIRE(buffer.pop_front().value() == 2);
        REQUIRE(buffer.pop_front().value() == 3);
        REQUIRE_FALSE(buffer.pop_front().has_value());

        REQUIRE(buffer.empty() == true);
        REQUIRE(buffer.full() == false);
        REQUIRE(buffer.size() == 0);  // NOLINT
        REQUIRE(buffer.capacity() == 3);

        buffer.push_back(1);
        buffer.push_back(2);
        buffer.push_back(3);
        buffer.push_back(4);

        REQUIRE(buffer.empty() == false);
        REQUIRE(buffer.full() == true);
        REQUIRE(buffer.size() == 3);  // NOLINT
        REQUIRE(buffer.capacity() == 3);

        REQUIRE(buffer[0] == 2);
        REQUIRE(buffer[1] == 3);
        REQUIRE(buffer[2] == 4);
        REQUIRE(buffer[3] == 2);

        REQUIRE(buffer.pop_front().value() == 2);
        REQUIRE(buffer.pop_front().value() == 3);
        REQUIRE(buffer.pop_front().value() == 4);
        REQUIRE_FALSE(buffer.pop_front().has_value());

        // In case of a POD type T, values are not initialized:
        REQUIRE(buffer[0] == 2);
        REQUIRE(buffer[1] == 3);
        REQUIRE(buffer[2] == 4);
        REQUIRE(buffer[3] == 2);
    }

    SECTION("Front and back") {
        rav::RingBuffer<uint8_t> buffer({1, 2, 3});
        REQUIRE(buffer.front() == 1);
        REQUIRE(buffer.back() == 3);
    }

    SECTION("Copy construct") {
        rav::RingBuffer<uint8_t> buffer({1, 2, 3});
        auto buffer2(buffer);  // NOLINT
        REQUIRE(buffer2.size() == 3);
        REQUIRE(buffer2[0] == 1);
        REQUIRE(buffer2[1] == 2);
        REQUIRE(buffer2[2] == 3);
    }

    SECTION("Copy assign") {
        rav::RingBuffer<uint8_t> buffer({1, 2, 3});
        auto buffer2 = buffer;  // NOLINT
        REQUIRE(buffer2.size() == 3);
        REQUIRE(buffer2[0] == 1);
        REQUIRE(buffer2[1] == 2);
        REQUIRE(buffer2[2] == 3);
    }

    SECTION("Move construct") {
        rav::RingBuffer<uint8_t> buffer({1, 2, 3});
        auto buffer2(std::move(buffer));
        REQUIRE(buffer2.size() == 3);
        REQUIRE(buffer2[0] == 1);
        REQUIRE(buffer2[1] == 2);
        REQUIRE(buffer2[2] == 3);
    }

    SECTION("Move assign") {
        rav::RingBuffer<uint8_t> buffer({1, 2, 3});
        rav::RingBuffer<uint8_t> buffer2({4, 5, 6});
        buffer2 = std::move(buffer);  // NOLINT
        REQUIRE(buffer2.size() == 3);
        REQUIRE(buffer2[0] == 1);
        REQUIRE(buffer2[1] == 2);
        REQUIRE(buffer2[2] == 3);
        REQUIRE(buffer.empty());
    }

    SECTION("Iterator") {
        rav::RingBuffer<uint8_t> buffer(3);
        std::vector<uint8_t> values;

        SECTION("0") {
            for (auto& value : buffer) {
                values.push_back(value);
            }
            REQUIRE(values.empty());
        }

        SECTION("1") {
            buffer.push_back(1);
            for (auto& value : buffer) {
                values.push_back(value);
            }
            REQUIRE(values.size() == 1);
            REQUIRE(values == std::vector<uint8_t> {1});
        }

        SECTION("2") {
            buffer.push_back(1);
            buffer.push_back(2);
            for (auto& value : buffer) {
                values.push_back(value);
            }
            REQUIRE(values.size() == 2);
            REQUIRE(values == std::vector<uint8_t> {1, 2});
        }

        SECTION("3") {
            buffer.push_back(1);
            buffer.push_back(2);
            buffer.push_back(3);
            for (auto& value : buffer) {
                values.push_back(value);
            }
            REQUIRE(values.size() == 3);
            REQUIRE(values == std::vector<uint8_t> {1, 2, 3});
        }

        SECTION("3") {
            buffer.push_back(1);
            buffer.push_back(2);
            buffer.push_back(3);
            buffer.push_back(4);
            for (auto& value : buffer) {
                values.push_back(value);
            }
            REQUIRE(values.size() == 3);
            REQUIRE(values == std::vector<uint8_t> {2, 3, 4});
        }

        SECTION("Modify values") {
            buffer.push_back(1);
            buffer.push_back(2);
            buffer.push_back(3);
            for (auto& value : buffer) {
                value = 0;
            }
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 0);
            REQUIRE(buffer[2] == 0);
        }
    }

    SECTION("Const Iterator") {
        const rav::RingBuffer<uint8_t> buffer({1, 2, 3});
        std::vector<uint8_t> values;

        for (auto& value : buffer) {
            values.push_back(value);
        }

        REQUIRE(values.size() == 3);
        REQUIRE(values == std::vector<uint8_t> {1, 2, 3});
    }
}
