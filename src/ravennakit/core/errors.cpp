/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/core/errors.hpp"

const char* rav::error_to_description(const error error) {
    switch (error) {
        case error::invalid_pointer:
            return "Invalid pointer";
        case error::invalid_header_length_length:
            return "Invalid header length length";
        case error::invalid_sender_info_length_length:
            return "Invalid sender info length length";
        case error::invalid_report_block_length_length:
            return "Invalid report block length length";
        case error::invalid_version_version:
            return "Invalid version version";
        case error::invalid_state:
            return "Invalid state";
        case error::resource_failure:
            return "Allocation failure";
        case error::multicast_membership_failure:
            return "Multicast membership failure";
        default:
            return "Unknown error";
    }
}

const char* rav::error_to_name(const error error) {
    switch (error) {
        case error::invalid_pointer:
            return "invalid_pointer";
        case error::invalid_header_length_length:
            return "invalid_header_length_length";
        case error::invalid_sender_info_length_length:
            return "invalid_sender_info_length_length";
        case error::invalid_report_block_length_length:
            return "invalid_report_block_length_length";
        case error::invalid_version_version:
            return "invalid_version_version";
        case error::invalid_state:
            return "invalid_state";
        case error::resource_failure:
            return "allocation_failure";
        case error::multicast_membership_failure:
            return "multicast_membership_failure";
        default:
            return "unknown_error";
    }
}
