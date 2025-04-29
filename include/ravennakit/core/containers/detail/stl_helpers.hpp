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

namespace rav {

template<typename Map, typename Key>
auto get_or_default(const Map& map, const Key& key) -> typename Map::mapped_type {
    if (auto it = map.find(key); it != map.end())
        return it->second;
    return typename Map::mapped_type{};
}

}
