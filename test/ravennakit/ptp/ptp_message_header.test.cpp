/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/ptp/messages/ptp_message_header.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("ptp_message_header") {
    constexpr std::array<const uint8_t, 300> data {
        0xfd,                                            // majorSdoId & messageType
        0x12,                                            // minorVersionPTP & versionPTP
        0x01, 0x2c,                                      // messageLength (300)
        0x01,                                            // domainNumber
        0x22,                                            // minorSdoId
        0x00, 0xff,                                      // flags
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00,  // correctionField
        0x12, 0x34, 0x56, 0x78,                          // message type specific (ignored)
        0x12, 0x34, 0x56, 0x78, 0x00, 0x02, 0x80, 0x00,  // sourcePortIdentity.clockIdentity
        0xab, 0xcd,                                      // sourcePortIdentity.portNumber
        0x11, 0x22,                                      // sequenceId
        0xff,                                            // controlField (ignored)
        0x81,                                            // logMessageInterval
    };

    auto header = rav::ptp_message_header::from_data(rav::buffer_view(data));

    REQUIRE(header.has_value());
    REQUIRE(header->sdo_id == 0xf22);
    REQUIRE(header->message_type == rav::ptp_message_type::management);
    REQUIRE(header->version.major == 0x2);
    REQUIRE(header->version.minor == 0x1);
    REQUIRE(header->message_length == 300);
    REQUIRE(header->domain_number == 1);

    REQUIRE(header->flags.alternate_master_flag == false);
    REQUIRE(header->flags.two_step_flag == false);
    REQUIRE(header->flags.unicast_flag == false);
    REQUIRE(header->flags.profile_specific_1 == false);
    REQUIRE(header->flags.profile_specific_2 == false);
    REQUIRE(header->flags.leap61 == true);
    REQUIRE(header->flags.leap59 == true);
    REQUIRE(header->flags.current_utc_offset_valid == true);
    REQUIRE(header->flags.ptp_timescale == true);
    REQUIRE(header->flags.time_traceable == true);
    REQUIRE(header->flags.frequency_traceable == true);
    REQUIRE(header->flags.synchronization_uncertain == true);

    REQUIRE(header->correction_field == 0x28000);
    REQUIRE(header->source_port_identity.clock_identity.data[0] == 0x12);
    REQUIRE(header->source_port_identity.clock_identity.data[1] == 0x34);
    REQUIRE(header->source_port_identity.clock_identity.data[2] == 0x56);
    REQUIRE(header->source_port_identity.clock_identity.data[3] == 0x78);
    REQUIRE(header->source_port_identity.clock_identity.data[4] == 0x00);
    REQUIRE(header->source_port_identity.clock_identity.data[5] == 0x02);
    REQUIRE(header->source_port_identity.clock_identity.data[6] == 0x80);
    REQUIRE(header->source_port_identity.clock_identity.data[7] == 0x00);
    REQUIRE(header->source_port_identity.port_number == 0xabcd);
    REQUIRE(header->sequence_id == 0x1122);
    REQUIRE(header->logMessageInterval == -127);

    SECTION("Test flags") {}
}

TEST_CASE("ptp_message_header::flag_field") {
    uint8_t octet1 = 0;
    uint8_t octet2 = 0;

    auto flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.alternate_master_flag == false);
    REQUIRE(flags.two_step_flag == false);
    REQUIRE(flags.unicast_flag == false);
    REQUIRE(flags.profile_specific_1 == false);
    REQUIRE(flags.profile_specific_2 == false);
    REQUIRE(flags.leap61 == false);
    REQUIRE(flags.leap59 == false);
    REQUIRE(flags.current_utc_offset_valid == false);
    REQUIRE(flags.ptp_timescale == false);
    REQUIRE(flags.time_traceable == false);
    REQUIRE(flags.frequency_traceable == false);
    REQUIRE(flags.synchronization_uncertain == false);

    octet1 = 1 << 0;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.alternate_master_flag == true);

    octet1 = 1 << 1;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.two_step_flag == true);

    octet1 = 1 << 2;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.unicast_flag == true);

    octet1 = 1 << 5;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.profile_specific_1 == true);

    octet1 = 1 << 6;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.profile_specific_2 == true);

    octet2 = 1 << 0;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.leap61 == true);

    octet2 = 1 << 1;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.leap59 == true);

    octet2 = 1 << 2;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.current_utc_offset_valid == true);

    octet2 = 1 << 3;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.ptp_timescale == true);

    octet2 = 1 << 4;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.time_traceable == true);

    octet2 = 1 << 5;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.frequency_traceable == true);

    octet2 = 1 << 6;
    flags = rav::ptp_message_header::flag_field::from_octets(octet1, octet2);
    REQUIRE(flags.synchronization_uncertain == true);
}
