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

#include "ravennakit/core/string_parser.hpp"

#include <string>
#include <tl/expected.hpp>

namespace rav {

class rtsp_request {
public:
    struct header {
        std::string name;
        std::string value;
    };

    std::string method;
    std::string uri;
    int rtsp_version_major{};
    int rtsp_version_minor{};
    std::vector<header> headers;
    std::string data;

    /**
     * Finds a header by name and returns its value.
     * @param name The name of the header.
     * @returns The value of the header if found, otherwise nullptr.
     */
    [[nodiscard]] const std::string* get_header_value(const std::string& name) const {
        for (const auto& header : headers) {
            if (header.name == name) {
                return &header.value;
            }
        }
        return nullptr;
    }

    /**
     * @returns Tries to find the Content-Length header and returns its value as integer.
     */
    [[nodiscard]] std::optional<long> get_content_length() const {
        if (const std::string* content_length = get_header_value("Content-Length"); content_length) {
            return rav::ston<long>(*content_length);
        }
        return std::nullopt;
    }

    /**
     * Resets the request to its initial state.
     */
    void reset() {
        method.clear();
        uri.clear();
        rtsp_version_major = {};
        rtsp_version_minor = {};
        headers.clear();
        data.clear();
    }
};

}  // namespace rav
