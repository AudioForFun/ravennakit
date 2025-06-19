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

namespace rav {

/**
 * A node which can be linked together into a linked-list, and which can hold data of any type.
 * @tparam T The type of the data to be stored in the linked node.
 */
template<class T>
class LinkedNode {
  public:
    /**
     * Iterator for linked nodes.
     */
    class Iterator {
      public:
        explicit Iterator(LinkedNode* node) : current(node) {}

        LinkedNode& operator*() const {
            return *current;
        }

        Iterator& operator++() {
            if (current) {
                current = current->next_;
            }
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

      private:
        LinkedNode* current {};
    };

    LinkedNode() = default;

    /**
     * Creates a new linked node with the given data.
     * @param data The data to be stored in the linked node.
     */
    explicit LinkedNode(T data) : value_(data) {}

    /**
     * Destructor which removes itself from the linked list if linked.
     */
    ~LinkedNode() {
        unlink();
    }

    LinkedNode(const LinkedNode&) = delete;
    LinkedNode& operator=(const LinkedNode&) = delete;

    /**
     * Constructs a linked node replacing other. After returning, this will have happened:
     * - This will replace other in its linked list. This will now be part of the linked list of other.
     * - Other will be unlinked.
     * - The value contained in other will be moved to this.
     * @param other The linked node to replace.
     */
    LinkedNode(LinkedNode&& other) noexcept {
        if (other.prev_) {
            other.prev_->next_ = this;
            prev_ = other.prev_;
            other.prev_ = nullptr;
        }

        if (other.next_) {
            other.next_->prev_ = this;
            next_ = other.next_;
            other.next_ = nullptr;
        }

        value_ = std::move(other.value_);
        other.value_ = T();
    }

    /**
     * Move assigns other to this. After returning, this will have happened:
     * - This will remove itself from its linked list (if linked).
     * - This will replace other in its linked list. This will now be part of the linked list of other.
     * - The value contained in other will be moved to this.
     * @param other Other node to be moved.
     */
    LinkedNode& operator=(LinkedNode&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        unlink();

        if (other.prev_) {
            other.prev_->next_ = this;
            prev_ = other.prev_;
            other.prev_ = nullptr;
        }

        if (other.next_) {
            other.next_->prev_ = this;
            next_ = other.next_;
            other.next_ = nullptr;
        }

        value_ = std::move(other.value_);
        other.value_ = T();

        return *this;
    }

    /**
     * Assigns a new value to the linked node.
     * @param value The new value to be assigned.
     * @return this
     */
    LinkedNode& operator=(T value) {
        value_ = std::move(value);
        return *this;
    }

    /**
     * Returns the first node in the linked list.
     * @return The first node in the linked list, or this if not linked.
     */
    LinkedNode* front() {
        auto* current = this;
        while (current->prev_ != nullptr) {
            current = current->prev_;
        }
        return current;
    }

    /**
     * Returns the last node in the linked list.
     * @return The last node in the linked list, or this if not linked.
     */
    LinkedNode* back() {
        auto* current = this;
        while (current->next_ != nullptr) {
            current = current->next_;
        }
        return current;
    }

    /**
     * Pushes a node to the back of the linked list. If the node is already linked, it will be removed from its current
     * position.
     * @param node The node to push to the back of the linked list.
     */
    void push_back(LinkedNode& node) {
        if (node.is_linked()) {
            node.unlink();
        }
        auto* last_node = back();
        last_node->next_ = &node;
        node.prev_ = last_node;
    }

    /**
     * Unlinks the node from the linked list.
     */
    void unlink() {
        if (prev_) {
            prev_->next_ = next_;
        }
        if (next_) {
            next_->prev_ = prev_;
        }
        prev_ = nullptr;
        next_ = nullptr;
    }

    /**
     * @returns The data stored in the linked node.
     */
    T& operator*() {
        return value_;
    }

    /**
     * @returns The data stored in the linked node.
     */
    const T& operator*() const {
        return value_;
    }

    /**
     * @returns The data stored in the linked node.
     */
    T* operator->() {
        return &value_;
    }

    /**
     * @returns The data stored in the linked node.
     */
    const T* operator->() const {
        return &value_;
    }

    /**
     * @returns The data stored in the linked node.
     */
    T& value() {
        return value_;
    }

    /**
     * @returns The data stored in the linked node.
     */
    const T& value() const {
        return value_;
    }

    /**
     * Resets the data stored in the linked node to its default value. Does not unlink the node.
     */
    void reset_value() {
        value_ = T();
    }

    /**
     * Unlinks this node and resets the value it holds.
     */
    void reset() {
        unlink();
        reset_value();
    }

    /**
     * @returns An iterator to the first node in the linked list.
     */
    Iterator begin() {
        return Iterator(front());
    }

    /**
     * @returns An iterator to the end of the linked list.
     */
    Iterator end() {
        return Iterator(nullptr);
    }

    /**
     *
     * @returns An iterator to the first node in the linked list.
     */
    Iterator begin() const {
        return Iterator(back());
    }

    /**
     * @returns An iterator to the end of the linked list.
     */
    Iterator end() const {
        return Iterator(nullptr);
    }

    /**
     * @returns True if this is the first node in the linked list, false otherwise.
     */
    [[nodiscard]] bool is_front() const {
        return prev_ == nullptr && next_ != nullptr;
    }

    /**
     * @returns True if this is the last node in the linked list, false otherwise.
     */
    [[nodiscard]] bool is_back() const {
        return next_ == nullptr && prev_ != nullptr;
    }

    /**
     * @returns True if this node is linked to another node, false otherwise.
     */
    [[nodiscard]] bool is_linked() const {
        return prev_ != nullptr || next_ != nullptr;
    }

    /**
     * @param f The function to be called for each node in the linked list.
     */
    void foreach (const std::function<void(LinkedNode&)>& f) {
        for (auto& node : *this) {
            f(node);
        }
    }

  private:
    T value_ {};
    LinkedNode* prev_ = nullptr;
    LinkedNode* next_ = nullptr;
};

}  // namespace rav
