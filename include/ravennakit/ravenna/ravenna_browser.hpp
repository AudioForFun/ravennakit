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

#include "ravennakit/core/linked_node.hpp"
#include "ravennakit/dnssd/dnssd_browser.hpp"
#include "ravennakit/rtsp/rtsp_client.hpp"

namespace rav {

struct ravenna_node_resolved {
    const dnssd::service_description& description;
};

struct ravenna_session_resolved {
    const dnssd::service_description& description;
};

/**
 * Discovers RAVENNA nodes and streams.
 */
class ravenna_browser final {
  public:
    class subscriber final:
        public event_emitter<subscriber, ravenna_session_resolved>,
        public linked_node<subscriber*> {
      public:
        using event_emitter::emit;

        subscriber() : linked_node(this) {}

        subscriber(const subscriber&) = delete;
        subscriber& operator=(const subscriber&) = delete;

        subscriber(subscriber&&) = delete;
        subscriber& operator=(subscriber&&) = delete;
    };

    explicit ravenna_browser(asio::io_context& io_context);

    void subscribe(subscriber& subscriber);

  private:
    asio::io_context& io_context_;
    std::unique_ptr<dnssd::dnssd_browser> node_browser_;
    std::unique_ptr<dnssd::dnssd_browser> session_browser_;
    linked_node<subscriber*> subscribers_ {nullptr};
};

}  // namespace rav
