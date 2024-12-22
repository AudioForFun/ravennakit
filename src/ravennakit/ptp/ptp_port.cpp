/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/ptp/ptp_port.hpp"

#include "ravennakit/core/util.hpp"
#include "ravennakit/core/containers/buffer_view.hpp"
#include "ravennakit/ptp/ptp_instance.hpp"
#include "ravennakit/ptp/ptp_profiles.hpp"
#include "ravennakit/ptp/messages/ptp_follow_up_message.hpp"
#include "ravennakit/ptp/messages/ptp_pdelay_resp_follow_up_message.hpp"
#include "ravennakit/ptp/messages/ptp_pdelay_resp_message.hpp"

namespace {
const auto k_ptp_multicast_address = asio::ip::make_address("224.0.1.129");
constexpr auto k_ptp_event_port = 319;
constexpr auto k_ptp_general_port = 320;
}  // namespace

rav::ptp_port::ptp_port(
    ptp_instance& parent, asio::io_context& io_context, const asio::ip::address& interface_address,
    const ptp_port_identity port_identity
) :
    parent_(parent),
    event_socket_(io_context, asio::ip::address_v4(), k_ptp_event_port),
    general_socket_(io_context, asio::ip::address_v4(), k_ptp_general_port) {
    // Initialize the port data set
    port_ds_.port_identity = port_identity;
    port_ds_.port_state = ptp_state::initializing;

    subscriptions_.push_back(event_socket_.join_multicast_group(k_ptp_multicast_address, interface_address));
    subscriptions_.push_back(general_socket_.join_multicast_group(k_ptp_multicast_address, interface_address));

    auto handler = [this](const udp_sender_receiver::recv_event& event) {
        handle_recv_event(event);
    };

    event_socket_.start(handler);
    general_socket_.start(handler);

    port_ds_.port_state = ptp_state::listening;
}

const rav::ptp_port_identity& rav::ptp_port::get_port_identity() const {
    return port_ds_.port_identity;
}

void rav::ptp_port::assert_valid_state(const ptp_profile& profile) const {
    port_ds_.assert_valid_state(profile);
}

void rav::ptp_port::apply_state_decision_algorithm(
    const ptp_default_ds& default_ds, const std::optional<ptp_best_announce_message>& ebest
) {
    if (!ebest && port_ds_.port_state == ptp_state::listening) {
        RAV_TRACE("Remaining in listening state because no Ebest is available");
        return;
    }

    const auto recommended_state = calculate_recommended_state(default_ds, ebest->get_comparison_data_set());
    if (!recommended_state) {
        RAV_TRACE("Port is listening, and no ebest is available. No state change is recommended.");
        return;
    }

    RAV_TRACE("Calculated recommended state: {}", to_string(recommended_state.value()));

    parent_.update_data_sets(recommended_state.value(), ebest ? std::optional(ebest->message) : std::nullopt);


}

std::optional<rav::ptp_state_decision_code> rav::ptp_port::calculate_recommended_state(
    const ptp_default_ds& default_ds, const std::optional<ptp_comparison_data_set>& ebest
) const {
    if (!ebest && port_ds_.port_state == ptp_state::listening) {
        return std::nullopt;
    }

    const ptp_comparison_data_set d0(default_ds);

    if (range(1, 127).contains(default_ds.clock_quality.clock_class)) {
        // D0 better or better by topology than Erbest
        if (!erbest_
            || d0.compare(ptp_comparison_data_set(*erbest_, port_ds_.port_identity))
                >= ptp_comparison_data_set::result::better_by_topology) {
            return ptp_state_decision_code::m1;  // BMC_MASTER (D0)
        }
        return ptp_state_decision_code::p1;  // BMC_PASSIVE (Erbest)
    }

    // D0 better or better by topology than Ebest
    if (!ebest || d0.compare(*ebest) >= ptp_comparison_data_set::result::better_by_topology) {
        return ptp_state_decision_code::m2;  // BMC_MASTER (D0)
    }

    // Ebest received on port R
    if (ebest->identity_of_receiver == port_ds_.port_identity) {
        return ptp_state_decision_code::s1;  // BMC_SLAVE (Ebest = Erbest)
    }

    // Ebest better by topology than Erbest
    if (!erbest_
        || ebest->compare(ptp_comparison_data_set(*erbest_, port_ds_.port_identity))
            == ptp_comparison_data_set::result::better_by_topology) {
        return ptp_state_decision_code::p2;  // BMC_PASSIVE (Erbest)
    }

    return ptp_state_decision_code::m3;  // BMC_MASTER (Ebest)
}

