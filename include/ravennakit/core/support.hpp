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

/**
 * This function returns a reference to a read/write, default-constructed static object of type T. There will be exactly
 * one of these objects present per instantiated type, per process.
 */
template<typename T>
T& get_global_instance_of_type() {
    static T _defaultObject;
    return _defaultObject;
}

/**
 * This function returns a reference to a read-only, default-constructed static object of type T. There will be exactly
 * one of these objects present per instantiated type, per process.
 */
template<typename T>
const T& get_global_const_instance_of_type() {
    const static T _defaultObject;
    return _defaultObject;
}

}
