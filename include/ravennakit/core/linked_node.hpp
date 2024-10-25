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
 * Baseclass for other classes that need to be linked together.
 * @tparam T The type of the data to be stored in the linked node.
 */
template<class T>
class linked_node {
  public:
    /**
     * Iterator for linked nodes.
     */
    class iterator {
      public:
        explicit iterator(linked_node* node) : current(node) {}

        T& operator*() const {
            return current->data();
        }

        T* operator->() const {
            return &current->data();
        }

        iterator& operator++() {
            if (current)
                current = current->next_;
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++*this;
            return temp;
        }

        iterator& operator--() {
            if (current)
                current = current->prev_;
            return *this;
        }

        iterator operator--(int) {
            iterator temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const iterator& other) const {
            return current != other.current;
        }

      private:
        linked_node* current {};
    };

    /**
     * Creates a new linked node with the given data.
     * @param data The data to be stored in the linked node.
     */
    explicit linked_node(T data) : data_(data) {}

    /**
     * Destructor which removes itself from the linked list if linked.
     */
    ~linked_node() {
        remove();
    }

    /**
     * Returns the first node in the linked list.
     * @return The first node in the linked list, or this if not linked.
     */
    linked_node* front() {
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
    linked_node* back() {
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
    void push_back(linked_node& node) {
        if (node.is_linked()) {
            node.remove();
        }
        auto* last_node = back();
        last_node->next_ = &node;
        node.prev_ = last_node;
    }

    /**
     * Removes the node from the linked list.
     */
    void remove() {
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
    T& data() {
        return data_;
    }

    /**
     * @returns The data stored in the linked node.
     */
    const T& data() const {
        return data_;
    }

    /**
     * @returns An iterator to the first node in the linked list.
     */
    iterator begin() {
        return iterator(front());
    }

    /**
     * @returns An iterator to the end of the linked list.
     */
    iterator end() {
        return iterator(nullptr);
    }

    /**
     *
     * @returns An iterator to the first node in the linked list.
     */
    iterator begin() const {
        return iterator(back());
    }

    /**
     * @returns An iterator to the end of the linked list.
     */
    iterator end() const {
        return iterator(nullptr);
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
    void for_each(const std::function<void(T&)>& f) {
        for (auto& node : *this) {
            f(node);
        }
    }

  private:
    T data_;
    linked_node* prev_ = nullptr;
    linked_node* next_ = nullptr;
};

}  // namespace rav