rav::ptp_state rav::ptp_port::state() const {
    return port_ds_.port_state;
}

std::optional<rav::ptp_best_announce_message>
rav::ptp_port::determine_ebest(const std::vector<std::unique_ptr<ptp_port>>& ports) {
    const ptp_port* best_port = nullptr;

    for (auto& port : ports) {
        if (port == nullptr) {
            RAV_ASSERT_FALSE("Found a nullptr in the port list");
            continue;
        }

        port->calculate_erbest();

        if (!port->erbest_) {
            continue;
        }

        if (best_port == nullptr) {
            best_port = port.get();
            continue;
        }

        if (!best_port->erbest_) {
            best_port = port.get();
            continue;
        }

        ptp_comparison_data_set best_port_set(best_port->erbest_.value(), best_port->port_ds_.port_identity);
        ptp_comparison_data_set port_set(port->erbest_.value(), port->port_ds_.port_identity);

        const auto r = port_set.compare(best_port_set);

        if (r >= ptp_comparison_data_set::result::error1 && r <= ptp_comparison_data_set::result::error2) {
            RAV_ERROR("PTP data set comparison failed");
            return std::nullopt;
        }

        if (r >= ptp_comparison_data_set::result::better_by_topology) {
            best_port = port.get();
        }
    }

    if (best_port == nullptr) {
        return std::nullopt;
    }

    RAV_ASSERT(best_port->erbest_, "Best port should have a valid Erbest");

    return ptp_best_announce_message {*best_port->erbest_, best_port->port_ds_.port_identity};
}

void rav::ptp_port::handle_recv_event(const udp_sender_receiver::recv_event& event) {
    const buffer_view data(event.data, event.size);
    auto header = ptp_message_header::from_data(data);
    if (!header) {
        RAV_TRACE("PTP Header error: {}", to_string(header.error()));
        return;
    }

    if (!parent_.should_process_ptp_messages(header.value())) {
        return;
    }

    switch (header->message_type) {
        case ptp_message_type::announce: {
            auto announce_message =
                ptp_announce_message::from_data(header.value(), data.subview(ptp_message_header::k_header_size));
            if (!announce_message) {
                RAV_ERROR("{} error: {}", header->to_string(), to_string(announce_message.error()));
            }
            RAV_TRACE("Announce from {}", announce_message->source_to_string());
            handle_announce_message(announce_message.value(), {});
            break;
        }
        case ptp_message_type::sync: {
            auto sync_message = ptp_sync_message::from_data(data.subview(ptp_message_header::k_header_size));
            if (!sync_message) {
                RAV_ERROR("{} error: {}", header->to_string(), to_string(sync_message.error()));
            }
            // RAV_TRACE("{} {}", header->to_string(), sync_message->to_string());
            handle_sync_message(sync_message.value(), {});
            break;
        }
        case ptp_message_type::delay_req: {
            auto delay_req = ptp_delay_req_message::from_data(data.subview(ptp_message_header::k_header_size));
            if (!delay_req) {
                RAV_ERROR("{} error: {}", header->to_string(), to_string(delay_req.error()));
            }
            // RAV_TRACE("{} {}", header->to_string(), delay_req->to_string());
            handle_delay_req_message(delay_req.value(), {});
            break;
        }
        case ptp_message_type::p_delay_req: {
            auto pdelay_req = ptp_pdelay_req_message::from_data(data.subview(ptp_message_header::k_header_size));
            if (!pdelay_req) {
                RAV_ERROR("{} error: {}", header->to_string(), to_string(pdelay_req.error()));
            }
            // RAV_TRACE("{} {}", header->to_string(), pdelay_req->to_string());
            handle_pdelay_req_message(pdelay_req.value(), {});
            break;
        }
        case ptp_message_type::p_delay_resp: {
            auto pdelay_resp = ptp_pdelay_resp_message::from_data(data.subview(ptp_message_header::k_header_size));
            if (!pdelay_resp) {
                RAV_ERROR("{} error: {}", header->to_string(), to_string(pdelay_resp.error()));
            }
            // RAV_TRACE("{} {}", header->to_string(), pdelay_resp->to_string());
            handle_pdelay_resp_message(pdelay_resp.value(), {});
            break;
        }
        case ptp_message_type::follow_up: {
            auto follow_up = ptp_follow_up_message::from_data(data.subview(ptp_message_header::k_header_size));
            if (!follow_up) {
                RAV_ERROR("{} error: {}", header->to_string(), to_string(follow_up.error()));
            }
            // RAV_TRACE("{} {}", header->to_string(), follow_up->to_string());
            handle_follow_up_message(follow_up.value(), {});
            break;
        }
        case ptp_message_type::delay_resp: {
            auto delay_resp = ptp_delay_req_message::from_data(data.subview(ptp_message_header::k_header_size));
            if (!delay_resp) {
                RAV_ERROR("{} error: {}", header->to_string(), to_string(delay_resp.error()));
            }
            // RAV_TRACE("{} {}", header->to_string(), delay_resp->to_string());
            handle_delay_resp_message(delay_resp.value(), {});
            break;
        }
        case ptp_message_type::p_delay_resp_follow_up: {
            auto pdelay_resp_follow_up =
                ptp_pdelay_resp_follow_up_message::from_data(data.subview(ptp_message_header::k_header_size));
            if (!pdelay_resp_follow_up) {
                RAV_ERROR("{} error: {}", header->to_string(), to_string(pdelay_resp_follow_up.error()));
            }
            // RAV_TRACE("{} {}", header->to_string(), pdelay_resp_follow_up->to_string());
            handle_pdelay_resp_follow_up_message(pdelay_resp_follow_up.value(), {});
            break;
        }
        case ptp_message_type::signaling: {
            RAV_ASSERT_FALSE("Signaling messages are not implemented");
            break;
        }
        case ptp_message_type::management: {
            RAV_ASSERT_FALSE("Management messages are not implemented");
            break;
        }
        case ptp_message_type::reserved1:
        case ptp_message_type::reserved2:
        case ptp_message_type::reserved3:
        case ptp_message_type::reserved4:
        case ptp_message_type::reserved5:
        case ptp_message_type::reserved6:
        default: {
            RAV_WARNING("Unknown PTP message received: {}", to_string(header->message_type));
        }
    }
}

