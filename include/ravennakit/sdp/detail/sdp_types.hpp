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
#include <string>

namespace rav::sdp {

enum class netw_type { undefined, internet };
enum class addr_type { undefined, ipv4, ipv6, both };
enum class media_direction { sendrecv, sendonly, recvonly, inactive };
enum class filter_mode { undefined, exclude, include };

std::string to_string(const netw_type& type);
std::string to_string(const addr_type& type);
std::string to_string(const media_direction& direction);
std::string to_string(const filter_mode& filter_mode);

}
