#include "ravennakit/dnssd/log.hpp"

#include <iostream>
#include <string>
#ifdef _WIN32
    #include <Windows.h>
#endif

static void dnssd_debug_log(const std::string& str) {
#ifdef _WIN32
    OutputDebugStringA(str.c_str());
#else
    std::cout << str;
#endif
}