void rav::ptp_port::handle_announce_message(
    const ptp_announce_message& announce_message, buffer_view<const uint8_t> tlvs
) {
    std::ignore = announce_message;
    std::ignore = tlvs;

    if (port_ds_.port_state == ptp_state::initializing) {
        RAV_TRACE("Discarding announce message while initializing");
        return;
    }

    if (port_ds_.port_state == ptp_state::disabled) {
        RAV_TRACE("Discarding announce message while disabled");
        return;
    }

    if (port_ds_.port_state == ptp_state::faulty) {
        RAV_TRACE("Discarding announce message while faulty");
        return;
    }

    // IEEE 1588-2019: 9.3.2.5.a If a message comes from the same PTP instance, the message is not qualified.
    if (announce_message.header.source_port_identity.clock_identity == port_ds_.port_identity.clock_identity) {
        RAV_WARNING("Message is not qualified because it comes from the same PTP instance");
        return;
    }

    // IEEE 1588-2019: 9.3.2.5.d If steps removed of 255 or greater, message is not qualified.
    if (announce_message.steps_removed >= 255) {
        RAV_WARNING("Message is not qualified because steps removed is 255 or greater");
        return;
    }

    // IEEE 1588-2019: 9.5.3
    if (port_ds_.port_state == ptp_state::slave || port_ds_.port_state == ptp_state::uncalibrated) {
        if (announce_message.header.source_port_identity == parent_.get_parent_ds().parent_port_identity) {
            parent_.update_data_sets(ptp_state_decision_code::s1, announce_message);
            // TODO: Reset the announce receipt timeout timer
        } else {
            // Message is considered foreign
            foreign_master_list_.add_or_update_entry(announce_message);
        }
    } else {
        // Message is considered foreign
        foreign_master_list_.add_or_update_entry(announce_message);
    }

    // IEEE 1588-2019: 9.3.2.3.c If the port state is Slave, Uncalibrated, or Passive, the previous Erbest is used, and
    // updated with newer messages from this port.
    if (port_ds_.port_state == ptp_state::slave || port_ds_.port_state == ptp_state::uncalibrated
        || port_ds_.port_state == ptp_state::passive) {
        if (erbest_ && erbest_->header.source_port_identity == announce_message.header.source_port_identity) {
            if (announce_message.header.sequence_id > erbest_->header.sequence_id) {
                erbest_ = announce_message;
                RAV_TRACE("Updated Erbest with newer announce message from the same port: {}", erbest_->to_string());
            } else {
                RAV_WARNING("Received an older announce message from the same port");
            }
        }
    }

    // IEEE 1588-2019: 9.5.3 NOTE 2 Trigger state decision event right away
    parent_.execute_state_decision_event();
}

