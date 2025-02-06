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

#include "ptp_measurement.hpp"
#include "ravennakit/ptp/messages/ptp_delay_req_message.hpp"
#include "ravennakit/ptp/messages/ptp_follow_up_message.hpp"
#include "ravennakit/ptp/messages/ptp_sync_message.hpp"
#include "ravennakit/core/random.hpp"
#include "ravennakit/core/tracy.hpp"
#include "ravennakit/core/math/sliding_stats.hpp"
#include "ravennakit/ptp/messages/ptp_delay_resp_message.hpp"

namespace rav {

/**
 * This class captures all the data needed to calculate the mean path delay and the offset from the master clock using
 * the request-response delay mechanism.
 */
class ptp_request_response_delay_sequence {
  public:
    enum class state {
        initial,
        awaiting_follow_up,
        ready_to_be_scheduled,
        delay_req_send_scheduled,
        awaiting_delay_resp,
        delay_resp_received,
    };

    ptp_request_response_delay_sequence() = default;

    /**
     * Constructor.
     * @param sync_message The initiating sync message.
     */
    explicit ptp_request_response_delay_sequence(const ptp_sync_message& sync_message) :
        sync_message_(sync_message), t1_(sync_message.origin_timestamp), t2_(sync_message_.receive_timestamp) {
        corrected_sync_correction_field_ =
            ptp_time_interval::from_wire_format(sync_message_.header.correction_field).total_seconds_double();
        if (sync_message.header.flags.two_step_flag) {
            state_ = state::awaiting_follow_up;
        } else {
            state_ = state::ready_to_be_scheduled;
        }
    }

    /**
     * Updates the sequence with the follow-up message.
     * @param follow_up_message The follow-up message to update with.
     */
    void update(const ptp_follow_up_message& follow_up_message) {
        TRACY_ZONE_SCOPED;
        RAV_ASSERT(state_ == state::awaiting_follow_up, "State should be awaiting_follow_up");
        follow_up_correction_field_ = ptp_time_interval::from_wire_format(follow_up_message.header.correction_field);
        t1_ = follow_up_message.precise_origin_timestamp;
        state_ = state::ready_to_be_scheduled;
    }

    /**
     * Updates the sequence with the delay response message.
     * Sets the state to delay_resp_received.
     * @param delay_resp_message The delay response message to update with.
     */
    void update(const ptp_delay_resp_message& delay_resp_message) {
        TRACY_ZONE_SCOPED;
        RAV_ASSERT(state_ == state::awaiting_delay_resp, "State should be awaiting_delay_resp");
        delay_resp_correction_field_ = ptp_time_interval::from_wire_format(delay_resp_message.header.correction_field);
        t4_ = delay_resp_message.receive_timestamp;
        state_ = state::delay_resp_received;
    }

    /**
     * Tests whether given header matches the sequence.
     * @param header The header to test.
     * @return True if the header matches the sequence, false otherwise.
     */
    [[nodiscard]] bool matches(const ptp_message_header& header) const {
        TRACY_ZONE_SCOPED;
        return sync_message_.header.matches(header);
    }

    /**
     * Creates a delay request message.
     * @param port_ds The port data set of the port that received the sync message.
     * @return The created delay request message.
     */
    [[nodiscard]] ptp_delay_req_message create_delay_req_message(const ptp_port_ds& port_ds) {
        TRACY_ZONE_SCOPED;
        RAV_ASSERT(state_ == state::delay_req_send_scheduled, "State should be delay_req_send_scheduled");
        requesting_port_identity_ = port_ds.port_identity;
        ptp_delay_req_message delay_req_message;
        delay_req_message.header = sync_message_.header;
        delay_req_message.header.source_port_identity = requesting_port_identity_;
        delay_req_message.header.message_type = ptp_message_type::delay_req;
        delay_req_message.header.message_length = ptp_delay_req_message::k_message_length;
        delay_req_message.header.correction_field = {};
        delay_req_message.origin_timestamp = {};
        return delay_req_message;
    }

