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
#include "ravenna_rtsp_client.hpp"
#include "ravenna_receiver.hpp"
#include "ravennakit/dnssd/dnssd_browser.hpp"

#include <string>

namespace rav {

/**
 * This class contains all the components to act like a RAVENNA node as specified in the RAVENNA protocol.
 */
class ravenna_node {
  public:
    ravenna_node();
    ~ravenna_node();

    void create_receiver(const std::string& ravenna_session_name);

    /**
     * Adds a subscriber to the browser.
     * This method can be called from any thread, and will wait until the operation is complete.
     * @param subscriber The subscriber to add.
     */
    void subscribe_to_browser(ravenna_browser::subscriber* subscriber);

    /**
     * Removes a subscriber from the browser.
     * This method can be called from any thread, and will wait until the operation is complete.
     * @param subscriber The subscriber to remove.
     */
    void unsubscribe_from_browser(ravenna_browser::subscriber* subscriber);

  private:
    asio::io_context io_context_;
    std::thread maintenance_thread_;

    ravenna_browser browser_ {io_context_};

    std::vector<ravenna_receiver> receivers_;
};

}  // namespace rav
