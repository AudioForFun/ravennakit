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

#include "detail/rtp_sender.hpp"

namespace rav::rtp {

class StreamSender {
  public:
    explicit StreamSender(asio::io_context& io_context, const asio::ip::address_v4& interface_address);

    [[nodiscard]] const asio::ip::address_v4& get_interface_address() const {
        return rtp_sender_.get_interface_address();
    }

  protected:
    Sender rtp_sender_;
};

}  // namespace rav::rtp
