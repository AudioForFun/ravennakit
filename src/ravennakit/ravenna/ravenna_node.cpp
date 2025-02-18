/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/ravenna/ravenna_node.hpp"

rav::ravenna_node::ravenna_node() {
    maintenance_thread_ = std::thread([this] {
        io_context_.run();
    });
}

rav::ravenna_node::~ravenna_node() {
    io_context_.stop();
    if (maintenance_thread_.joinable()) {
        maintenance_thread_.join();
    }
}

void rav::ravenna_node::create_receiver(const std::string& ravenna_session_name) {
    std::ignore = ravenna_session_name;
}

void rav::ravenna_node::subscribe_to_browser(ravenna_browser::subscriber* subscriber) {
    dispatch_wait([this, subscriber] {
        browser_.subscribe(subscriber);
    });
}

void rav::ravenna_node::unsubscribe_from_browser(ravenna_browser::subscriber* subscriber) {
    dispatch_wait([this, subscriber] {
        browser_.unsubscribe(subscriber);
    });
}

void rav::ravenna_node::dispatch_wait(std::function<void()>&& work) {
    RAV_ASSERT(work, "Invalid work");

    std::promise<void> promise;
    const auto future = promise.get_future();

    asio::dispatch(io_context_, [p = std::move(promise), f = std::move(work)]() mutable {
        f();
        p.set_value();
    });

    future.wait();
}
