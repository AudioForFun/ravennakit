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

namespace {

class test_subscriber final {
  public:
    void notify(const std::string& message) {
        messages.push_back(message);
    }

    std::vector<std::string> messages;
};

}  // namespace

TEST_CASE("rav::SubscriberList") {
    rav::SubscriberList<test_subscriber> list;

    SECTION("Add, notify and remove") {
        test_subscriber subscriber1;
        test_subscriber subscriber2;
        REQUIRE(list.add(&subscriber1));
        REQUIRE(list.add(&subscriber2));

        list.foreach ([](auto* subscriber) {
            subscriber->notify("Hello");
        });

        REQUIRE(subscriber1.messages.size() == 1);
        REQUIRE(subscriber1.messages[0] == "Hello");

        REQUIRE(subscriber2.messages.size() == 1);
        REQUIRE(subscriber2.messages[0] == "Hello");

        REQUIRE(list.remove(&subscriber1));

        list.foreach ([](auto* subscriber) {
            subscriber->notify("World");
        });

        REQUIRE(subscriber1.messages.size() == 1);
        REQUIRE(subscriber1.messages[0] == "Hello");

        REQUIRE(subscriber2.messages.size() == 2);
        REQUIRE(subscriber2.messages[0] == "Hello");
        REQUIRE(subscriber2.messages[1] == "World");

        REQUIRE(list.remove(&subscriber2));
    }

    SECTION("Notify using iterators") {
        test_subscriber subscriber1;
        REQUIRE(list.add(&subscriber1));

        for (auto* sub : list) {
            sub->notify("Hello");
        }

        REQUIRE(subscriber1.messages.size() == 1);
        REQUIRE(subscriber1.messages[0] == "Hello");

        REQUIRE(list.remove(&subscriber1));
    }

    SECTION("Double subscribe") {
        test_subscriber subscriber1;
        REQUIRE(list.add(&subscriber1));
        REQUIRE(list.size() == 1);
        REQUIRE_FALSE(list.add(&subscriber1));
        REQUIRE(list.size() == 1);

        for (auto* sub : list) {
            sub->notify("Hello");
        }

        REQUIRE(subscriber1.messages.size() == 1);
        REQUIRE(subscriber1.messages[0] == "Hello");

        REQUIRE(list.remove(&subscriber1));
    }

    SECTION("Move construct") {
        test_subscriber subscriber1;
        test_subscriber subscriber2;
        REQUIRE(list.add(&subscriber1));
        REQUIRE(list.add(&subscriber2));

        rav::SubscriberList list2(std::move(list));

        REQUIRE(list.empty());
        REQUIRE(list2.size() == 2);

        REQUIRE(list2.remove(&subscriber1));
        REQUIRE(list2.remove(&subscriber2));
    }

    SECTION("Move assign") {
        test_subscriber subscriber1;
        test_subscriber subscriber2;
        REQUIRE(list.add(&subscriber1));
        REQUIRE(list.add(&subscriber2));

        rav::SubscriberList<test_subscriber> list2;
        test_subscriber subscriber3;
        REQUIRE(list2.add(&subscriber3));

        list2 = std::move(list);

        REQUIRE(list.empty());
        REQUIRE(list2.size() == 2);

        std::vector<test_subscriber*> list2_subscribers;
        for (auto* sub : list2) {
            list2_subscribers.push_back(sub);
        }

        REQUIRE(list2_subscribers.at(0) == &subscriber1);
        REQUIRE(list2_subscribers.at(1) == &subscriber2);

        REQUIRE(list2.remove(&subscriber1));
        REQUIRE(list2.remove(&subscriber2));
    }

    SECTION("subscriber_list with context") {
        rav::SubscriberList<test_subscriber, std::string> list_with_context;

        SECTION("Add, notify and remove") {
            test_subscriber subscriber1;
            test_subscriber subscriber2;
            REQUIRE(list_with_context.add(&subscriber1, "subscriber1"));
            REQUIRE(list_with_context.add(&subscriber2, "subscriber2"));

            list_with_context.foreach ([](auto* subscriber, const std::string& ctx) {
                subscriber->notify(ctx);
            });

            REQUIRE(subscriber1.messages.size() == 1);
            REQUIRE(subscriber1.messages[0] == "subscriber1");

            REQUIRE(subscriber2.messages.size() == 1);
            REQUIRE(subscriber2.messages[0] == "subscriber2");

            REQUIRE(list_with_context.remove(&subscriber1));

            list_with_context.foreach ([](auto* subscriber, const std::string& ctx) {
                subscriber->notify(ctx);
            });

            REQUIRE(subscriber1.messages.size() == 1);
            REQUIRE(subscriber1.messages[0] == "subscriber1");

            REQUIRE(subscriber2.messages.size() == 2);
            REQUIRE(subscriber2.messages[0] == "subscriber2");
            REQUIRE(subscriber2.messages[1] == "subscriber2");

            REQUIRE(list_with_context.remove(&subscriber2));
        }

        SECTION("Notify using iterators") {
            test_subscriber subscriber1;
            REQUIRE(list_with_context.add(&subscriber1, "subscriber1"));

            for (auto& [sub, ctx] : list_with_context) {
                sub->notify(ctx);
            }

            REQUIRE(subscriber1.messages.size() == 1);
            REQUIRE(subscriber1.messages[0] == "subscriber1");

            REQUIRE(list_with_context.remove(&subscriber1));
        }

        SECTION("Double subscribe") {
            test_subscriber subscriber1;
            REQUIRE(list_with_context.add(&subscriber1, "subscriber1-1"));
            REQUIRE(list_with_context.size() == 1);
            REQUIRE_FALSE(list_with_context.add(&subscriber1, "subscriber1-2"));
            REQUIRE(list_with_context.size() == 1);

            for (auto& [sub, ctx] : list_with_context) {
                sub->notify(ctx);
            }

            REQUIRE(subscriber1.messages.size() == 1);
            REQUIRE(subscriber1.messages[0] == "subscriber1-1");

            REQUIRE(list_with_context.remove(&subscriber1));
        }

        SECTION("Move construct") {
            test_subscriber subscriber1;
            test_subscriber subscriber2;
            REQUIRE(list_with_context.add(&subscriber1, "subscriber1"));
            REQUIRE(list_with_context.add(&subscriber2, "subscriber2"));

            rav::SubscriberList list2(std::move(list_with_context));

            REQUIRE(list_with_context.empty());
            REQUIRE(list2.size() == 2);

            REQUIRE(list2.remove(&subscriber1));
            REQUIRE(list2.remove(&subscriber2));
        }

        SECTION("Move assign") {
            test_subscriber subscriber1;
            test_subscriber subscriber2;
            REQUIRE(list_with_context.add(&subscriber1, "subscriber1"));
            REQUIRE(list_with_context.add(&subscriber2, "subscriber2"));

            rav::SubscriberList<test_subscriber, std::string> list2;
            test_subscriber subscriber3;
            REQUIRE(list2.add(&subscriber3, "subscriber3"));

            list2 = std::move(list_with_context);

            REQUIRE(list_with_context.empty());
            REQUIRE(list2.size() == 2);

            std::vector<std::pair<test_subscriber*, std::string>> list2_subscribers;
            for (auto& [sub, ctx] : list2) {
                list2_subscribers.emplace_back(sub, ctx);
            }

            REQUIRE(list2_subscribers.at(0).first == &subscriber1);
            REQUIRE(list2_subscribers.at(0).second == "subscriber1");
            REQUIRE(list2_subscribers.at(1).first == &subscriber2);
            REQUIRE(list2_subscribers.at(1).second == "subscriber2");

            REQUIRE(list2.remove(&subscriber1));
            REQUIRE(list2.remove(&subscriber2));
        }
    }
}
