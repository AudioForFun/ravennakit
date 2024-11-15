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

#include <functional>
#include <vector>

namespace rav {

/**
 * Super basic list of subscribers. It is not thread safe.
 * Also, make sure that the subscriber is not destroyed while it is in the list.
 * @tparam T The type of the subscriber.
 */
template<class T>
class subscriber_list {
  public:
    subscriber_list() = default;

    subscriber_list(const subscriber_list&) = delete;
    subscriber_list& operator=(const subscriber_list&) = delete;

    subscriber_list(subscriber_list&&) = default;
    subscriber_list& operator=(subscriber_list&&) = default;

    /**
     * @returns An iterator to the beginning of the list.
     */
    auto begin() const {
        return subscribers_.begin();
    }

    /**
     * @returns An iterator to the end of the list.
     */
    auto end() const {
        return subscribers_.end();
    }

    /**
     * @returns An iterator to the beginning of the list.
     */
    auto begin() {
        return subscribers_.begin();
    }

    /**
     * @returns An iterator to the end of the list.
     */
    auto end() {
        return subscribers_.end();
    }

    /**
     * Adds the given subscriber to the list. If subscriber is already in the list it will not be added.
     * @param subscriber The subscriber to add.
     * @return true if the subscriber was added, or false if it was already in the list.
     */
    bool add(T* subscriber) {
        if (contains(subscriber)) {
            return false;
        }
        subscribers_.push_back(subscriber);
        return true;
    }

    /**
     * Removes the given subscriber from the list.
     * @param subscriber The subscriber to remove.
     * @returns true if the subscriber was removed, or false if it was not in the list.
     */
    bool remove(T* subscriber) {
        auto it = std::find(subscribers_.begin(), subscribers_.end(), subscriber);
        if (it == subscribers_.end()) {
            return false;
        }
        subscribers_.erase(it);
        return true;
    }

    /**
     * Clears the list.
     */
    void clear() {
        subscribers_.clear();
    }

    /**
     * Calls given function for each subscriber.
     * @param f The function to call for each subscriber. Must be not null.
     * @param excluding The subscriber to exclude from the call.
     */
    void foreach (const std::function<void(T*)>& f, const T* excluding = nullptr) {
        for (auto subscriber : subscribers_) {
            if (subscriber != excluding) {
                f(subscriber);
            }
        }
    }

    /**
     * @returns The number of subscribers.
     */
    [[nodiscard]] size_t size() const {
        return subscribers_.size();
    }

    /**
     * @return true if there are no subscribers.
     */
    [[nodiscard]] bool empty() const {
        return subscribers_.empty();
    }

    /**
     * Checks if the list contains the given subscriber.
     * @param subscriber The subscriber to check.
     * @return true if the list contains the subscriber, or false if not.
     */
    bool contains(T* subscriber) const {
        return std::find(subscribers_.begin(), subscribers_.end(), subscriber) != subscribers_.end();
    }

  private:
    std::vector<T*> subscribers_;
};

}  // namespace rav
