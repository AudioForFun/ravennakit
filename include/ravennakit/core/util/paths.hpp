//
// Created by Ruurd Adema on 27/08/2025.
// Copyright (c) 2025 Sound on Digital. All rights reserved.
//

#pragma once

#include <filesystem>

namespace rav::paths {

/**
 * macOS: /Users/<username>
 * @return The path to the users home folder, or an empty path if the home folder could not be determined.
 */
std::filesystem::path home();

/**
 * macOS: /Users/<username>/Desktop
 * @return The path to the desktop folder, or an empty path if the folder could not be determined.
 */
std::filesystem::path desktop();

/**
 * macOS: /Users/<username>/Documents
 * @return The path to the documents folder, or an empty path if the folder could not be determined.
 */
std::filesystem::path documents();

/**
 * macOS: /Users/<username>/Pictures
 * @return The path to the pictures folder, or an empty path if the folder could not be determined.
 */
std::filesystem::path pictures();

/**
 * macOS: /Users/<username>/Downloads
 * @return The path to the downloads folder, or an empty path if the folder could not be determined.
 */
std::filesystem::path downloads();

/**
 * macOS: /Users/<username>/Library/Application Support
 * @return A path to the application data folder, or an empty path if the folder could not be retrieved.
 */
std::filesystem::path application_data();

/**
 * macOS: /Users/<username>/Library/Caches
 * @return A path to the cache folder, or an empty path if the folder could not be retrieved.
 */
std::filesystem::path cache();

}  // namespace rav::paths
