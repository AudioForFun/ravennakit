//
// Created by Ruurd Adema on 27/08/2025.
// Copyright (c) 2025 Sound on Digital. All rights reserved.
//

#include "ravennakit/core/util/paths.hpp"

#include <fmt/format.h>

int main() {
    fmt::println("Home: {}", rav::paths::home().c_str());
    fmt::println("Desktop: {}", rav::paths::desktop().c_str());
    fmt::println("Documents: {}", rav::paths::documents().c_str());
    fmt::println("Downloads: {}", rav::paths::downloads().c_str());
    fmt::println("Pictures: {}", rav::paths::pictures().c_str());
    fmt::println("Application data: {}", rav::paths::application_data().c_str());
    fmt::println("Cache: {}", rav::paths::cache().c_str());
}
