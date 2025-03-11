/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#pragma once

namespace rav {

/**
 * Little helper class which keeps track of how many instances of this class have been created and how many are still
 * alive. Useful to track object creation and destruction in tests.
 */
class counted_object {
  public:
    inline static size_t global_instances_created {};
    inline static size_t global_instances_alive {};
    size_t index {};

    counted_object() : index(global_instances_created++) {
        global_instances_alive++;
    }

    ~counted_object() {
        global_instances_alive--;
    }

    static void reset() {
        global_instances_created = 0;
        global_instances_alive = 0;
    }

    counted_object(const counted_object&) = delete;
    counted_object& operator=(const counted_object&) = delete;

    counted_object(counted_object&&) = delete;
    counted_object& operator=(counted_object&&) = delete;
};

}  // namespace rav
