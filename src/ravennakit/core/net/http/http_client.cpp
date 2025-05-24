/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/core/net/http/http_client.hpp"

#include <utility>

#include "ravennakit/core/assert.hpp"

rav::HttpClient::HttpClient(boost::asio::io_context& io_context, const boost::urls::url& url) :
    io_context_(io_context) {
    host_ = url.host();
    service_ = url.port();
    target_ = url.path();
}

rav::HttpClient::HttpClient(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint) :
    io_context_(io_context) {
    host_ = endpoint.address().to_string();
    service_ = std::to_string(endpoint.port());
}

rav::HttpClient::HttpClient(
    boost::asio::io_context& io_context, const boost::asio::ip::address& address, const uint16_t port
) :
    io_context_(io_context) {
    host_ = address.to_string();
    service_ = std::to_string(port);
}

void rav::HttpClient::set_host(const boost::urls::url& url) {
    host_ = url.host();
    service_ = url.port();
    target_ = url.path();
}

void rav::HttpClient::set_host(const std::string_view url) {
    const boost::urls::url parsed_url(url);
    set_host(parsed_url);
}

void rav::HttpClient::set_host(
    const std::string_view host, const std::string_view service, const std::string_view target
) {
    host_ = host;
    service_ = service;
    target_ = target;
}

rav::HttpClient::HttpClient(boost::asio::io_context& io_context) : io_context_(io_context) {}

rav::HttpClient::HttpClient(boost::asio::io_context& io_context, const std::string_view url) : io_context_(io_context) {
    const boost::urls::url parsed_url(url);
    host_ = parsed_url.host();
    service_ = parsed_url.port();
    target_ = parsed_url.path();
}

void rav::HttpClient::get_async(const std::string_view target, CallbackType callback) const {
    request_async(io_context_, http::verb::get, host_, service_, target, {}, {}, std::move(callback));
}

void rav::HttpClient::post_async(std::string body, CallbackType callback, const std::string_view content_type) const {
    request_async(
        io_context_, http::verb::post, host_, service_, target_, std::move(body), content_type, std::move(callback)
    );
}

void rav::HttpClient::post_async(
    const std::string_view target, std::string body, CallbackType callback, const std::string_view content_type
) const {
    request_async(
        io_context_, http::verb::post, host_, service_, target, std::move(body), content_type, std::move(callback)
    );
}

void rav::HttpClient::get_async(CallbackType callback) const {
    request_async(io_context_, http::verb::get, host_, service_, target_, {}, {}, std::move(callback));
}

void rav::HttpClient::request_async(
    boost::asio::io_context& io_context, const http::verb method, const std::string_view host,
    const std::string_view service, const std::string_view target, std::string body,
    const std::string_view content_type, CallbackType callback
) {
    RAV_ASSERT(!host.empty(), "Host cannot be empty");

    auto request = http::request<http::string_body>(method, target.empty() ? "/" : target, 11);
    request.set(http::field::host, host);
    request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.set(http::field::accept, "*/*");

    if (!body.empty()) {
        request.set(http::field::content_type, content_type);
        request.body() = std::move(body);
        request.prepare_payload();
    }

    const auto session = std::make_shared<Session>(io_context);
    session->request(std::move(request), host, service.empty() ? k_default_port : service, std::move(callback));
}

rav::HttpClient::Session::Session(boost::asio::io_context& io_context) : resolver_(io_context), stream_(io_context) {}

void rav::HttpClient::Session::request(
    http::request<http::string_body> request, const std::string_view host, const std::string_view port,
    CallbackType callback
) {
    request_ = std::move(request);
    callback_ = std::move(callback);
    resolver_.async_resolve(
        host, port.empty() ? k_default_port : port,
        boost::beast::bind_front_handler(&Session::on_resolve, shared_from_this())
    );
}

void rav::HttpClient::Session::on_resolve(
    const boost::beast::error_code& ec, const tcp::resolver::results_type& results
) {
    if (ec) {
        callback_(ec);
    }

    // Set a timeout on the operation
    stream_.expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    stream_.async_connect(results, boost::beast::bind_front_handler(&Session::on_connect, shared_from_this()));
}

void rav::HttpClient::Session::
    on_connect(const boost::beast::error_code& ec, const tcp::resolver::results_type::endpoint_type&) {
    if (ec) {
        return callback_(ec);
    }

    // Set a timeout on the operation
    stream_.expires_after(std::chrono::seconds(30));

    // Send the HTTP request to the remote host
    http::async_write(stream_, request_, boost::beast::bind_front_handler(&Session::on_write, shared_from_this()));
}

void rav::HttpClient::Session::on_write(const boost::beast::error_code& ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        return callback_(ec);
    }

    // Receive the HTTP response
    http::async_read(
        stream_, buffer_, response_, boost::beast::bind_front_handler(&Session::on_read, shared_from_this())
    );
}

void rav::HttpClient::Session::on_read(boost::beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        return callback_(ec);
    }

    // Gracefully close the socket
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes so don't bother reporting it.
    if (ec && ec != boost::beast::errc::not_connected) {
        return callback_(ec);
    }

    callback_(response_);
}
