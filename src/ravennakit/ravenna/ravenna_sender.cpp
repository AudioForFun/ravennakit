/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#include "ravennakit/ravenna/ravenna_sender.hpp"

#include "ravennakit/rtp/rtp_packet_view.hpp"

#if RAV_WINDOWS
    #include <timeapi.h>
#endif

rav::RavennaSender::RavennaSender(
    asio::io_context& io_context, dnssd::Advertiser& advertiser, rtsp::Server& rtsp_server, ptp::Instance& ptp_instance,
    rtp::Sender& rtp_sender, const Id id
) :
    advertiser_(advertiser),
    rtsp_server_(rtsp_server),
    ptp_instance_(ptp_instance),
    rtp_sender_(rtp_sender),
    id_(id),
    timer_(io_context) {
    ptp_parent_changed_slot_ =
        ptp_instance.on_parent_changed.subscribe([this](const ptp::Instance::ParentChangedEvent& event) {
            if (grandmaster_identity_ == event.parent.grandmaster_identity) {
                return;
            }
            grandmaster_identity_ = event.parent.grandmaster_identity;
            send_announce();
        });

#if RAV_WINDOWS
    timeBeginPeriod(1);
#endif
}

rav::RavennaSender::~RavennaSender() {
#if RAV_WINDOWS
    timeEndPeriod(1);
#endif

    timer_.cancel();

    if (advertisement_id_.is_valid()) {
        advertiser_.unregister_service(advertisement_id_);
    }

    rtsp_server_.unregister_handler(this);
}

rav::Id rav::RavennaSender::get_id() const {
    return id_;
}

void rav::RavennaSender::update_configuration(const Configuration& configuration) {
    bool register_rtsp = false;
    bool advertise_session = false;
    if (configuration.session_name.has_value()) {
        if (configuration.session_name != configuration_.session_name) {
            if (!rtsp_path_by_name_.empty()) {
                rtsp_server_.unregister_handler(this);
            }
            if (advertisement_id_.is_valid()) {
                advertiser_.unregister_service(advertisement_id_);
            }

            configuration_.session_name = configuration.session_name;

            if (configuration_.session_name.has_value() && !configuration_.session_name->empty()) {
                register_rtsp = true;
                advertise_session = true;
            }
        }
    }

    if (!configuration_.destination_address.has_value()) {
        // Construct a multicast address from the interface address
        const auto interface_address_bytes = rtp_sender_.get_interface_address().to_bytes();
        configuration_.destination_address = asio::ip::address_v4(
            {239, interface_address_bytes[2], interface_address_bytes[3], static_cast<uint8_t>(id_.value() % 0xff)}
        );
    }

    if (!configuration_.enabled.has_value() || configuration_.enabled == false) {
        return; // Not active.
    }

    if (register_rtsp) {
        RAV_ASSERT(configuration_.session_name.has_value(), "Session name not set");
        RAV_ASSERT(!configuration_.session_name->empty(), "Session name not set");

        // Register handlers for the paths
        rtsp_path_by_name_ = fmt::format("/by-name/{}", *configuration_.session_name);
        rtsp_path_by_id_ = fmt::format("/by-id/{}", id_.to_string());

        rtsp_server_.register_handler(rtsp_path_by_name_, this);
        rtsp_server_.register_handler(rtsp_path_by_id_, this);
    }

    if (advertise_session) {
        RAV_ASSERT(configuration_.session_name.has_value(), "Session name not set");
        RAV_ASSERT(!configuration_.session_name->empty(), "Session name not set");

        advertisement_id_ = advertiser_.register_service(
            "_rtsp._tcp,_ravenna_session", configuration_.session_name->c_str(), nullptr, rtsp_server_.port(), {},
            false, false
        );
    }
}

const rav::RavennaSender::Configuration& rav::RavennaSender::get_configuration() const {
    return configuration_;
}

bool rav::RavennaSender::set_audio_format(const AudioFormat format) {
    const auto sdp_format = sdp::Format::from_audio_format(format);
    if (!sdp_format) {
        RAV_ERROR("Failed to convert audio format to SDP format");
        return false;
    }

    if (format == audio_format_) {
        return true;  // Nothing to be done here
    }

    audio_format_ = format;
    sdp_format_ = *sdp_format;

    rtp_packet_.payload_type(sdp_format_.payload_type);
    // TODO: Implement proper SSRC generation
    rtp_packet_.ssrc(static_cast<uint32_t>(Random().get_random_int(0, std::numeric_limits<int>::max())));

    return true;
}

