/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#pragma once

#include <cstddef>

namespace rav::util {

/**
 * Returns the number of elements in a c-style array.
 * @tparam Type The type of the elements.
 * @tparam N The total number of elements.
 * @return The number of elements in the array.
 */
template<typename Type, size_t N>
constexpr int num_elements_in_array(Type (&)[N]) noexcept {
    return N;
}

}  // namespace rsdk
