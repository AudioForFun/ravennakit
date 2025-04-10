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

#ifndef RAV_ENABLE_JSON
    #define RAV_ENABLE_JSON
#endif

#ifdef RAV_ENABLE_JSON
    #define RAV_HAS_NLOHMANN_JSON 1
    #include "expected.hpp"
    #include <nlohmann/json.hpp>
#else
    #define RAV_HAS_NLOHMANN_JSON 0
#endif
