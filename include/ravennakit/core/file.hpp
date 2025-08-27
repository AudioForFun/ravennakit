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

#include "ravennakit/core/expected.hpp"

#include <filesystem>
#include <fstream>

namespace rav {

/**
 * Represents a file on the file system.
 */
class File {
  public:
    File() = default;

    /**
     * Constructs a file object with the given path.
     * @param path The path to the file or directory.
     */
    explicit File(std::filesystem::path path) : path_(std::move(path)) {}

    /**
     * Constructs a file object with the given path.
     * @param path The path to the file or directory.
     */
    explicit File(const char* path) : path_(path) {}

    /**
     * Appends given path to the file path.
     * @param p The path to append.
     * @return A new file object with the appended path.
     */
    File& operator/(const std::filesystem::path& p) {
        return *this /= p;
    }

    /**
     * Appends given path to the file path.
     * @param p The path to append.
     * @return A new file object with the appended path.
     */
    File& operator/=(const std::filesystem::path& p) {
        path_ /= p;
        return *this;
    }

    /**
     * @returns True if the file of directory exists, or false if the file or directory does not exist.
     */
    [[nodiscard]] bool exists() const {
        return std::filesystem::exists(path_);
    }

    /**
     * Creates the file if it does not already exist.
     * @return True if the file was created or already existed, or false if the file could not be created.
     */
    [[nodiscard]] bool create_if_not_exists() const {
        if (!exists()) {
            std::ofstream f(path_);
            if (!f.good()) {
                return false;  // Failed to create the file
            }
            f.close();
            if (!exists()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @return The path to the file or directory.
     */
    [[nodiscard]] const std::filesystem::path& path() const {
        return path_;
    }

    /**
     * @return The parent directory of the file.
     */
    [[nodiscard]] File parent() const {
        return File(path_.parent_path());
    }

    /**
     * @return The absolute path to the file.
     */
    [[nodiscard]] File absolute() const {
        return File(std::filesystem::absolute(path_));
    }

    /**
     * @throws std::filesystem::filesystem_error if the file does not exist.
     * @return The size of the file in bytes.
     */
    [[nodiscard]] std::uintmax_t size() const {
        return std::filesystem::file_size(path_);
    }

    /**
     * @return Path as a string.
     */
    [[nodiscard]] std::string to_string() const {
        return path_.string();
    }

  private:
    std::filesystem::path path_;
};

}  // namespace rav

namespace rav::file {

enum class Error {
    invalid_path,
    file_does_not_exist,
    failed_to_open,
    failed_to_get_file_size,
    failed_to_read_from_file,
};

inline tl::expected<std::string, Error> read_file_as_string(const std::filesystem::path& file) {
    if (file.empty()) {
        return tl::unexpected(Error::invalid_path);
    }

    std::ifstream stream;

    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    stream.open(file, std::ios::binary | std::ios::ate);
    if (!stream.is_open()) {
        if (!std::filesystem::exists(file)) {
            return tl::unexpected(Error::file_does_not_exist);
        }
        return tl::unexpected(Error::failed_to_open);
    }

    const auto file_size = stream.tellg();
    if (file_size < 0) {
        return tl::unexpected(Error::failed_to_get_file_size);
    }

    stream.seekg(0);

    RAV_ASSERT(
        std::filesystem::file_size(file) == static_cast<uintmax_t>(file_size),
        "File reports a different size than the stream"
    );

    std::string result(static_cast<size_t>(file_size), '\0');
    stream.read(result.data(), file_size);

    if (stream.fail() && !stream.eof()) {
        return tl::unexpected(Error::failed_to_read_from_file);
    }

    const auto count = stream.gcount();
    if (count < 0) {
        return tl::unexpected(Error::failed_to_read_from_file);
    }

    if (count != file_size) {
        return tl::unexpected(Error::failed_to_read_from_file);
    }

    return result;
}

}  // namespace rav::file
