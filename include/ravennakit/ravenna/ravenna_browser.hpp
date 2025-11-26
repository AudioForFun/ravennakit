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

#pragma once

#include "ravennakit/core/util/subscriber_list.hpp"
#include "ravennakit/dnssd/dnssd_browser.hpp"
#include "ravennakit/core/expected.hpp"

namespace rav {

/**
 * Convenience class which contains a dnssd browser for nodes and sessions.
 */
class RavennaBrowser {
  public:
    /**
     * Baseclass for other classes which need updates on discovered nodes and sessions.
     */
    class Subscriber {
      public:
        virtual ~Subscriber() = default;

        /**
         * Called when a node is discovered.
         * @param desc The service description of the discovered node.
         */
        virtual void ravenna_node_discovered([[maybe_unused]] const dnssd::ServiceDescription& desc) {}

        /**
         * Called when a node is removed.
         * @param desc The service description of the removed node.
         */
        virtual void ravenna_node_removed([[maybe_unused]] const dnssd::ServiceDescription& desc) {}

        /**
         * Called when a session is discovered.
         * @param desc The service description of the discovered session.
         */
        virtual void ravenna_session_discovered([[maybe_unused]] const dnssd::ServiceDescription& desc) {}

        /**
         * Called when a session is removed.
         * @param desc The service description of the removed session.
         */
        virtual void ravenna_session_removed([[maybe_unused]] const dnssd::ServiceDescription& desc) {}
    };

    explicit RavennaBrowser(boost::asio::io_context& io_context);

    /**
     * Finds a node by its name.
     * @param session_name The name of the node to find.
     * @return The service description of the node, or nullptr if not found.
     */
    [[nodiscard]] const dnssd::ServiceDescription* find_session(const std::string& session_name) const;

    /**
     * Finds a node by its name.
     * @param node_name The name of the node to find.
     * @return The service description of the node, or nullptr if not found.
     */
    [[nodiscard]] const dnssd::ServiceDescription* find_node(const std::string& node_name) const;

    /**
     * Adds a subscriber to the browser.
     * @param subscriber_to_add The subscriber to add.
     * @return true if the subscriber was added, or false if it was already in the list.
     */
    [[nodiscard]] bool subscribe(Subscriber* subscriber_to_add);

    /**
     * Removes a subscriber from the browser.
     * @param subscriber_to_remove The subscriber to remove.
     * @return true if the subscriber was removed, or false if it wasn't found.
     */
    [[nodiscard]] bool unsubscribe(const Subscriber* subscriber_to_remove);

    /**
     * Enables or disables browsing for nodes.
     * @param enabled True to enable browsing, false to disable browsing.
     */
    tl::expected<void, const char*> set_node_browsing_enabled(bool enabled);

    /**
     * Enables or disables browsing for nodes.
     * @param enabled True to enable browsing, false to disable browsing.
     */
    tl::expected<void, const char*> set_session_browsing_enabled(bool enabled);

  private:
    boost::asio::io_context& io_context_;
    std::unique_ptr<dnssd::Browser> node_browser_;
    std::unique_ptr<dnssd::Browser> session_browser_;

    SubscriberList<Subscriber> subscribers_;
};

}  // namespace rav