    /**
     * Schedules the delay request message to be sent.
     * @param port_ds The port data set of the port that received the sync message.
     */
    void schedule_delay_req_message_send(const ptp_port_ds& port_ds) {
        TRACY_ZONE_SCOPED;
        const auto max_interval_ms = std::pow(2, port_ds.log_min_delay_req_interval + 1) * 1000;
        const auto seconds =
            static_cast<double>(random().get_random_int(0, static_cast<int>(max_interval_ms))) / 1000.0;
        scheduled_send_time_ = sync_message_.receive_timestamp;
        scheduled_send_time_.add_seconds(seconds);
        state_ = state::delay_req_send_scheduled;
    }

    /**
     * @return The time the delay request message is scheduled to be sent.
     */
    [[nodiscard]] std::optional<ptp_timestamp> get_delay_req_scheduled_send_time() const {
        TRACY_ZONE_SCOPED;
        if (state_ != state::delay_req_send_scheduled) {
            return std::nullopt;
        }
        return scheduled_send_time_;
    }

    /**
     * Sets the time the delay request message was sent.
     * Sets the state to awaiting_delay_resp.
     * @param sent_at The time the delay request message was sent.
     */
    void set_delay_req_sent_time(const ptp_timestamp& sent_at) {
        TRACY_ZONE_SCOPED;
        RAV_ASSERT(state_ == state::delay_req_send_scheduled, "State should be delay_req_send_scheduled");
        t3_ = sent_at;
        state_ = state::awaiting_delay_resp;
    }

    /**
     * @return The port identity of the port that initiated the sequence.
     */
    [[nodiscard]] const ptp_port_identity& get_requesting_port_identity() const {
        return requesting_port_identity_;
    }

    /**
     * @return The sequence id of the sync message.
     */
    [[nodiscard]] wrapping_uint<uint16_t> get_sequence_id() const {
        return sync_message_.header.sequence_id;
    }

    /**
     * Calculates the mean path delay.
     * @return The mean path delay.
     */
    [[nodiscard]] double calculate_mean_path_delay() const {
        TRACY_ZONE_SCOPED;
        RAV_ASSERT(state_ == state::delay_resp_received, "State should be delay_resp_received");
        const auto t1 = t1_.total_seconds_double();
        const auto t2 = t2_.total_seconds_double();
        const auto t3 = t3_.total_seconds_double();
        const auto t4 = t4_.total_seconds_double();

        auto result = t2 - t3 + (t4 - t1) - corrected_sync_correction_field_;
        if (sync_message_.header.flags.two_step_flag) {
            result -= follow_up_correction_field_.total_seconds_double();
        }
        result -= delay_resp_correction_field_.total_seconds_double();
        return result / 2.0;
    }

    /**
     * @return The current state of the sequence.
     */
    [[nodiscard]] state get_state() const {
        return state_;
    }

    /**
     * @return The current state of the sequence as a string.
     */
    [[nodiscard]] std::string to_string() const {
        TRACY_ZONE_SCOPED;
        return fmt::format(
            "{}, state: {}, requesting_port_identity: {}", sync_message_.header.sequence_id.value(),
            state_to_string(state_), requesting_port_identity_.to_string()
        );
    }

  private:
    state state_ = state::initial;
    ptp_sync_message sync_message_;
    ptp_timestamp scheduled_send_time_ {};
    double corrected_sync_correction_field_ {};
    ptp_time_interval follow_up_correction_field_ {};
    ptp_time_interval delay_resp_correction_field_ {};
    ptp_timestamp t1_ {};  // Sync.originTimestamp or Follow_Up.preciseOriginTimestamp if two-step
    ptp_timestamp t2_ {};  // Sync receive time (measured locally)
    ptp_timestamp t3_ {};  // Delay request send time (measured locally)
    ptp_timestamp t4_ {};  // Delay_Resp.receiveTimestamp
    ptp_port_identity requesting_port_identity_ {};

    static const char* state_to_string(const state s) {
        switch (s) {
            case state::initial:
                return "initial";
            case state::awaiting_follow_up:
                return "awaiting_follow_up";
            case state::ready_to_be_scheduled:
                return "ready_to_be_scheduled";
            case state::delay_req_send_scheduled:
                return "delay_req_send_scheduled";
            case state::awaiting_delay_resp:
                return "awaiting_delay_resp";
            case state::delay_resp_received:
                return "delay_resp_received";
            default:
                return "unknown";
        }
    }
};

}  // namespace rav
