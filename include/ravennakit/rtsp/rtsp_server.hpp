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

#include "detail/rtsp_connection.hpp"
#include "detail/rtsp_request.hpp"
#include "detail/rtsp_response.hpp"
#include "ravennakit/core/events.hpp"

#include <asio.hpp>
#include <unordered_map>

namespace rav::rtsp {

/**
 * Server for accepting RTSP connections.
 * This class assumes a single threaded io_context and no attempt to synchronise access and callbacks have been made.
 */
class server final: connection::subscriber {
  public:
    using request_handler = std::function<void(connection::request_event)>;

    /**
     * Baseclass for other classes which need to handle requests for specific paths.
     */
    class path_handler {
    public:
        virtual ~path_handler() = default;

        /**
         * Called when a request is received.
         * @param event The event containing the request.
         */
        virtual void on_request([[maybe_unused]] connection::request_event event) const {}

        /**
         * Called when a response is received.
         * @param event The event containing the response.
         */
        virtual void on_response([[maybe_unused]] connection::response_event event) {}
    };

    server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint);
    server(asio::io_context& io_context, const char* address, uint16_t port);
    ~server() override;

    /**
     * @returns The port the server is listening on.
     */
    [[nodiscard]] uint16_t port() const;

    /**
     * Sets given handler to handle requests for given path.
     * @param path The path to associate the handler with. The path should NOT be uri encoded.
     * @param handler The handler to set. If the handler is nullptr it will remove any previously registered handler for
     * path.
     */
    void register_handler(const std::string& path, path_handler* handler);

    /**
     * Removes given handler from all paths.
     */
    void unregister_handler(const path_handler* handler_to_remove);

    /**
     * Sends a request to all connected clients. The path will determine which clients will receive the request.
     * @param path The path to send the request to.
     * @param request The request to send.
     */
    void send_request(const std::string& path, const request& request) const;

    /**
     * Closes the listening socket. Implies cancellation.
     */
    void stop();

    /**
     * Resets handlers for all paths.
     */
    void reset() noexcept;

  protected:
    void on_connect(connection& connection) override;
    void on_request(connection& connection, const request& request) override;
    void on_response(connection& connection, const response& response) override;
    void on_disconnect(connection& connection) override;

  private:
    static constexpr auto k_special_path_all = "/all";

    struct path_context {
        path_handler* handler;
        std::vector<std::shared_ptr<connection>> connections;
    };

    asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<std::string, path_context> paths_;

    void async_accept();
};

}  // namespace rav
