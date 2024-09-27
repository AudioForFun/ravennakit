/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/sdp/session_description.hpp"

#include <catch2/catch_all.hpp>

#include "ravennakit/core/util.hpp"

TEST_CASE("session_description", "[session_description]") {
    SECTION("Test crlf delimited string") {
        constexpr auto crlf =
            "v=0\r\n"
            "o=- 13 0 IN IP4 192.168.15.52\r\n"
            "s=Anubis_610120_13\r\n";
        auto result = rav::session_description::parse_new(crlf);
        REQUIRE(result.is_ok());
        REQUIRE(result.get_ok().version() == 0);
    }

    SECTION("Test n delimited string") {
        constexpr auto n =
            "v=0\n"
            "o=- 13 0 IN IP4 192.168.15.52\n"
            "s=Anubis_610120_13\n";
        auto result = rav::session_description::parse_new(n);
        REQUIRE(result.is_ok());
        REQUIRE(result.get_ok().version() == 0);
    }
}

TEST_CASE("session_description | description from anubis", "[session_description]") {
    constexpr auto k_anubis_sdp =
        "v=0\r\n"
        "o=- 13 0 IN IP4 192.168.15.52\r\n"
        "s=Anubis_610120_13\r\n"
        "c=IN IP4 239.1.15.52/15\r\n"
        "t=0 0\r\n"
        "a=clock-domain:PTPv2 0\r\n"
        "a=ts-refclk:ptp=IEEE1588-2008:00-1D-C1-FF-FE-51-9E-F7:0\r\n"
        "a=mediaclk:direct=0\r\n"
        "m=audio 5004 RTP/AVP 98\r\n"
        "c=IN IP4 239.1.15.52/15\r\n"
        "a=rtpmap:98 L16/48000/2\r\n"
        "a=source-filter: incl IN IP4 239.1.15.52 192.168.15.52\r\n"
        "a=clock-domain:PTPv2 0\r\n"
        "a=sync-time:0\r\n"
        "a=framecount:48\r\n"
        "a=palign:0\r\n"
        "a=ptime:1\r\n"
        "a=ts-refclk:ptp=IEEE1588-2008:00-1D-C1-FF-FE-51-9E-F7:0\r\n"
        "a=mediaclk:direct=0\r\n"
        "a=recvonly\r\n"
        "a=midi-pre2:50040 0,0;0,1\r\n";

    auto result = rav::session_description::parse_new(k_anubis_sdp);
    REQUIRE(result.is_ok());

    SECTION("Parse a description from an Anubis") {
        REQUIRE(result.get_ok().version() == 0);
    }

    SECTION("Test version") {
        constexpr auto sdp =
            "v=1\r\n"
            "o=- 13 0 IN IP4 192.168.15.52\r\n"
            "s=Anubis_610120_13\r\n";
        REQUIRE(rav::session_description::parse_new(sdp).is_err());
    }

    SECTION("Test origin") {
        const auto& origin = result.get_ok().origin();
        REQUIRE(origin.username == "-");
        REQUIRE(origin.session_id == "13");
        REQUIRE(origin.session_version == 0);
        REQUIRE(origin.network_type == rav::session_description::netw_type::internet);
        REQUIRE(origin.address_type == rav::session_description::addr_type::ipv4);
        REQUIRE(origin.unicast_address == "192.168.15.52");
    }

    SECTION("Test connection") {
        const auto& connection = result.get_ok().connection_info();
        REQUIRE(connection.has_value());
        REQUIRE(connection->network_type == rav::session_description::netw_type::internet);
        REQUIRE(connection->address_type == rav::session_description::addr_type::ipv4);
        REQUIRE(connection->address == "239.1.15.52");
    }

    SECTION("Test session name") {
        REQUIRE(result.get_ok().session_name() == "Anubis_610120_13");
    }

    SECTION("Test time") {
        auto time = result.get_ok().time_active();
        REQUIRE(time.start_time == 0);
        REQUIRE(time.stop_time == 0);
    }

    SECTION("Test media") {
        const auto& descriptions = result.get_ok().media_descriptions();
        REQUIRE(descriptions.size() == 1);

        const auto& media = descriptions[0];
        REQUIRE(media.media_type() == "audio");
        REQUIRE(media.port() == 5004);
        REQUIRE(media.number_of_ports() == 1);
        REQUIRE(media.protocol() == "RTP/AVP");
        REQUIRE(media.formats().size() == 1);

        auto format = media.formats()[0];
        REQUIRE(format.payload_type == 98);
        REQUIRE(format.encoding_name == "L16");
        REQUIRE(format.clock_rate == 48000);
        REQUIRE(format.num_channels == 2);
        REQUIRE(media.connection_infos().size() == 1);

        const auto& conn = media.connection_infos().back();
        REQUIRE(conn.network_type == rav::session_description::netw_type::internet);
        REQUIRE(conn.address_type == rav::session_description::addr_type::ipv4);
        REQUIRE(conn.address == "239.1.15.52");
        REQUIRE(conn.ttl.has_value() == true);
        REQUIRE(*conn.ttl == 15);
        REQUIRE(static_cast<int64_t>(media.ptime().value()) == 1);

        SECTION("Test refclk on media") {
            const auto& refclk = media.reference_clock();
            REQUIRE(refclk.has_value());
            REQUIRE(refclk->source() == rav::sdp::reference_clock::clock_source::ptp);
            REQUIRE(refclk->ptp_version() == rav::sdp::reference_clock::ptp_ver::IEEE_1588_2008);
            REQUIRE(refclk->gmid() == "00-1D-C1-FF-FE-51-9E-F7");
            REQUIRE(refclk->domain() == 0);
        }

        SECTION("Test mediaclk on media") {
            const auto& media_clock = media.media_clock().value();
            REQUIRE(media_clock.mode() == rav::sdp::media_clock::clock_mode::direct);
            REQUIRE(media_clock.offset().value() == 0);
            REQUIRE_FALSE(media_clock.rate().has_value());
        }
    }

    SECTION("Media direction") {
        REQUIRE(result.get_ok().direction() == rav::session_description::media_direction::sendrecv);
    }

    SECTION("Test refclk on session") {
        const auto& refclk = result.get_ok().reference_clock();
        REQUIRE(refclk.has_value());
        REQUIRE(refclk->source() == rav::sdp::reference_clock::clock_source::ptp);
        REQUIRE(refclk->ptp_version() == rav::sdp::reference_clock::ptp_ver::IEEE_1588_2008);
        REQUIRE(refclk->gmid() == "00-1D-C1-FF-FE-51-9E-F7");
        REQUIRE(refclk->domain() == 0);
    }

    SECTION("Test mediaclk attribute") {
        auto media_clock = result.move_ok().media_clock().value();
        REQUIRE(media_clock.mode() == rav::sdp::media_clock::clock_mode::direct);
        REQUIRE(media_clock.offset().value() == 0);
        REQUIRE_FALSE(media_clock.rate().has_value());
    }
}

