/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#pragma once

#include "ravennakit/core/assert.hpp"

#include <cstdint>
#include <atomic>

namespace rav {

/**
 * A class which represents a unique identifier. How unique the identifier is depends on how this class is used. The id
 * itself is a uint64_t.
 */
class Id {
  public:
    class Generator {
      public:
        Generator() = default;

        /**
         * This function is thread safe and can be safely called from any thread at any time.
         * @return The next unique ID
         */
        [[nodiscard]] Id next() {
            RAV_ASSERT(next_id_ != 0, "Next ID is 0, which is reserved for invalid IDs");
            RAV_ASSERT(next_id_ != std::numeric_limits<uint64_t>::max(), "The next ID is at the maximum value");
            return Id(next_id_.fetch_add(1));
        }

      private:
        std::atomic<uint64_t> next_id_ {1};
    };

    Id() = default;

    /**
     * Constructs an id from an integer value.
     * @param int_id The integer value of the ID
     */
    explicit Id(const uint64_t int_id) : id_(int_id) {}

    Id(const Id& other) = default;
    Id(Id&& other) noexcept = default;

    Id& operator=(const Id& other) = default;
    Id& operator=(Id&& other) noexcept = default;

    /**
     * @return True if the id is not 0, false otherwise.
     */
    [[nodiscard]] bool is_valid() const noexcept {
        return id_ != 0;
    }

    /**
     * @return The integer value of the ID.
     */
    [[nodiscard]] uint64_t value() const {
        return id_;
    }

    /**
     * @return The integer value of the ID as string.
     */
    [[nodiscard]] std::string to_string() const {
        return std::to_string(id_);
    }

    bool operator==(const uint64_t value) const {
        return id_ == value;
    }

    friend bool operator==(const Id& lhs, const Id& rhs) {
        return lhs.id_ == rhs.id_;
    }

    friend bool operator!=(const Id& lhs, const Id& rhs) {
        return !(lhs == rhs);
    }

    /**
     * Returns the next id from a process-wide, global generator.
     * This function is thread safe and can be safely called from any thread at any time.
     * @return The next id.
     */
    static Id next_process_wide_unique_id() noexcept {
        static Generator gen;
        return gen.next();
    }

  private:
    uint64_t id_ {};
};

}  // namespace rav::util
