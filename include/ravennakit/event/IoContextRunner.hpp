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

#include <asio.hpp>

class IoContextRunner {
  public:
    IoContextRunner() = default;

    /**
     * Constructs a runner with a specific number of threads.
     * @param num_threads Number of threads to run the io_context on.
     */
    explicit IoContextRunner(const size_t num_threads) : num_threads_(num_threads) {}

    ~IoContextRunner() {
        stop();
    }

    /**
     * Starts the runner asynchronously, returning immediately. Suitable for cases where this runner works alongside an
     * event loop. Call stop() to stop the runner.
     */
    void run_async() {
        start();
    }

    /**
     * Starts the runner synchronously, and executes work on the io_context until it is stopped. Suitable for cases
     * where this runner is the main event loop. Call stop() to stop the runner.
     */
    void run() {
        start();
        io_context_.run();
    }

    /**
     * Stops the runner and joins all threads.
     */
    void stop() {
        io_context_.stop();
        for (auto& thread : threads_) {
            thread.join();
        }
        threads_.clear();
    }

    /**
     * @return The io_context used by this runner.
     */
    asio::io_context& io_context() {
        return io_context_;
    }

  private:
    const size_t num_threads_ = std::thread::hardware_concurrency();
    asio::io_context io_context_ {};
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_ {make_work_guard(io_context_)};
    std::vector<std::thread> threads_ {};

    void start() {
        stop();

        threads_.clear();
        io_context_.restart();

        for (size_t i = 0; i < num_threads_; i++) {
            threads_.emplace_back([this, i] {
                try {
                    io_context_.run();
                } catch (const std::exception& e) {
                    std::cerr << "Exception thrown on io_context runner thread: " << e.what() << "\n";
                } catch (...) {
                    std::cerr << "Unknown exception thrown on io_context runner thread\n";
                }

                std::cout << "IoContextRunner thread " << i << " exited\n";
            });
        }
    }
};
