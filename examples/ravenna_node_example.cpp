/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/log.hpp"

int main(int const argc, char* argv[]) {
#if RAV_ENABLE_SPDLOG
    spdlog::set_level(spdlog::level::trace);
#endif
}
