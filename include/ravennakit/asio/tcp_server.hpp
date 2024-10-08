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
#include "ravennakit/core/log.hpp"

#include <asio.hpp>

namespace rav {

class tcp_server {
  public:
    tcp_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint) :
        acceptor_(asio::make_strand(io_context), endpoint) {
        async_accept();
    }

    ~tcp_server() {
        stop();
    }

    [[nodiscard]] uint16_t port() const {
        return acceptor_.local_endpoint().port();
    }

    void stop() {

        std::unique_lock lock(mutex_);
        asio::post(acceptor_.get_executor(), [this, lock = std::move(lock)] mutable {
            // acceptor_.cancel();
            acceptor_.close();

            asio::post(acceptor_.get_executor(), [lock = std::move(lock)] {});
        });

        // Wait for the task above to finish
        std::lock_guard guard(mutex_);
    }

  private:
    asio::ip::tcp::acceptor acceptor_;
    std::mutex mutex_;


    void async_accept() {
        acceptor_.async_accept([this](const std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                RAV_TRACE("Accepting connection from: {}", socket.remote_endpoint().address().to_string());
                async_accept();
            }
        });
    }
};

}  // namespace rav
