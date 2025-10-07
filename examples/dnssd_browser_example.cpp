#include "ravennakit/core/log.hpp"
#include "ravennakit/core/system.hpp"
#include "ravennakit/dnssd/dnssd_browser.hpp"

#include <iostream>
#include <string>

int main(const int argc, char* argv[]) {
    rav::set_log_level_from_env();
    rav::do_system_checks();

    if (argc < 2) {
        std::cout << "Expected an argument which specifies the service type to browse for (example: _http._tcp)"
                  << std::endl;
        return -1;
    }

    boost::asio::io_context io_context;  // NOLINT

    const auto browser = rav::dnssd::Browser::create(io_context);

    if (browser == nullptr) {
        std::cout << "No browser implementation available for this platform" << std::endl;
        exit(-1);
    }

    browser->on_service_discovered = [](const auto& desc) {
        RAV_LOG_INFO("Service discovered: {}", desc.to_string());
    };
    browser->on_service_removed = [](const auto& desc) {
        RAV_LOG_INFO("Service removed: {}", desc.to_string());
    };
    browser->on_service_resolved = [](const auto& desc) {
        RAV_LOG_INFO("Service resolved: {}", desc.to_string());
    };
    browser->on_address_added = [](const auto& desc, const auto& address, const auto& interface_index) {
        RAV_LOG_INFO("Address added ({}): {} on interface {}", address, desc.to_string(), interface_index);
    };
    browser->on_address_removed = [](const auto& desc, const auto& address, const auto& interface_index) {
        RAV_LOG_INFO("Address removed ({}): {} on interface {}", address, desc.to_string(), interface_index);
    };
    browser->on_error = [](const auto& error_message){
        RAV_LOG_ERROR("Error: {}", error_message);
    };

    browser->browse_for(argv[1]);

    std::thread io_context_thread([&io_context] {
        io_context.run();
    });

    std::cout << "Press enter to exit..." << std::endl;
    std::cin.get();

    io_context.stop();
    io_context_thread.join();

    std::cout << "Exit" << std::endl;

    return 0;
}
