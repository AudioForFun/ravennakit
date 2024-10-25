/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/linked_node.hpp"

#include <iostream>
#include <catch2/catch_all.hpp>

TEST_CASE("linked_node | Build a list", "[linked_node]") {
    rav::linked_node n1(1);
    rav::linked_node n2(2);
    rav::linked_node n3(3);

    std::vector<int> nodes;

    SECTION("Single node") {
        REQUIRE(n1.data() == 1);
        REQUIRE(n1.is_front() == false);
        REQUIRE(n1.is_back() == false);
        REQUIRE(n1.is_linked() == false);

        REQUIRE(n2.data() == 2);
        REQUIRE(n2.is_front() == false);
        REQUIRE(n2.is_back() == false);
        REQUIRE(n2.is_linked() == false);

        REQUIRE(n3.data() == 3);
        REQUIRE(n3.is_front() == false);
        REQUIRE(n3.is_back() == false);
        REQUIRE(n3.is_linked() == false);

        for (const auto& node : n1) {
            nodes.push_back(node);
        }

        REQUIRE(nodes == std::vector {1});
    }

    n1.push_back(n2);

    SECTION("Two nodes") {
        REQUIRE(n1.data() == 1);
        REQUIRE(n1.is_front() == true);
        REQUIRE(n1.is_back() == false);
        REQUIRE(n1.is_linked() == true);

        REQUIRE(n2.data() == 2);
        REQUIRE(n2.is_front() == false);
        REQUIRE(n2.is_back() == true);
        REQUIRE(n2.is_linked() == true);

        REQUIRE(n3.data() == 3);
        REQUIRE(n3.is_front() == false);
        REQUIRE(n3.is_back() == false);
        REQUIRE(n3.is_linked() == false);

        for (const auto& node : n1) {
            nodes.push_back(node);
        }

        REQUIRE(nodes == std::vector {1, 2});
    }

    n1.push_back(n3);

    SECTION("Three nodes") {
        REQUIRE(n1.data() == 1);
        REQUIRE(n1.is_front() == true);
        REQUIRE(n1.is_back() == false);
        REQUIRE(n1.is_linked() == true);

        REQUIRE(n2.data() == 2);
        REQUIRE(n2.is_front() == false);
        REQUIRE(n2.is_back() == false);
        REQUIRE(n2.is_linked() == true);

        REQUIRE(n3.data() == 3);
        REQUIRE(n3.is_front() == false);
        REQUIRE(n3.is_back() == true);
        REQUIRE(n3.is_linked() == true);

        for (const auto& node : n1) {
            nodes.push_back(node);
        }

        REQUIRE(nodes == std::vector {1, 2, 3});
    }

    n2.remove();

    SECTION("Two nodes again") {
        REQUIRE(n1.data() == 1);
        REQUIRE(n1.is_front() == true);
        REQUIRE(n1.is_back() == false);
        REQUIRE(n1.is_linked() == true);

        REQUIRE(n2.data() == 2);
        REQUIRE(n2.is_front() == false);
        REQUIRE(n2.is_back() == false);
        REQUIRE(n2.is_linked() == false);

        REQUIRE(n3.data() == 3);
        REQUIRE(n3.is_front() == false);
        REQUIRE(n3.is_back() == true);
        REQUIRE(n3.is_linked() == true);

        for (const auto& node : n1) {
            nodes.push_back(node);
        }

        REQUIRE(nodes == std::vector {1, 3});
    }

    n1.remove();

    SECTION("One node again") {
        REQUIRE(n1.data() == 1);
        REQUIRE(n1.is_front() == false);
        REQUIRE(n1.is_back() == false);
        REQUIRE(n1.is_linked() == false);

        REQUIRE(n2.data() == 2);
        REQUIRE(n2.is_front() == false);
        REQUIRE(n2.is_back() == false);
        REQUIRE(n2.is_linked() == false);

        REQUIRE(n3.data() == 3);
        REQUIRE(n3.is_front() == false);
        REQUIRE(n3.is_back() == false);
        REQUIRE(n3.is_linked() == false);

        for (const auto& node : n1) {
            nodes.push_back(node);
        }

        REQUIRE(nodes == std::vector {1});

        nodes.clear();

        for (const auto& node : n2) {
            nodes.push_back(node);
        }

        REQUIRE(nodes == std::vector {2});

        nodes.clear();

        for (const auto& node : n3) {
            nodes.push_back(node);
        }

        REQUIRE(nodes == std::vector {3});
    }
}

TEST_CASE("linked_node | try to break it", "[linked_node]") {
    rav::linked_node n1(1);
    rav::linked_node n2(2);
    rav::linked_node n3(3);

    std::vector<int> nodes;

    n1.push_back(n2);
    n1.push_back(n3);

    SECTION("Adding a node twice should keep integrity") {
        n1.push_back(n2);

        for (const auto& node : n1) {
            nodes.push_back(node);
        }

        REQUIRE(nodes == std::vector {1, 3, 2});
    }

    SECTION("When a node goes out of scope it should remove itself") {
        {
            rav::linked_node n4(4);
            n1.push_back(n4);

            for (const auto& node : n1) {
                nodes.push_back(node);
            }
            REQUIRE(nodes == std::vector {1, 2, 3, 4});
        }

        nodes.clear();
        for (const auto& node : n1) {
            nodes.push_back(node);
        }
        REQUIRE(nodes == std::vector {1, 2, 3});
    }
}