void rav::RavennaSender::set_packet_time(const aes67::PacketTime packet_time) {
    if (ptime_ == packet_time) {
        return;
    }
    ptime_ = packet_time;
    resize_internal_buffers();
}

float rav::RavennaSender::get_signaled_ptime() const {
    return ptime_.signaled_ptime(audio_format_.sample_rate);
}

void rav::RavennaSender::start(const uint32_t timestamp_samples) {
    if (running_) {
        return;
    }
    running_ = true;
    rtp_packet_.timestamp(timestamp_samples);
    resize_internal_buffers();
    start_timer();
    RAV_TRACE("Start transmitting at timestamp: {}", timestamp_samples);
}

void rav::RavennaSender::start(const ptp::Timestamp timestamp) {
    start(static_cast<uint32_t>(timestamp.to_samples(audio_format_.sample_rate)));
}

void rav::RavennaSender::stop() {
    if (!running_) {
        return;
    }
    running_ = false;
}

bool rav::RavennaSender::subscribe(Subscriber* subscriber) {
    return subscribers_.add(subscriber);
}

bool rav::RavennaSender::unsubscribe(Subscriber* subscriber) {
    if (subscribers_.remove(subscriber)) {
        subscriber->ravenna_sender_configuration_updated(id_, configuration_);
        return true;
    }
    return false;
}

bool rav::RavennaSender::is_running() const {
    return running_;
}

uint32_t rav::RavennaSender::get_framecount() const {
    return ptime_.framecount(audio_format_.sample_rate);
}

void rav::RavennaSender::on_data_requested(OnDataRequestedHandler handler) {
    on_data_requested_handler_ = std::move(handler);
}

void rav::RavennaSender::on_request(rtsp::Connection::RequestEvent event) const {
    const auto sdp = build_sdp();  // Should the SDP be cached and updated on changes?
    RAV_TRACE("SDP:\n{}", sdp.to_string("\n").value());
    const auto encoded = sdp.to_string();
    if (!encoded) {
        RAV_ERROR("Failed to encode SDP");
        return;
    }
    auto response = rtsp::Response(200, "OK", *encoded);
    if (const auto* cseq = event.rtsp_request.rtsp_headers.get("cseq")) {
        response.rtsp_headers.set(*cseq);
    }
    response.rtsp_headers.set("content-type", "application/sdp");
    event.rtsp_connection.async_send_response(response);
}

void rav::RavennaSender::send_announce() const {
    auto sdp = build_sdp().to_string();
    if (!sdp) {
        RAV_ERROR("Failed to encode SDP: {}", sdp.error());
        return;
    }

    auto interface_address_string = rtp_sender_.get_interface_address().to_string();

    rtsp::Request request;
    request.method = "ANNOUNCE";
    request.rtsp_headers.set("content-type", "application/sdp");
    request.data = std::move(sdp.value());
    request.uri =
        Uri::encode("rtsp://", interface_address_string + ":" + std::to_string(rtsp_server_.port()), rtsp_path_by_name_);
    rtsp_server_.send_request(rtsp_path_by_name_, request);
    request.uri =
        Uri::encode("rtsp://", interface_address_string + ":" + std::to_string(rtsp_server_.port()), rtsp_path_by_id_);
    rtsp_server_.send_request(rtsp_path_by_name_, request);
}