TEST_CASE("session_description | origin_field", "[session_description]") {
    SECTION("Parse origin line") {
        auto result = rav::session_description::origin_field::parse_new("o=- 13 0 IN IP4 192.168.15.52");
        REQUIRE(result.is_ok());
        auto origin = result.move_ok();
        REQUIRE(origin.username == "-");
        REQUIRE(origin.session_id == "13");
        REQUIRE(origin.session_version == 0);
        REQUIRE(origin.network_type == rav::session_description::netw_type::internet);
        REQUIRE(origin.address_type == rav::session_description::addr_type::ipv4);
        REQUIRE(origin.unicast_address == "192.168.15.52");
    }
}

TEST_CASE("session_description | connection_info_field", "[session_description]") {
    SECTION("Parse connection line") {
        auto result = rav::session_description::connection_info_field::parse_new("c=IN IP4 239.1.15.52");
        REQUIRE(result.is_ok());
        auto connection = result.move_ok();
        REQUIRE(connection.network_type == rav::session_description::netw_type::internet);
        REQUIRE(connection.address_type == rav::session_description::addr_type::ipv4);
        REQUIRE(connection.address == "239.1.15.52");
        REQUIRE(connection.ttl.has_value() == false);
        REQUIRE(connection.number_of_addresses.has_value() == false);
    }

    SECTION("Parse connection line with ttl") {
        auto result = rav::session_description::connection_info_field::parse_new("c=IN IP4 239.1.15.52/15");
        REQUIRE(result.is_ok());
        auto connection = result.move_ok();
        REQUIRE(connection.network_type == rav::session_description::netw_type::internet);
        REQUIRE(connection.address_type == rav::session_description::addr_type::ipv4);
        REQUIRE(connection.address == "239.1.15.52");
        REQUIRE(connection.ttl.has_value());
        REQUIRE(*connection.ttl == 15);
        REQUIRE(connection.number_of_addresses.has_value() == false);
    }

    SECTION("Parse connection line with ttl and number of addresses") {
        auto result = rav::session_description::connection_info_field::parse_new("c=IN IP4 239.1.15.52/15/3");
        REQUIRE(result.is_ok());
        auto connection = result.move_ok();
        REQUIRE(connection.network_type == rav::session_description::netw_type::internet);
        REQUIRE(connection.address_type == rav::session_description::addr_type::ipv4);
        REQUIRE(connection.address == "239.1.15.52");
        REQUIRE(connection.ttl.has_value());
        REQUIRE(*connection.ttl == 15);
        REQUIRE(connection.number_of_addresses.has_value());
        REQUIRE(*connection.number_of_addresses == 3);
    }

    SECTION("Parse ipv6 connection line with number of addresses") {
        auto result = rav::session_description::connection_info_field::parse_new("c=IN IP6 ff00::db8:0:101/3");
        REQUIRE(result.is_ok());
        auto connection = result.move_ok();
        REQUIRE(connection.network_type == rav::session_description::netw_type::internet);
        REQUIRE(connection.address_type == rav::session_description::addr_type::ipv6);
        REQUIRE(connection.address == "ff00::db8:0:101");
        REQUIRE(connection.ttl.has_value() == false);
        REQUIRE(connection.number_of_addresses.has_value());
        REQUIRE(*connection.number_of_addresses == 3);
    }

    SECTION("Parse ipv6 connection line with ttl and number of addresses (which should fail)") {
        auto result = rav::session_description::connection_info_field::parse_new("c=IN IP6 ff00::db8:0:101/127/3");
        REQUIRE(result.is_err());
    }
}

