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

namespace rav {

enum class error {
    invalid_pointer,
    invalid_header_length_length,
    invalid_sender_info_length_length,
    invalid_report_block_length_length,
    invalid_version_version,
    invalid_state,
    resource_failure,
};

/**
 * @param error The error to get a description for.
 * @return A description for given error.
 */
const char* error_to_description(error error);

/**
 * @param error The error to get the name for.
 * @return The name for given error.
 */
const char* error_to_name(error error);

}  // namespace rav
