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

namespace rav::aes67::constants {

/**
 * The maximum MTU for AES67 packets. From AES67-2023 6.3.
 * Note: on connections offering lower MTU than Ethernet’s 1500 bytes, senders can use a smaller maximum payload than
 * specified here.
 */
static constexpr auto k_max_payload = 1440;

/**
 * The standard 1500-byte Ethernet MTU is assumed. From AES67-2023 6.3.
 */
static constexpr auto k_mtu = 1500;

}  // namespace rav::aes67::constants
