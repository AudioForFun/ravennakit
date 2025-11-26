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

#include "ravennakit/rtsp/detail/rtsp_connection.hpp"

rav::rtsp::Connection::~Connection() = default;

rav::rtsp::Connection::Connection(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {
    parser_.on_request = [this](const Request& request) {
        if (subscriber_) {
            subscriber_->on_request(*this, request);
        }
    };

    parser_.on_response = [this](const Response& response) {
        if (subscriber_) {
            subscriber_->on_response(*this, response);
        }
    };
}

void rav::rtsp::Connection::async_send_response(const Response& response) {
    const auto encoded = response.encode();
    RAV_LOG_TRACE("Sending response: {}", response.to_debug_string(false));
    async_send_data(encoded);
}

void rav::rtsp::Connection::async_send_request(const Request& request) {
    const auto encoded = request.encode();
    RAV_LOG_TRACE("Sending {}", request.to_debug_string(false));
    async_send_data(encoded);
}

void rav::rtsp::Connection::shutdown() {
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}

void rav::rtsp::Connection::start() {
    async_read_some();
}

void rav::rtsp::Connection::stop() {
    socket_.close();
}

void rav::rtsp::Connection::set_subscriber(Subscriber* subscriber_to_set) {
    subscriber_ = subscriber_to_set;
}

void rav::rtsp::Connection::async_connect(const boost::asio::ip::tcp::resolver::results_type& results) {
    auto self = shared_from_this();
    boost::asio::async_connect(
        socket_, results,
        [self](const boost::system::error_code ec, const boost::asio::ip::tcp::endpoint& endpoint) {
            if (ec) {
                RAV_LOG_ERROR("Failed to connect: {}", ec.message());
                return;
            }
            RAV_LOG_INFO("Connected to {}:{}", endpoint.address().to_string(), endpoint.port());
            self->async_write();      // Schedule a write operation, in case there is data to send
            self->async_read_some();  // Start reading chain
            if (self->subscriber_) {
                self->subscriber_->on_connect(*self);
            }
        }
    );
}

void rav::rtsp::Connection::async_send_data(const std::string& data) {
    const bool should_trigger_async_write = output_buffer_.exhausted() && socket_.is_open();
    output_buffer_.write(data);
    if (should_trigger_async_write) {
        async_write();
    }
}

void rav::rtsp::Connection::async_write() {
    if (output_buffer_.exhausted()) {
        return;
    }
    auto self = shared_from_this();
    boost::asio::async_write(
        socket_, boost::asio::buffer(output_buffer_.data()),
        [self](const boost::system::error_code ec, const std::size_t length) {
            if (ec) {
                RAV_LOG_ERROR("Write error: {}", ec.message());
                return;
            }
            self->output_buffer_.consume(length);
            if (!self->output_buffer_.exhausted()) {
                self->async_write();  // Schedule another write
            }
        }
    );
}

void rav::rtsp::Connection::async_read_some() {
    auto buffer = input_buffer_.prepare(512);
    auto self = shared_from_this();
    socket_.async_read_some(
        boost::asio::buffer(buffer.data(), buffer.size_bytes()),
        [self](const boost::system::error_code ec, const std::size_t length) mutable {
            if (ec) {
                self->subscriber_->on_disconnect(*self);
                if (ec == boost::asio::error::operation_aborted) {
                    RAV_LOG_TRACE("Operation aborted");
                    return;
                }
                if (ec == boost::asio::error::eof) {
                    RAV_LOG_TRACE("EOF");
                    return;
                }
                RAV_LOG_ERROR("Read error: {}. Closing connection.", ec.message());
                return;
            }

            self->input_buffer_.commit(length);

            auto result = self->parser_.parse(self->input_buffer_);
            if (!(result == Parser::result::good || result == Parser::result::indeterminate)) {
                RAV_LOG_ERROR("Parsing error: {}", static_cast<int>(result));
                return;
            }

            self->async_read_some();
        }
    );
}

boost::asio::ip::tcp::endpoint rav::rtsp::Connection::remote_endpoint() const {
    return socket_.remote_endpoint();
}
