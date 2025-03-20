/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/containers/vector_buffer.hpp"
#include "ravennakit/core/events.hpp"

#include <catch2/catch_all.hpp>

namespace {
struct str_event {
    std::string string;
};

struct int_event {
    int number;
};
}

TEST_CASE("events", "[events]") {
    rav::VectorBuffer<std::string> fired_events;

    rav::Events<str_event, int_event> events;
    events.on<str_event>([&fired_events](const str_event& event) {
        fired_events.push_back(event.string);
    });
    events.on<int_event>([&fired_events](const int_event& event) {
        fired_events.push_back(std::to_string(event.number));
    });

    events.emit(str_event {"Hello"});

    REQUIRE(fired_events.read() == "Hello");
    REQUIRE(fired_events.empty());

    events.emit(int_event {42});

    REQUIRE(fired_events.read() == "42");
    REQUIRE(fired_events.empty());

    events.reset<str_event>();

    events.emit(str_event {"Hello"});
    events.emit(int_event {42});

    REQUIRE(fired_events.read() == "42");
    REQUIRE(fired_events.empty());

    events.reset();

    events.emit(str_event {"Hello"});
    events.emit(int_event {42});

    REQUIRE(fired_events.empty());
}
