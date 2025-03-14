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

#include "ravenna_rtsp_client.hpp"
#include "ravennakit/rtp/rtp_stream_receiver.hpp"
#include "ravennakit/rtp/detail/rtp_receiver.hpp"
#include "ravennakit/sdp/sdp_session_description.hpp"

namespace rav {

class ravenna_receiver: public rtp_stream_receiver, public ravenna_rtsp_client::subscriber {
  public:
    explicit ravenna_receiver(ravenna_rtsp_client& rtsp_client, rtp_receiver& rtp_receiver);
    ~ravenna_receiver() override;

    ravenna_receiver(const ravenna_receiver&) = delete;
    ravenna_receiver& operator=(const ravenna_receiver&) = delete;

    ravenna_receiver(ravenna_receiver&&) noexcept = delete;
    ravenna_receiver& operator=(ravenna_receiver&&) noexcept = delete;

    /**
     * Subscribes to a session.
     * @param session_name The name of the session to subscribe to.
     * @return true if the subscriber was added, or false if it was already in the list.
     */
    [[nodiscard]] bool subscribe_to_session(const std::string& session_name);

    /**
     * @return The name of the session, which is empty if not subscribed.
     */
    const std::string& get_session_name() const;

    /**
     * @return The SDP for the session.
     */
    std::optional<sdp::session_description> get_sdp() const;

    /**
     * @return The SDP text for the session. This is the original SDP text as receiver from the server potentially
     * including things which haven't been parsed into the session_description.
     */
    [[nodiscard]] std::optional<std::string> get_sdp_text() const;

    // ravenna_rtsp_client::subscriber overrides
    void on_announced(const ravenna_rtsp_client::announced_event& event) override;

  private:
    ravenna_rtsp_client& rtsp_client_;
    std::string session_name_;
};

}  // namespace rav
