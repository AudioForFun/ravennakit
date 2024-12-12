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

namespace rav {

/**
 * Server for accepting RTSP connections.
 * This class assumes a single threaded io_context and no attempt to synchronise access and callbacks have been made.
 */
class rtsp_server: rtsp_connection::subscriber {
  public:
    using events_type =
        events<rtsp_connection::connect_event, rtsp_connection::request_event, rtsp_connection::response_event>;

    rtsp_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint);
    rtsp_server(asio::io_context& io_context, const char* address, uint16_t port);
    ~rtsp_server() override;

    /**
     * @returns The port the server is listening on.
     */
    [[nodiscard]] uint16_t port() const;

    /**
     * Closes the listening socket. Implies cancellation.
     */
    void stop();

    /**
     * Registers a handler for a specific event.
     * @tparam T The event type.
     * @param handler The handler to register.
     */
    template<class T>
    void on(events_type::handler<T> handler) {
        events_.on(handler);
    }

    /**
     * Resets handlers for all events.
     */
    void reset() noexcept;

  protected:
    void on_connect(rtsp_connection& connection) override;
    void on_request(const rtsp_request& request, rtsp_connection& connection) override;
    void on_response(const rtsp_response& response, rtsp_connection& connection) override;

  private:
    asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<rtsp_connection>> connections_;
    events_type events_;

    void async_accept();
};

}  // namespace rav
