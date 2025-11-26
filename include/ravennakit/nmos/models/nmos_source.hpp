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

#include "nmos_source_audio.hpp"

#include <variant>

namespace rav::nmos {

/**
 * Describes a Source
 * https://specs.amwa.tv/is-04/releases/v1.3.3/APIs/schemas/with-refs/source.html
 */
struct Source {
    std::variant<SourceAudio> any_of;

    [[nodiscard]] boost::uuids::uuid get_id() const {
        return std::visit(
            [](const auto& source) {
                return source.id;
            },
            any_of
        );
    }

    [[nodiscard]] Version get_version() const {
        return std::visit(
            [](const auto& source) {
                return source.version;
            },
            any_of
        );
    }

    void set_version(const Version& version) {
        std::visit(
            [&version](auto& source) {
                source.version = version;
            },
            any_of
        );
    }

    [[nodiscard]] boost::uuids::uuid get_device_id() const {
        return std::visit(
            [](const auto& source) {
                return source.device_id;
            },
            any_of
        );
    }
};

inline void tag_invoke(const boost::json::value_from_tag& tag, boost::json::value& jv, const Source& source) {
    std::visit(
        [&tag, &jv](const auto& s) {
            tag_invoke(tag, jv, s);
        },
        source.any_of
    );
}

}  // namespace rav::nmos
