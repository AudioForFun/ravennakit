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

#pragma once

namespace rav {

/**
 * Holds the number of instances created and destroyed. Useful for tracking object creation and destruction in tests.
 */
class ObjectCounter {
  public:
    size_t instances_created = 0;
    size_t instances_alive = 0;
};

/**
 * Little helper class which keeps track of how many instances of this class have been created and how many are still
 * alive. Useful to track object creation and destruction in tests.
 */
class CountedObject {
  public:
    CountedObject() = delete;

    explicit CountedObject(ObjectCounter& counter) : counter_(counter), index_(counter.instances_created++) {
        ++counter_.instances_alive;
    }

    ~CountedObject() {
        --counter_.instances_alive;
    }

    CountedObject(const CountedObject&) = delete;
    CountedObject& operator=(const CountedObject&) = delete;

    CountedObject(CountedObject&&) = delete;
    CountedObject& operator=(CountedObject&&) = delete;

    /**
     *
     * @return The index of the object, which is based on given counter.
     */
    [[nodiscard]] size_t index() const {
        return index_;
    }

  private:
    ObjectCounter& counter_;
    size_t index_ {};
};

}  // namespace rav
