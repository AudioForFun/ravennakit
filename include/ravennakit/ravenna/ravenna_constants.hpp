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

#include <cstdint>

namespace rav::ravenna::constants {

constexpr uint16_t k_default_rtp_port = 5004;
constexpr uint16_t k_default_rtcp_port = k_default_rtp_port + 1;
constexpr std::array k_supported_rtp_encoding_names = {"L16", "L24"};

}  // namespace rav::ravenna::constants
