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

#include <uvw/loop.h>
#include <uvw/udp.h>

#include "rtp_packet_view.hpp"

namespace rav {

struct rtp_packet_event {
    rtp_packet_view packet;
};

class rtp_receiver : public uvw::emitter<rtp_receiver, rtp_packet_event> {
  public:
    using udp_flags = uvw::udp_handle::udp_flags;

    rtp_receiver() = delete;
    ~rtp_receiver() override;

    rtp_receiver(const rtp_receiver&) = delete;
    rtp_receiver& operator=(const rtp_receiver&) = delete;

    rtp_receiver(rtp_receiver&&) = delete;
    rtp_receiver& operator=(rtp_receiver&&) = delete;

    explicit rtp_receiver(const std::shared_ptr<uvw::loop>& loop) : loop_(loop) {}

    int
    start_receiving_unicast(const std::string& address, uint16_t port = 5004, udp_flags opts = udp_flags::_UVW_ENUM);

  private:
    std::shared_ptr<uvw::loop> loop_;
    std::shared_ptr<uvw::udp_handle> rtp_socket_;
    std::shared_ptr<uvw::udp_handle> rtcp_socket_;
};

}  // namespace rav
