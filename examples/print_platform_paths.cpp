//
// Created by Ruurd Adema on 27/08/2025.
// Copyright (c) 2025 Sound on Digital. All rights reserved.
//

#include "ravennakit/core/file.hpp"
#include "ravennakit/core/util/paths.hpp"

#include <fmt/format.h>

int main() {
    fmt::println("Home: {}", rav::paths::home().string());
    fmt::println("Desktop: {}", rav::paths::desktop().string());
    fmt::println("Documents: {}", rav::paths::documents().string());
    fmt::println("Downloads: {}", rav::paths::downloads().string());
    fmt::println("Pictures: {}", rav::paths::pictures().string());
    fmt::println("Application data: {}", rav::paths::application_data().string());
    fmt::println("Cache: {}", rav::paths::cache().string());
}