void rav::ptp_port::handle_sync_message(const ptp_sync_message& sync_message, buffer_view<const uint8_t> tlvs) {
    std::ignore = sync_message;
    std::ignore = tlvs;
}

void rav::ptp_port::handle_delay_req_message(
    const ptp_delay_req_message& delay_req_message, buffer_view<const uint8_t> tlvs
) {
    std::ignore = delay_req_message;
    std::ignore = tlvs;
}

void rav::ptp_port::handle_follow_up_message(
    const ptp_follow_up_message& follow_up_message, buffer_view<const uint8_t> tlvs
) {
    std::ignore = follow_up_message;
    std::ignore = tlvs;
}

void rav::ptp_port::handle_delay_resp_message(
    const ptp_delay_req_message& delay_resp_message, buffer_view<const uint8_t> tlvs
) {
    std::ignore = delay_resp_message;
    std::ignore = tlvs;
}

void rav::ptp_port::handle_pdelay_req_message(
    const ptp_pdelay_req_message& delay_req_message, buffer_view<const uint8_t> tlvs
) {
    std::ignore = delay_req_message;
    std::ignore = tlvs;
}

void rav::ptp_port::handle_pdelay_resp_message(
    const ptp_pdelay_resp_message& delay_req_message, buffer_view<const uint8_t> tlvs
) {
    std::ignore = delay_req_message;
    std::ignore = tlvs;
}

void rav::ptp_port::handle_pdelay_resp_follow_up_message(
    const ptp_pdelay_resp_follow_up_message& delay_req_message, buffer_view<const uint8_t> tlvs
) {
    std::ignore = delay_req_message;
    std::ignore = tlvs;
}

void rav::ptp_port::calculate_erbest() {
    if (erbest_) {
        RAV_ASSERT(
            foreign_master_list_.size() > 0,
            "Erbest is set, but there are no entries in the foreign master list. Only the entries which didn't become the best announce message should have been removed."
        );

        if (foreign_master_list_.size() == 1) {
            RAV_ASSERT(
                foreign_master_list_.begin()->foreign_master_port_identity == erbest_->header.source_port_identity,
                "Erbest is set, but the foreign master list contains an entry from a different source."
            );

            // Since no new entries were added, erbest is still the best.
            return;
        }
    }

    // IEEE 1588-2019: 9.3.2.3 Calculate Erbest
    std::optional<ptp_announce_message> erbest;

    bool should_include_previous_erbest = erbest_
        && (port_ds_.port_state == ptp_state::slave || port_ds_.port_state == ptp_state::uncalibrated
            || port_ds_.port_state == ptp_state::passive);

    for (const auto& entry : foreign_master_list_) {
        if (const auto& announce_message = entry.most_recent_announce_message) {
            if (should_include_previous_erbest
                && announce_message->header.source_port_identity == erbest_->header.source_port_identity) {
                // Don't include this entry, because it is the previous Erbest, which might have been updated outside
                // the foreign master list.
                continue;
            }
            RAV_TRACE("Consider Erbest candidate: {}", announce_message->source_to_string());
            if (erbest) {
                if (ptp_comparison_data_set::compare(*announce_message, *erbest, port_ds_.port_identity)
                    >= ptp_comparison_data_set::result::better_by_topology) {
                    erbest = announce_message;
                }
            } else {
                erbest = announce_message;
            }
        }
    }

    if (erbest) {
        if (should_include_previous_erbest) {
            if (ptp_comparison_data_set::compare(*erbest, *erbest_, port_ds_.port_identity)
                >= ptp_comparison_data_set::result::better_by_topology) {
                erbest_ = erbest;
                RAV_TRACE("New Erbest: {}", erbest_->source_to_string());
            }
        } else {
            erbest_ = erbest;  // The non-empty set is considered better than the empty set.
            RAV_TRACE("New Erbest: {}", erbest_->source_to_string());
        }

        // IEEE 1588-2019: 9.3.2.3.b Remove entries from the foreign master list which didn't become Erbest.
        // Only when we found a new Erbest, we remove the other entries.
        foreign_master_list_.remove_all_except_for(erbest_);
    }
}
