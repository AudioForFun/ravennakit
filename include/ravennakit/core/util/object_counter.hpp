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
 * Holds the number of instances created and destroyed. Useful for tracking object creation and destruction in tests.
 */
class object_counter {
  public:
    size_t instances_created = 0;
    size_t instances_alive = 0;
};

/**
 * Little helper class which keeps track of how many instances of this class have been created and how many are still
 * alive. Useful to track object creation and destruction in tests.
 */
class counted_object {
  public:
    explicit counted_object(object_counter& counter) : counter_(counter), index_(counter.instances_created++) {
        ++counter_.instances_alive;
    }

    ~counted_object() {
        --counter_.instances_alive;
    }

    counted_object(const counted_object&) = delete;
    counted_object& operator=(const counted_object&) = delete;

    counted_object(counted_object&&) = delete;
    counted_object& operator=(counted_object&&) = delete;

    /**
     *
     * @return The index of the object, which is based on given counter.
     */
    [[nodiscard]] size_t index() const {
        return index_;
    }

  private:
    object_counter& counter_;
    size_t index_ {};
};

}  // namespace rav
