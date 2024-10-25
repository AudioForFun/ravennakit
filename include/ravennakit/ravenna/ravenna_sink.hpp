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

#include "ravennakit/sdp/session_description.hpp"
#include "ravennakit/ravenna/ravenna_browser.hpp"

namespace rav {

class ravenna_sink {
  public:
    explicit ravenna_sink(ravenna_browser& browser, std::string session_name);

    ~ravenna_sink() = default;

    void set_manual_sdp(sdp::session_description sdp);

    void subscribe_to_session_name(std::string session_name);

  private:
    std::optional<sdp::session_description> sdp_;
    std::string session_name_;
    ravenna_browser::subscriber ravenna_browser_subscriber_;
};

}  // namespace rav
