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

#include <boost/beast.hpp>
#include <fmt/ostream.h>

// Make beast response printable with fmt
template<typename Body, typename Allocator>
struct fmt::formatter<boost::beast::http::response<Body, boost::beast::http::basic_fields<Allocator>>>:
    ostream_formatter {};

// Make beast request printable with fmt
template<typename Body, typename Allocator>
struct fmt::formatter<boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>>:
    ostream_formatter {};
