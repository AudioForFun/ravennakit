//
// Created by Ruurd Adema on 27/08/2025.
// Copyright (c) 2025 Sound on Digital. All rights reserved.
//

#include "ravennakit/core/file.hpp"
#include "ravennakit/core/util/common_paths.hpp"
#include "ravennakit/core/util/uri.hpp"

#include <fmt/format.h>

namespace {
void print(const char* subject, const std::filesystem::path& path) {
#if RAV_WINDOWS
    fmt::println("{}: {}", subject, path.string());
#else
    fmt::println("{}: file://{}", subject, rav::Uri::encode(path.string()));
#endif
}
}  // namespace

int main() {
    print("Home", rav::common_paths::home());
    print("Desktop", rav::common_paths::desktop());
    print("Documents", rav::common_paths::documents());
    print("Downloads", rav::common_paths::downloads());
    print("Pictures", rav::common_paths::pictures());
    print("Application data", rav::common_paths::application_data());
    print("Cache", rav::common_paths::cache());
    print("Temporary std", std::filesystem::temp_directory_path());
}
