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
#include "nmos_flow_core.hpp"

namespace rav::nmos {

struct FlowAudio: FlowCore {
    struct SampleRate {
        int numerator {};
        int denominator {1};
    };

    /// Format of the data coming from the Flow as a URN
    static constexpr auto k_format {"urn:x-nmos:format:audio"};

    /// Number of audio samples per second for this Flow
    SampleRate sample_rate {};
};

inline void
tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const FlowAudio::SampleRate& sample_rate) {
    jv = {{"numerator", sample_rate.numerator}, {"denominator", sample_rate.denominator}};
}

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const FlowAudio& flow_audio) {
    tag_invoke(tag, jv, static_cast<const FlowCore&>(flow_audio));
    auto& object = jv.as_object();
    object["format"] = FlowAudio::k_format;
    object["sample_rate"] = boost::json::value_from(flow_audio.sample_rate);
}

}  // namespace rav::nmos
