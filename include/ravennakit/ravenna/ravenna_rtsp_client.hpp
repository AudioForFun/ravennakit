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

#include "ravenna_browser.hpp"
#include "ravennakit/core/events.hpp"
#include "ravennakit/core/linked_node.hpp"
#include "ravennakit/dnssd/dnssd_browser.hpp"
#include "ravennakit/rtsp/rtsp_client.hpp"
#include "ravennakit/sdp/sdp_session_description.hpp"

namespace rav {

/**
 * Maintains connections to one or more RAVENNA RTSP servers, upon request.
 */
class ravenna_rtsp_client: public ravenna_browser::subscriber {
  public:
    struct announced_event {
        const std::string& session_name;
        const sdp::session_description& sdp;
    };

    class subscriber {
      public:
        subscriber() = default;
        virtual ~subscriber() = default;

        subscriber(const subscriber&) = delete;
        subscriber& operator=(const subscriber&) = delete;

        subscriber(subscriber&&) noexcept = delete;
        subscriber& operator=(subscriber&&) noexcept = delete;

        /**
         * Called when a session is announced.
         * @param event The announced event.
         */
        virtual void on_announced([[maybe_unused]] const announced_event& event) {}
    };

    explicit ravenna_rtsp_client(asio::io_context& io_context, ravenna_browser& browser);
    ~ravenna_rtsp_client() override;

    /**
     * Subscribes to a session.
     * @param subscriber The subscriber to add.
     * @param session_name The name of the session to subscribe to.
     * @return true if the subscriber was added, or false if it was already in the list.
     */
    [[nodiscard]] bool subscribe_to_session(subscriber* subscriber, const std::string& session_name);

    /**
     * Unsubscribes from all sessions.
     * @param subscriber The subscriber to remove.
     * @return true if the subscriber was removed from at least one session, or false if it wasn't.
     */
    [[nodiscard]] bool unsubscribe_from_all_sessions(subscriber* subscriber);

    /**
     * Tries to find the SDP for the given session.
     * @param session_name The name of the session to get the SDP for.
     * @return The SDP for the session, if it exists, otherwise an empty optional.
     */
    [[nodiscard]] std::optional<sdp::session_description> get_sdp_for_session(const std::string& session_name) const;

    /**
     * Tries to find the SDP text for the given session. The difference between this and get_sdp_for_session is that the
     * return value will contain the original SDP text, including things which might not be parsed into the
     * session_description.
     * @param session_name The name of the session to get the SDP text for.
     * @return The SDP text for the session, if it exists, otherwise an empty optional.
     */
    [[nodiscard]] std::optional<std::string> get_sdp_text_for_session(const std::string& session_name) const;

    /**
     * @return The io_context used by this client.
     */
    [[nodiscard]] asio::io_context& get_io_context() const;

    // ravenna_browser::subscriber overrides
    void ravenna_session_discovered(const dnssd::dnssd_browser::service_resolved& event) override;

  private:
    struct session_context {
        std::string session_name;
        subscriber_list<subscriber> subscribers;
        std::optional<sdp::session_description> sdp_;
        std::optional<std::string> sdp_text_;
        std::string host_target;
        uint16_t port {};
    };

    struct connection_context {
        std::string host_target;
        uint16_t port {};
        rtsp_client client;
    };

    asio::io_context& io_context_;
    ravenna_browser& browser_;
    std::vector<session_context> sessions_;
    std::vector<connection_context> connections_;

    connection_context& find_or_create_connection(const std::string& host_target, uint16_t port);
    connection_context* find_connection(const std::string& host_target, uint16_t port);
    void update_session_with_service(session_context& session, const dnssd::service_description& service);
    void do_maintenance();
    void handle_incoming_sdp(const std::string& sdp_text);
};

}  // namespace rav
