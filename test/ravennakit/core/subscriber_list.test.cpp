/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/subscriber_list.hpp"

#include <catch2/catch_all.hpp>

namespace {

class test_subscriber final {
  public:
    void notify(const std::string& message) {
        messages.push_back(message);
    }

    std::vector<std::string> messages;
};

}  // namespace

TEST_CASE("subscriber_list") {
    rav::subscriber_list<test_subscriber> list;

    SECTION("Add, notify and remove") {
        test_subscriber subscriber1;
        test_subscriber subscriber2;
        list.add(&subscriber1);
        list.add(&subscriber2);

        list.foreach ([](auto* subscriber) {
            subscriber->notify("Hello");
        });

        REQUIRE(subscriber1.messages.size() == 1);
        REQUIRE(subscriber1.messages[0] == "Hello");

        REQUIRE(subscriber2.messages.size() == 1);
        REQUIRE(subscriber2.messages[0] == "Hello");

        list.remove(&subscriber1);

        list.foreach ([](auto* subscriber) {
            subscriber->notify("World");
        });

        REQUIRE(subscriber1.messages.size() == 1);
        REQUIRE(subscriber1.messages[0] == "Hello");

        REQUIRE(subscriber2.messages.size() == 2);
        REQUIRE(subscriber2.messages[0] == "Hello");
        REQUIRE(subscriber2.messages[1] == "World");
    }

    SECTION("Notify using iterators") {
        test_subscriber subscriber1;
        list.add(&subscriber1);

        for (auto* sub : list) {
            sub->notify("Hello");
        }

        REQUIRE(subscriber1.messages.size() == 1);
        REQUIRE(subscriber1.messages[0] == "Hello");
    }

    SECTION("Double subscribe") {
        test_subscriber subscriber1;
        list.add(&subscriber1);
        REQUIRE(list.size() == 1);
        list.add(&subscriber1);
        REQUIRE(list.size() == 1);

        for (auto* sub : list) {
            sub->notify("Hello");
        }

        REQUIRE(subscriber1.messages.size() == 1);
        REQUIRE(subscriber1.messages[0] == "Hello");
    }

    SECTION("Move construct") {
        test_subscriber subscriber1;
        test_subscriber subscriber2;
        list.add(&subscriber1);
        list.add(&subscriber2);

        rav::subscriber_list list2(std::move(list));

        REQUIRE(list.empty());
        REQUIRE(list2.size() == 2);
    }

    SECTION("Move assign") {
        test_subscriber subscriber1;
        test_subscriber subscriber2;
        list.add(&subscriber1);
        list.add(&subscriber2);

        rav::subscriber_list<test_subscriber> list2;
        test_subscriber subscriber3;
        list2.add(&subscriber3);

        list2 = std::move(list);

        REQUIRE(list.empty());
        REQUIRE(list2.size() == 2);

        std::vector<test_subscriber*> list2_subscribers;
        for (auto* sub : list2) {
            list2_subscribers.push_back(sub);
        }

        REQUIRE(list2_subscribers.at(0) == &subscriber1);
        REQUIRE(list2_subscribers.at(1) == &subscriber2);
    }
}
