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
#include "ravennakit/core/assert.hpp"

#include <mutex>

namespace rav {

/**
 * This class behaves like a Read-Copy-Update (RCU) synchronization mechanism and allows to share objects among multiple
 * readers which can read the most recent value in a wait-free manner (which also implies lock-free).
 * The writer side is protected by a mutex and can update the value in a blocking manner.
 * It's important to reclaim memory by calling reclaim() periodically to delete outdated values. As long as there are
 * readers using an object, the object and newer objects won't be deleted.
 *
 * To give a realtime thread access to objects, create a reader object and use the read_lock() method to get a lock. The
 * lock provides access to the object through the * and -> operators, as well as the get() method. One important thing
 * to keep in mind is that as long as there is at least one lock active, new locks will get the same current value of
 * the reader and will not update the reader and the lock to use the latest one. Usually this is not a problem because a
 * lock should be short-lived anyway.
 *
 * @tparam T The type of the object to share.
 */
template<class T>
class rcu {
  public:
    /**
     * A reader object gives a single thread access to the most recent value. Store a reader object per thread, and use
     * read_lock to acquire a lock which in turn provides access to the value.
     */
    class reader {
      public:
        /**
         * A lock object provides access to the value. One important thing to keep in mind is that as long as there is
         * at least one lock active, new locks will get the same current value of the reader and will not update the
         * reader and the lock to use the latest one. Usually this is not a problem because a lock should be short-lived
         * anyway.
         *
         * Getting and using a lock is wait-free.
         */
        class read_lock {
          public:
            /**
             * Constructs a lock from given reader.
             * All methods are real-time safe (wait-free), but not thread safe.
             * @param parent_reader The reader to associate this lock with.
             */
            explicit read_lock(reader& parent_reader) : reader_(parent_reader) {
                if (reader_.num_locks_ >= 1) {
                    value_ = reader_.reader_value_;
                } else {
                    RAV_ASSERT(reader_.reader_value_ == nullptr, "Reader value should be nullptr");
                    reader_.reader_value_.store(reader_.owner_.most_recent_value_.load());
                    value_ = reader_.reader_value_;
                }
                reader_.num_locks_ += 1;
            }

            ~read_lock() {
                if (value_ && reader_.num_locks_ == 1) {
                    reader_.reader_value_.store(nullptr);
                }
                reader_.num_locks_ -= 1;
                RAV_ASSERT_NO_THROW(reader_.num_locks_ >= 0, "Number of locks should be non-negative");
            }

            /**
             * @return A reference to the contained object. Reference is only valid if the value is not nullptr.
             */
            const T& operator*() const {
                RAV_ASSERT(value_ != nullptr, "Value is nullptr");
                return *value_;
            }

            /**
             * @return A pointer to the contained object, or nullptr if the value is nullptr.
             */
            const T* operator->() const {
                RAV_ASSERT(value_ != nullptr, "Value is nullptr");
                return value_;
            }

            read_lock(const read_lock&) = delete;
            read_lock& operator=(const read_lock&) = delete;
            read_lock(read_lock&&) = delete;
            read_lock& operator=(read_lock&&) = delete;

            /**
             * @return A pointer to the contained object, or nullptr if the value is nullptr.
             */
            T* get() {
                return value_;
            }

            /**
             * @return A pointer to the contained object, or nullptr if the value is nullptr.
             */
            const T* get() const {
                return value_;
            }

          private:
            reader& reader_;
            T* value_ {nullptr};
        };

        /**
         * Constructs a reader object and registers it with the owner.
         * Real-time safe: no.
         * Thread safe: yes.
         * @param owner The owner of this reader.
         */
        explicit reader(rcu& owner) : owner_(owner) {
            std::lock_guard lock(owner_.readers_mutex_);
            owner_.readers_.push_back(this);
        }

        ~reader() {
            std::lock_guard lock(owner_.readers_mutex_);
            owner_.readers_.erase(
                std::remove(owner_.readers_.begin(), owner_.readers_.end(), this), owner_.readers_.end()
            );
        }

        /**
         * Creates a lock object which provides access to the value.
         * Real-time safe: wait-free.
         * Thread safe: no.
         * @return The lock object.
         */
        read_lock lock() {
            return read_lock(*this);
        }

      private:
        rcu& owner_;
        std::atomic<T*> reader_value_ {nullptr};
        int64_t num_locks_ {0};
    };

    /**
     * @return A reader object which uses this rcu object.
     */
    reader create_reader() {
        return reader(*this);
    }

    /**
     * Updates the current value with a new value constructed from the given arguments.
     * Real-time safe: no.
     * Thread safe: yes.
     * @tparam Args
     * @param args
     */
    template<class... Args>
    void update(Args&&... args) {
        update(std::make_unique<T>(std::forward<Args>(args)...));
    }

    /**
     * Updates the current value with a new value.
     * Real-time safe: no.
     * Thread safe: yes.
     * @param new_value New value to set.
     */
    void update(std::unique_ptr<T> new_value) {
        if (new_value == nullptr) {
            return;
        }
        std::lock_guard lock(values_mutex_);
        values_.push_back(std::move(new_value));
        most_recent_value_ = values_.back().get();
    }

    /**
     * Reclaims all values which are not used by any reader anymore. Only older objects than the first object used by
     * any reader are deleted.
     */
    void reclaim() {
        std::lock_guard lock(values_mutex_);

        RAV_ASSERT(!values_.empty(), "The last value should have never been reclaimed");

        for (auto it = values_.begin(); it != values_.end() - 1;) {
            if (has_reader_using_object(it->get())) {
                ++it;
                break;  // Don't delete values newer than the oldest used value.
            }
            it = values_.erase(it);
        }
    }

  private:
    // Protects the values_ vector.
    std::mutex values_mutex_;

    // Holds the current and previous values.
    std::vector<std::unique_ptr<T>> values_;

    // Stores the most recent value.
    std::atomic<T*> most_recent_value_ {nullptr};

    // Protects the readers_ vector.
    std::mutex readers_mutex_;

    // Holds the readers.
    std::vector<reader*> readers_;

    bool has_reader_using_object(const T* object) const {
        for (const auto* reader : readers_) {
            if (reader->reader_value_.load() == object) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace rav
