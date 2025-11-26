// SPDX-License-Identifier: AGPL-3.0-or-later
/*
 * Project: RAVENNAKIT (RAVENNA / AES67 / ST2110-30 SDK)
 * Copyright (c) 2024-2025 Sound on Digital
 *
 * This file is part of RAVENNAKIT.
 *
 * RAVENNAKIT is dual-licensed:
 *   1) Under the terms of the GNU Affero General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version (the "AGPL License"); and
 *   2) Under a commercial license from Sound on Digital, for customers who
 *      cannot (or do not wish to) comply with the AGPL License terms.
 *
 * If you obtained this file under the AGPL License, you may redistribute it
 * and/or modify it under the terms of the AGPL License. See the LICENSE
 * file in the project root for details.
 *
 * For commercial licensing, support, and other inquiries, please visit:
 *
 *     https://ravennakit.com
 *
 */

#pragma once
#include <cstdint>

namespace rav::ptp {

/**
 * The PTP state.
 * IEEE1588-2019: 8.2.15.3.1, 9.2.5, Table 27
 */
enum class State : uint8_t {
    undefined = 0x0,  // Not specified in IEEE1588-2019
    initializing = 0x1,
    faulty = 0x2,
    disabled = 0x3,
    listening = 0x4,
    pre_master = 0x5,
    master = 0x6,
    passive = 0x7,
    uncalibrated = 0x8,
    slave = 0x9,
};

/**
 * Converts a ptp_state to a string.
 * @param state The state to convert.
 * @return The string representation of the state.
 */
inline const char* to_string(const State state) {
    switch (state) {
        case State::undefined:
            return "undefined";
        case State::initializing:
            return "initializing";
        case State::faulty:
            return "faulty";
        case State::disabled:
            return "disabled";
        case State::listening:
            return "listening";
        case State::pre_master:
            return "pre_master";
        case State::master:
            return "master";
        case State::passive:
            return "passive";
        case State::uncalibrated:
            return "uncalibrated";
        case State::slave:
            return "slave";
        default:
            return "unknown";
    }
}

/**
 * IEEE1588-2019: 7.6.2.6, Table 5
 */
enum class ClockAccuracy : uint8_t {
    // 0x00 to 0x16 Reserved
    lt_1_ps = 0x17,    // The time is accurate to within 1 picosecond
    lt_2_5_ps = 0x18,  // The time is accurate to within 2.5 picoseconds
    lt_10_ps = 0x19,   // The time is accurate to within 10 picoseconds
    lt_25_ps = 0x1a,   // The time is accurate to within 25 picoseconds
    lt_100_ps = 0x1b,  // The time is accurate to within 100 picoseconds
    lt_250_ps = 0x1c,  // The time is accurate to within 250 picoseconds
    lt_1_ns = 0x1d,    // The time is accurate to within 1 nanosecond
    lt_2_5_ns = 0x1e,  // The time is accurate to within 2.5 nanoseconds
    lt_10_ns = 0x1f,   // The time is accurate to within 10 nanoseconds
    lt_25_ns = 0x20,   // The time is accurate to within 25 nanoseconds
    lt_100_ns = 0x21,  // The time is accurate to within 100 nanoseconds
    lt_250_ns = 0x22,  // The time is accurate to within 250 nanoseconds
    lt_1_us = 0x23,    // The time is accurate to within 1 microsecond
    lt_2_5_us = 0x24,  // The time is accurate to within 2.5 microseconds
    lt_10_us = 0x25,   // The time is accurate to within 10 microseconds
    lt_25_us = 0x26,   // The time is accurate to within 25 microseconds
    lt_100_us = 0x27,  // The time is accurate to within 100 microseconds
    lt_250_us = 0x28,  // The time is accurate to within 250 microseconds
    lt_1_ms = 0x29,    // The time is accurate to within 1 millisecond
    lt_2_5_ms = 0x2a,  // The time is accurate to within 2.5 milliseconds
    lt_10_ms = 0x2b,   // The time is accurate to within 10 milliseconds
    lt_25_ms = 0x2c,   // The time is accurate to within 25 milliseconds
    lt_100_ms = 0x2d,  // The time is accurate to within 100 milliseconds
    lt_250_ms = 0x2e,  // The time is accurate to within 250 milliseconds
    lt_1_s = 0x2f,     // The time is accurate to within 1 second
    lt_10_s = 0x30,    // The time is accurate to within 10 seconds
    gt_10_s = 0x31,    // Greater than 10 seconds
    // 0x32 to 0x7f Reserved
    // 0x80 to 0xfd Designated for assignment by alternate PTP Profiles
    unknown = 0xFE,   // The accuracy of the time is unknown
    reserved = 0xFF,  // Reserved
};

inline const char* to_string(const ClockAccuracy accuracy) {
    switch (accuracy) {
        case ClockAccuracy::lt_1_ps:
            return "within 1 picosecond";
        case ClockAccuracy::lt_2_5_ps:
            return "within 2.5 picoseconds";
        case ClockAccuracy::lt_10_ps:
            return "within 10 picoseconds";
        case ClockAccuracy::lt_25_ps:
            return "within 25 picoseconds";
        case ClockAccuracy::lt_100_ps:
            return "within 100 picoseconds";
        case ClockAccuracy::lt_250_ps:
            return "within 250 picoseconds";
        case ClockAccuracy::lt_1_ns:
            return "within 1 nanosecond";
        case ClockAccuracy::lt_2_5_ns:
            return "within 2.5 nanoseconds";
        case ClockAccuracy::lt_10_ns:
            return "within 10 nanoseconds";
        case ClockAccuracy::lt_25_ns:
            return "within 25 nanoseconds";
        case ClockAccuracy::lt_100_ns:
            return "within 100 nanoseconds";
        case ClockAccuracy::lt_250_ns:
            return "within 250 nanoseconds";
        case ClockAccuracy::lt_1_us:
            return "within 1 microsecond";
        case ClockAccuracy::lt_2_5_us:
            return "within 2.5 microseconds";
        case ClockAccuracy::lt_10_us:
            return "within 10 microseconds";
        case ClockAccuracy::lt_25_us:
            return "within 25 microseconds";
        case ClockAccuracy::lt_100_us:
            return "within 100 microseconds";
        case ClockAccuracy::lt_250_us:
            return "within 250 microseconds";
        case ClockAccuracy::lt_1_ms:
            return "within 1 millisecond";
        case ClockAccuracy::lt_2_5_ms:
            return "within 2.5 milliseconds";
        case ClockAccuracy::lt_10_ms:
            return "within 10 milliseconds";
        case ClockAccuracy::lt_25_ms:
            return "within 25 milliseconds";
        case ClockAccuracy::lt_100_ms:
            return "within 100 milliseconds";
        case ClockAccuracy::lt_250_ms:
            return "within 250 milliseconds";
        case ClockAccuracy::lt_1_s:
            return "within 1 second";
        case ClockAccuracy::lt_10_s:
            return "within 10 seconds";
        case ClockAccuracy::gt_10_s:
            return "greater than 10 seconds";
        case ClockAccuracy::reserved:
            return "reserved";
        case ClockAccuracy::unknown:
        default:
            return "unknown";
    }
}

/**
 * PTP Time source
 * IEEE1588-2019: 7.6.2.8, Table 6
 */
enum class TimeSource : uint8_t {
    undefined = 0x0,  // Not specified in IEEE1588-2019
    atomic_clock = 0x10,
    gnss = 0x20,
    terrestrial_radio = 0x30,
    serial_time_code = 0x39,
    ptp = 0x40,
    ntp = 0x50,
    hand_set = 0x60,
    other = 0x90,
    internal_oscillator = 0xA0,
    // 0xF0 to 0xFE Designated for assignment by alternate PTP Profiles
    reserved = 0xFF,
};

/**
 * State decision codes.
 * IEEE1588-2019: 9.3.1, 9.3.5, Table 30, 31, 32, 33.
 */
enum class StateDecisionCode {
    /// The PTP Port is in the MASTER state because it is on a clockClass 1 through 127 PTP Instance and is a PTP Port
    /// of the Grandmaster PTP Instance of the domain.
    m1,
    /// The PTP Port is in the MASTER state because it is on a clockClass 128 or higher PTP Instance and is a PTP Port
    /// of the Grandmaster PTP Instance of the domain.
    m2,
    /// The PTP Port is in the MASTER state, but it is not a PTP Port on the Grandmaster PTP Instance of the domain.
    m3,
    /// The PTP Port is in the SLAVE state.
    s1,
    /// The PTP Port is in the PASSIVE state because it is on a clockClass 1 through 127 PTP Instance and is either not
    /// on the Grandmaster PTP Instance of the domain or is PASSIVE to break a timing loop.
    p1,
    /// The PTP Port is in the PASSIVE state because it is on a clockClass 128 or higher PTP Instance and is PASSIVE to
    /// break a timing loop.
    p2
};

/**
 * Converts a ptp_state_decision_code to a string.
 * @param code The code to convert.
 * @return The string representation of the code.
 */
inline const char* to_string(const StateDecisionCode code) {
    switch (code) {
        case StateDecisionCode::m1:
            return "M1";
        case StateDecisionCode::m2:
            return "M2";
        case StateDecisionCode::m3:
            return "M3";
        case StateDecisionCode::s1:
            return "S1";
        case StateDecisionCode::p1:
            return "P1";
        case StateDecisionCode::p2:
            return "P2";
        default:
            return "Unknown";
    }
}

/**
 * PTP Message types.
 * IEEE1588-2019: Table 36
 */
enum class MessageType : uint8_t {
    sync = 0x0,          // Event
    delay_req = 0x1,     // Event
    p_delay_req = 0x2,   // Event
    p_delay_resp = 0x3,  // Event
    reserved1 = 0x4,
    reserved2 = 0x5,
    reserved3 = 0x6,
    reserved4 = 0x7,
    follow_up = 0x8,               // General
    delay_resp = 0x9,              // General
    p_delay_resp_follow_up = 0xa,  // General
    announce = 0xb,                // General
    signaling = 0xc,               // General
    management = 0xd,              // General
    reserved5 = 0xe,
    reserved6 = 0xf,
};

inline const char* to_string(const MessageType type) {
    switch (type) {
        case MessageType::sync:
            return "Sync";
        case MessageType::delay_req:
            return "Delay_Req";
        case MessageType::p_delay_req:
            return "Pdelay_Req";
        case MessageType::p_delay_resp:
            return "Pdelay_Resp";
        case MessageType::follow_up:
            return "Follow_Up";
        case MessageType::delay_resp:
            return "Delay_resp";
        case MessageType::p_delay_resp_follow_up:
            return "Pdelay_Resp_Follow_Up";
        case MessageType::announce:
            return "Announce";
        case MessageType::signaling:
            return "Signaling";
        case MessageType::management:
            return "Management";
        case MessageType::reserved1:
        case MessageType::reserved2:
        case MessageType::reserved3:
        case MessageType::reserved4:
        case MessageType::reserved5:
        case MessageType::reserved6:
            return "Reserved";
        default:
            return "Unknown";
    }
}

/**
 * PTP Delay mechanism.
 * IEEE1588-2019: 8.2.15.4.4, Table 21
 */
enum class DelayMechanism : uint8_t {
    undefined = 0x0,  // Not specified in IEEE1588-2019
    e2e = 0x1,
    p2p = 0x2,
    common_ptp = 0x3,
    special = 0x4,
    no_mechanism = 0xfe,
};

}  // namespace rav::ptp