TEST_CASE("session_description | time_field", "[session_description]") {
    SECTION("Test time field") {
        auto result = rav::session_description::time_active_field::parse_new("t=123456789 987654321");
        REQUIRE(result.is_ok());
        const auto time = result.move_ok();
        REQUIRE(time.start_time == 123456789);
        REQUIRE(time.stop_time == 987654321);
    }

    SECTION("Test invalid time field") {
        auto result = rav::session_description::time_active_field::parse_new("t=123456789 ");
        REQUIRE(result.is_err());
    }

    SECTION("Test invalid time field") {
        auto result = rav::session_description::time_active_field::parse_new("t=");
        REQUIRE(result.is_err());
    }
}

TEST_CASE("session_description | media_description", "[session_description]") {
    SECTION("Test media field") {
        auto result = rav::session_description::media_description::parse_new("m=audio 5004 RTP/AVP 98");
        REQUIRE(result.is_ok());
        const auto media = result.move_ok();
        REQUIRE(media.media_type() == "audio");
        REQUIRE(media.port() == 5004);
        REQUIRE(media.number_of_ports() == 1);
        REQUIRE(media.protocol() == "RTP/AVP");
        REQUIRE(media.formats().size() == 1);
        auto format = media.formats()[0];
        REQUIRE(format.payload_type == 98);
    }

    SECTION("Test media field with multiple formats") {
        auto result = rav::session_description::media_description::parse_new("m=audio 5004/2 RTP/AVP 98 99 100");
        REQUIRE(result.is_ok());

        auto media = result.move_ok();
        REQUIRE(media.media_type() == "audio");
        REQUIRE(media.port() == 5004);
        REQUIRE(media.number_of_ports() == 2);
        REQUIRE(media.protocol() == "RTP/AVP");
        REQUIRE(media.formats().size() == 3);

        const auto& format1 = media.formats()[0];
        REQUIRE(format1.payload_type == 98);
        REQUIRE(format1.encoding_name.empty());
        REQUIRE(format1.clock_rate == 0);
        REQUIRE(format1.num_channels == 0);

        const auto& format2 = media.formats()[1];
        REQUIRE(format2.payload_type == 99);
        REQUIRE(format2.encoding_name.empty());
        REQUIRE(format2.clock_rate == 0);
        REQUIRE(format2.num_channels == 0);

        const auto& format3 = media.formats()[2];
        REQUIRE(format3.payload_type == 100);
        REQUIRE(format3.encoding_name.empty());
        REQUIRE(format3.clock_rate == 0);
        REQUIRE(format3.num_channels == 0);

        media.parse_attribute("a=rtpmap:98 L16/48000/2");
        REQUIRE(format1.payload_type == 98);
        REQUIRE(format1.encoding_name == "L16");
        REQUIRE(format1.clock_rate == 48000);
        REQUIRE(format1.num_channels == 2);

        media.parse_attribute("a=rtpmap:99 L16/96000/2");
        REQUIRE(format2.payload_type == 99);
        REQUIRE(format2.encoding_name == "L16");
        REQUIRE(format2.clock_rate == 96000);
        REQUIRE(format2.num_channels == 2);

        media.parse_attribute("a=rtpmap:100 L24/44100");
        REQUIRE(format3.payload_type == 100);
        REQUIRE(format3.encoding_name == "L24");
        REQUIRE(format3.clock_rate == 44100);
        REQUIRE(format3.num_channels == 1);
    }

    SECTION("Test media field with multiple formats and an invalid one") {
        auto result = rav::session_description::media_description::parse_new("m=audio 5004/2 RTP/AVP 98 99 100 128");
        REQUIRE(result.is_err());
    }

    SECTION("Test media field direction") {
        auto result = rav::session_description::media_description::parse_new("m=audio 5004/2 RTP/AVP 98 99 100");
        REQUIRE(result.is_ok());
        auto media = result.move_ok();
        REQUIRE_FALSE(media.direction().has_value());
        auto result2 = media.parse_attribute("a=recvonly");
        REQUIRE(result2.is_ok());
        REQUIRE(media.direction().has_value());
        REQUIRE(*media.direction() == rav::session_description::media_direction::recvonly);
    }

    SECTION("Test maxptime attribute") {
        auto result = rav::session_description::media_description::parse_new("m=audio 5004/2 RTP/AVP 98 99 100");
        REQUIRE(result.is_ok());
        auto media = result.move_ok();
        REQUIRE_FALSE(media.max_ptime().has_value());
        media.parse_attribute("a=maxptime:60.5");
        REQUIRE(media.max_ptime().has_value());
        REQUIRE(rav::util::is_within(*media.max_ptime(), 60.5, 0.0001));
    }

    SECTION("Test mediaclk attribute") {
        auto result = rav::session_description::media_description::parse_new("m=audio 5004/2 RTP/AVP 98 99 100");
        REQUIRE(result.is_ok());
        auto media = result.move_ok();
        REQUIRE_FALSE(media.media_clock().has_value());
        media.parse_attribute("a=mediaclk:direct=5 rate=48000/1");
        REQUIRE(media.media_clock().has_value());
        const auto& clock = media.media_clock().value();
        REQUIRE(clock.mode() == rav::sdp::media_clock::clock_mode::direct);
        REQUIRE(clock.offset().value() == 5);
        REQUIRE(clock.rate().has_value());
        REQUIRE(clock.rate().value().numerator == 48000);
        REQUIRE(clock.rate().value().denominator == 1);
    }
}
