/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/rtsp/rtsp_client.hpp"

#include "ravennakit/core/log.hpp"
#include "ravennakit/rtsp/rtsp_request.hpp"

rav::rtsp_client::rtsp_client(asio::io_context& io_context) : socket_(asio::make_strand(io_context)) {
    parser_.on<rtsp_response>([](const rtsp_response& response, rtsp_parser&) {
        RAV_INFO("{}", response.to_debug_string());
    });
    parser_.on<rtsp_request>([](const rtsp_request& request, rtsp_parser&) {
        RAV_INFO("{}", request.to_debug_string());
    });
}

void rav::rtsp_client::connect(const asio::ip::tcp::endpoint& endpoint) {
    asio::post(socket_.get_executor(), [this, endpoint]() {
        async_connect(endpoint);
    });
}

void rav::rtsp_client::send_describe_request(const std::string& uri) {
    if (starts_with(uri, "/")) {
        RAV_THROW_EXCEPTION("URI must not start with a slash");
    }

    asio::post(socket_.get_executor(), [this, uri] {
        rtsp_request request;
        request.method = "DESCRIBE";
        request.uri = fmt::format("rtsp://{}/{}", socket_.remote_endpoint().address().to_string(), uri);
        request.rtsp_version_major = 1;
        request.rtsp_version_minor = 0;
        request.headers["CSeq"] = "15";
        request.headers["Accept"] = "application/sdp";
        const auto encoded = request.encode();
        RAV_TRACE("Sending request: {}", request.to_debug_string());
        const bool should_write = output_stream_.empty();
        output_stream_.write(encoded);
        if (should_write) {
            async_write();
        }
    });
}

void rav::rtsp_client::async_connect(const asio::ip::tcp::endpoint& endpoint) {
    socket_.async_connect(endpoint, [this](const asio::error_code ec) {
        if (ec) {
            RAV_ERROR("Connect error: {}", ec.message());
            return;
        }
        RAV_INFO("Connected to {}", socket_.remote_endpoint().address().to_string());
        async_write();  // Schedule a write in case there is data to send
        async_read_some();
        emit<rtsp::connect_event>(rtsp::connect_event {});
    });
}

void rav::rtsp_client::async_write() {
    if (output_stream_.empty()) {
        return;
    }
    asio::async_write(
        socket_, asio::buffer(output_stream_.data()),
        [this](const asio::error_code ec, std::size_t length) {
            if (ec) {
                RAV_ERROR("Write error: {}", ec.message());
                return;
            }

            RAV_INFO("Wrote {} bytes", length);

            output_stream_.consume(length);
            if (!output_stream_.empty()) {
                async_write();  // Schedule another write
            } else {
                output_stream_.reset();
            }
        }
    );
}

void rav::rtsp_client::async_read_some() {
    auto buffer = input_stream_.prepare(512);
    socket_.async_read_some(
        asio::buffer(buffer.data(), buffer.size_bytes()),
        [this](const asio::error_code ec, const std::size_t length) mutable {
            if (ec) {
                RAV_ERROR("Read error: {}", ec.message());
                // TODO: Close the connection?
                return;
            }

            input_stream_.commit(length);

            auto result = parser_.parse(input_stream_);
            if (!(result == rtsp_parser::result::good || result == rtsp_parser::result::indeterminate)) {
                RAV_ERROR("Parsing error: {}", static_cast<int>(result));
                return;
            }

            if (input_stream_.empty()) {
                input_stream_.reset();
            }

            async_read_some();
        }
    );
}