rav::sdp::SessionDescription rav::RavennaSender::build_sdp() const {
    if (!configuration_.destination_address.has_value()) {
        RAV_ERROR("Destination address not set");
        return {};
    }

    if (!configuration_.session_name || configuration_.session_name->empty()) {
        RAV_ERROR("Session name not set");
        return {};
    }

    // Connection info
    const sdp::ConnectionInfoField connection_info {
        sdp::NetwType::internet, sdp::AddrType::ipv4, configuration_.destination_address->to_string(), 15, {}
    };

    // Source filter
    sdp::SourceFilter filter(
        sdp::FilterMode::include, sdp::NetwType::internet, sdp::AddrType::ipv4,
        configuration_.destination_address->to_string(), {rtp_sender_.get_interface_address().to_string()}
    );

    // Reference clock
    const sdp::ReferenceClock ref_clock {
        sdp::ReferenceClock::ClockSource::ptp, sdp::ReferenceClock::PtpVersion::IEEE_1588_2008,
        grandmaster_identity_.to_string(), clock_domain_
    };

    // Media clock
    // ST 2110-30:2017 defines a constraint to use a zero offset exclusively.
    sdp::MediaClockSource media_clk {sdp::MediaClockSource::ClockMode::direct, 0, {}};

    sdp::RavennaClockDomain clock_domain {sdp::RavennaClockDomain::SyncSource::ptp_v2, clock_domain_};

    sdp::MediaDescription media;
    media.add_connection_info(connection_info);
    media.set_media_type("audio");
    media.set_port(5004);
    media.set_protocol("RTP/AVP");
    media.add_format(sdp_format_);
    media.add_source_filter(filter);
    media.set_clock_domain(clock_domain);
    media.set_sync_time(0);
    media.set_ref_clock(ref_clock);
    media.set_direction(sdp::MediaDirection::recvonly);
    media.set_ptime(get_signaled_ptime());
    media.set_framecount(get_framecount());

    sdp::SessionDescription sdp;

    // Origin
    const sdp::OriginField origin {
        "-",
        id_.to_string(),
        0,
        sdp::NetwType::internet,
        sdp::AddrType::ipv4,
        rtp_sender_.get_interface_address().to_string(),
    };
    sdp.set_origin(origin);

    // Session name
    sdp.set_session_name(*configuration_.session_name);
    sdp.set_connection_info(connection_info);
    sdp.set_clock_domain(clock_domain);
    sdp.set_ref_clock(ref_clock);
    sdp.set_media_clock(media_clk);
    sdp.add_media_description(media);

    return sdp;
}

void rav::RavennaSender::start_timer() {
    TRACY_ZONE_SCOPED;

#if RAV_WINDOWS
    // A dirty hack to get the precision somewhat right. This is only temporary since we have to come up with a much
    // tighter solution.
    auto expires_after = std::chrono::microseconds(1);
#else
    // A tenth of the packet time
    auto expires_after = std::chrono::microseconds(static_cast<int64_t>(get_signaled_ptime() * 1'000 / 10));
#endif

    timer_.expires_after(expires_after);
    timer_.async_wait([this](const asio::error_code ec) {
        if (ec == asio::error::operation_aborted) {
            return;
        }
        if (ec) {
            RAV_ERROR("Timer error: {}", ec.message());
            return;
        }
        send_data();
        start_timer();
    });
}

void rav::RavennaSender::send_data() {
    TRACY_ZONE_SCOPED;

    if (!running_) {
        return;
    }

    if (!on_data_requested_handler_) {
        RAV_WARNING("No data handler installed");
        return;
    }

    if (!configuration_.destination_address.has_value()) {
        RAV_ERROR("Destination address not set");
        return;
    }

    const auto framecount = get_framecount();
    const auto required_amount_of_data = framecount * audio_format_.bytes_per_frame();
    RAV_ASSERT(packet_intermediate_buffer_.size() == required_amount_of_data, "Buffer size mismatch");

    for (auto i = 0; i < 10; i++) {
        const auto now_samples = ptp_instance_.get_local_ptp_time().to_samples(audio_format_.sample_rate);
        if (WrappingUint(static_cast<uint32_t>(now_samples)) < rtp_packet_.timestamp()) {
            break;
        }

        if (!on_data_requested_handler_(rtp_packet_.timestamp().value(), BufferView(packet_intermediate_buffer_))) {
            return;  // No data was provided
        }

        if (audio_format_.byte_order == AudioFormat::ByteOrder::le) {
            swap_bytes(packet_intermediate_buffer_.data(), required_amount_of_data, audio_format_.bytes_per_sample());
        }

        send_buffer_.clear();
        rtp_packet_.encode(packet_intermediate_buffer_.data(), required_amount_of_data, send_buffer_);
        rtp_packet_.sequence_number_inc(1);
        rtp_packet_.timestamp_inc(framecount);
        rtp_sender_.send_to(send_buffer_, {*configuration_.destination_address, 5004});
    }
}

void rav::RavennaSender::resize_internal_buffers() {
    const auto bytes_per_packet = get_framecount() * audio_format_.bytes_per_frame();
    packet_intermediate_buffer_.resize(bytes_per_packet);
}
