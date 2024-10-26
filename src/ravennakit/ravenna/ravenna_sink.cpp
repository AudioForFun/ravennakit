/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/ravenna/ravenna_sink.hpp"
#include "ravennakit/core/todo.hpp"

rav::ravenna_sink::ravenna_sink(ravenna_browser& browser, std::string session_name) :
    session_name_(std::move(session_name)) {
    ravenna_browser_subscriber_->on<ravenna_session_resolved>([this](const ravenna_session_resolved& event) {
        RAV_INFO("RAVENNA Stream resolved: {} ({})", event.description.name, session_name_);
    });
    browser.subscribe(ravenna_browser_subscriber_);
}

void rav::ravenna_sink::set_manual_sdp(sdp::session_description sdp) {
    sdp_ = std::move(sdp);
}

void rav::ravenna_sink::subscribe_to_session_name(std::string session_name) {
    std::ignore = session_name;
    TODO("Implement");
}
