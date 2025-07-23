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

#include <algorithm>
#include <array>
#include <utility>

namespace rav {

/**
 * Convenience function to remove items from an stl container which match `pred`.
 * @tparam Container The container type.
 * @tparam Pred The predicate type.
 * @param container The container to remove item(s) from.
 * @param pred The predicate. If it returns true the item will be removed.
 * @return Number of elements removed.
 */
template<typename Container, typename Pred>
size_t stl_remove_if(Container& container, Pred pred) {
    auto old_size = container.size();
    container.erase(std::remove_if(container.begin(), container.end(), pred), container.end());
    return old_size - container.size();
}

/**
 * Convenience function to get an item from a map, or if the item doesn't exist return a default value.
 * @tparam Map The type of the map.
 * @tparam Key The key type.
 * @param map The map to get the item from.
 * @param key The key of the item to get.
 * @return The item, or a default constructed value.
 */
template<typename Map, typename Key>
auto stl_get_or_default(const Map& map, const Key& key) -> typename Map::mapped_type {
    if (auto it = map.find(key); it != map.end())
        return it->second;
    return typename Map::mapped_type{};
}

// Helper for `generate_array`
template <typename T, std::size_t N, typename F, std::size_t... Is>
std::array<T, N> generate_array_impl(F&& f, std::index_sequence<Is...>) {
    return {{ f(Is)... }};
}

/**
 * Helper function to initialize std::array with non-copyable types.
 */
template <typename T, std::size_t N, typename F>
std::array<T, N> generate_array(F&& f) {
    return generate_array_impl<T, N>(std::forward<F>(f), std::make_index_sequence<N>{});
}

}  // namespace rav
