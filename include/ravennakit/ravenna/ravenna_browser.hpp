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

#include "ravennakit/core/subscriber_list.hpp"
#include "ravennakit/dnssd/dnssd_browser.hpp"

namespace rav {

/**
 * Convenience class which contains a dnssd browser for nodes and sessions.
 */
class ravenna_browser {
  public:
    /**
     * Baseclass for other classes which need updates on discovered nodes and sessions.
     */
    class subscriber {
      public:
        virtual ~subscriber() = default;

        /**
         * Called when a node is discovered.
         * @param event The event containing the service description.
         */
        virtual void ravenna_node_discovered(const dnssd::dnssd_browser::service_resolved& event) {
            std::ignore = event;
        }

        /**
         * Called when a node is removed.
         * @param event The event containing the service description.
         */
        virtual void ravenna_node_removed(const dnssd::dnssd_browser::service_removed& event) {
            std::ignore = event;
        }

        /**
         * Called when a session is discovered.
         * @param event The event containing the service description.
         */
        virtual void ravenna_session_discovered(const dnssd::dnssd_browser::service_resolved& event) {
            std::ignore = event;
        }

        /**
         * Called when a session is removed.
         * @param event The event containing the service description.
         */
        virtual void ravenna_session_removed(const dnssd::dnssd_browser::service_removed& event) {
            std::ignore = event;
        }
    };

    explicit ravenna_browser(asio::io_context& io_context);

    /**
     * Subscribes a subscriber to the browser.
     * @param subscriber The subscriber to subscribe.
     */
    void subscribe(subscriber* subscriber);

    /**
     * Unsubscribes a subscriber from the browser.
     * @param subscriber The subscriber to unsubscribe.
     */
    void unsubscribe(subscriber* subscriber);

  private:
    std::unique_ptr<dnssd::dnssd_browser> node_browser_;
    dnssd::dnssd_browser::subscriber node_browser_subscriber_;

    std::unique_ptr<dnssd::dnssd_browser> session_browser_;
    dnssd::dnssd_browser::subscriber session_browser_subscriber_;

    subscriber_list<subscriber> subscribers_;
};

}  // namespace rav
