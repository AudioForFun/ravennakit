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

#include <cassert>
#include <iostream>

#include "exception.hpp"
#include "log.hpp"

/**
 * When RAV_LOG_ON_ASSERT is defined as true (1), a log message will be emitted when an assertion is hit. Default is on.
 */
#ifndef RAV_LOG_ON_ASSERT
    #define RAV_LOG_ON_ASSERT 1  // Enabled by default
#endif

/**
 * When RAV_THROW_EXCEPTION_ON_ASSERT is defined as true (1), an exception will be thrown when an assertion is hit.
 * Default is off.
 */
#ifndef RAV_THROW_EXCEPTION_ON_ASSERT
    #define RAV_THROW_EXCEPTION_ON_ASSERT 0
#endif

/**
 * When RAV_ABORT_ON_ASSERT is defined as true (1), program execution will abort when an assertion is hit. Default is
 * off.
 */
#ifndef RAV_ABORT_ON_ASSERT
    #define RAV_ABORT_ON_ASSERT 0
#endif

/**
 * The RAV_DEBUG macro enables certain debugging facilities. Define RAV_ENABLE_DEBUG to enable RAV_DEBUG in release builds.
 */
#if !defined(NDEBUG) || (defined(RAV_ENABLE_DEBUG) && (RAV_ENABLE_DEBUG != 0))
    #define RAV_DEBUG 1
#else
    #define RAV_DEBUG 0
#endif

/**
 * Logs a message if RAV_LOG_ON_ASSERT is set to true.
 * @param msg The message to log.
 */
#define LOG_IF_ENABLED(msg)    \
    if (RAV_LOG_ON_ASSERT) {   \
        RAV_LOG_CRITICAL(msg); \
    }

/**
 * Throws an exception if RAV_THROW_EXCEPTION_ON_ASSERT is set to true.
 * @param msg The message of the exception.
 */
#define THROW_EXCEPTION_IF_ENABLED(msg)  \
    if (RAV_THROW_EXCEPTION_ON_ASSERT) { \
        RAV_THROW_EXCEPTION(msg);        \
    }

/**
 * Aborts program execution if RAV_ABORT_ON_ASSERT is set to true.
 * @param msg The message to write to stdout before aborting.
 */
#define ABORT_IF_ENABLED(msg)                                      \
    if (RAV_ABORT_ON_ASSERT) {                                     \
        std::cerr << "Abort on assertion: " << (msg) << std::endl; \
        std::abort();                                              \
    }

/**
 * Asserts condition to be true, otherwise:
 *  - Logs if enabled
 *  - Throws if enabled
 *  - Aborts if enabled
 * @param condition The condition to test.
 * @param message The message for logging, throwing and/or aborting.
 */
#define RAV_ASSERT(condition, message)                                \
    do {                                                              \
        if (!(condition)) {                                           \
            LOG_IF_ENABLED("Assertion failure: " message)             \
            THROW_EXCEPTION_IF_ENABLED("Assertion failure: " message) \
            ABORT_IF_ENABLED(message)                                 \
        }                                                             \
    } while (false)

#if RAV_DEBUG
    /**
     * If RAV_DEBUG is enabled, asserts condition to be true, otherwise:
     *  - Logs if enabled
     *  - Throws if enabled
     *  - Aborts if enabled
     * @param condition The condition to test.
     * @param message The message for logging, throwing and/or aborting.
     */
    #define RAV_ASSERT_DEBUG(condition, message)                          \
        do {                                                              \
            if (!(condition)) {                                           \
                LOG_IF_ENABLED("Assertion failure: " message)             \
                THROW_EXCEPTION_IF_ENABLED("Assertion failure: " message) \
                ABORT_IF_ENABLED(message)                                 \
            }                                                             \
        } while (false)
#else
    #define RAV_ASSERT_DEBUG(condition, message) \
        do {                                     \
        } while (false)
#endif

/**
 * Assert condition to be true, otherwise:
 *  - Logs if enabled
 *  - Throws if enabled
 *  - Aborts if enabled
 *  - Returns void if `condition` is false
 * @param condition The condition to test.
 * @param message The message for logging, throwing and/or aborting.
 */
#define RAV_ASSERT_RETURN(condition, message)                         \
    do {                                                              \
        if (!(condition)) {                                           \
            LOG_IF_ENABLED("Assertion failure: " message)             \
            THROW_EXCEPTION_IF_ENABLED("Assertion failure: " message) \
            ABORT_IF_ENABLED(message)                                 \
            return;                                                   \
        }                                                             \
    } while (false)

/**
 * Asserts condition to be true, otherwise:
 *  - Logs if enabled
 *  - Throws if enabled
 *  - Aborts if enabled
 *  - Returns given `return_value` if `condition` is false
 * @param condition The condition to test.
 * @param message The message for logging, throwing and/or aborting.
 * @param return_value The value to return.
 */
#define RAV_ASSERT_RETURN_WITH(condition, message, return_value)      \
    do {                                                              \
        if (!(condition)) {                                           \
            LOG_IF_ENABLED("Assertion failure: " message)             \
            THROW_EXCEPTION_IF_ENABLED("Assertion failure: " message) \
            ABORT_IF_ENABLED(message)                                 \
            return return_value;                                      \
        }                                                             \
    } while (false)

/**
 * Asserts given condition, but never throws. Useful for places where an exception cannot be thrown like destructors.
 * @param condition The condition to test.
 * @param message The message to log or abort with.
 */
#define RAV_ASSERT_NO_THROW(condition, message)           \
    do {                                                  \
        if (!(condition)) {                               \
            LOG_IF_ENABLED("Assertion failure: " message) \
            ABORT_IF_ENABLED(message)                     \
        }                                                 \
    } while (false)

/**
 * Asserts with false, entering the RAV_ASSERT procedure as a quick way to assert that a branch is invalid.
 * @param message The message to log, throw, abort with.
 */
#define RAV_ASSERT_FALSE(message) RAV_ASSERT(false, message)
