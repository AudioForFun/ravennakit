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

#include "detail/http_fmt_adapters.hpp"

#include <boost/asio.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/string_body_fwd.hpp>
#include <boost/system/result.hpp>

namespace rav {

/**
 * A high level wrapper around boost::beast for creating an HTTP server.
 */
class HttpServer {
  public:
    /// The time to wait for a request before closing the connection.
    static constexpr auto k_timeout_seconds = 5;

    /**
     * Constructs a new HttpServer using the given io_context.
     * The io_context must be single threaded.
     * @param io_context The io_context to use for the server.
     */
    explicit HttpServer(boost::asio::io_context& io_context);

    /**
     * Starts the server on the given host and port.
     * @param host The host to bind to.
     * @param port The port to bind to.
     * @return An error code if the server fails to start, or an empty result if it succeeds.
     */
    boost::system::result<void> start(std::string_view host, uint16_t port);

    /**
     * Stops the server and closes all client sessions.
     */
    void stop();

    /**
     * @return The local (listening) endpoint of the server.
     */
    [[nodiscard]] boost::asio::ip::tcp::endpoint get_local_endpoint() const;

    /**
     * @return The address string of the server in the format "address:port".
     */
    [[nodiscard]] std::string get_address_string() const;

    /**
     * @return The number of active client sessions.
     */
    [[nodiscard]] size_t get_client_count() const;

  private:
    class Listener;
    std::shared_ptr<Listener> listener_;

    class ClientSession;
    std::vector<std::shared_ptr<ClientSession>> client_sessions_;

    boost::asio::io_context& io_context_;

    void on_accept(boost::asio::ip::tcp::socket socket);
    void on_listener_error(const boost::beast::error_code& ec, std::string_view what) const;
    void on_client_error(const boost::beast::error_code& ec, std::string_view what) const;
    boost::beast::http::message_generator on_request(boost::beast::http::request<boost::beast::http::string_body>);
    void remove_client_session(const ClientSession* session);
};

}  // namespace rav
