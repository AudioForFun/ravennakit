#include "ravennakit/dnssd/bonjour/bonjour_browser.hpp"

#include <iostream>
#include <string>

int main (const int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Expected an argument which specifies the servicetype to browse for (example: _http._tcp)"
                  << std::endl;
        return -1;
    }

    dnssd::bonjour_browser browser;

    browser.on_service_discovered ([] (const dnssd::service_description& serviceDescription) {
        std::cout << "Service discovered: " << serviceDescription.description() << std::endl;
    });

    browser.on_service_removed ([] (const dnssd::service_description& serviceDescription) {
        std::cout << "Service removed: " << serviceDescription.description() << std::endl;
    });

    browser.on_service_resolved ([] (const dnssd::service_description& serviceDescription, uint32_t interfaceIndex) {
        std::cout << "Service resolved: " << serviceDescription.description() << std::endl;
    });

    browser.on_address_added (
        [] (const dnssd::service_description& serviceDescription, const std::string& address, uint32_t interfaceIndex) {
            std::cout << "Address added (" << address << "): " << serviceDescription.description() << std::endl;
        });

    browser.on_address_removed (
        [] (const dnssd::service_description& serviceDescription, const std::string& address, uint32_t interfaceIndex) {
            std::cout << "Address removed (" << address << "): " << serviceDescription.description() << std::endl;
        });

    browser.on_browse_error ([] (const dnssd::result& error) {
        std::cout << "Error: " << error.description() << std::endl;
    });

    auto const result = browser.browse_for (argv[1]);
    if (result.has_error())
    {
        std::cout << "Error: " << result.description() << std::endl;
        return -1;
    };

    std::cout << "Press enter to exit..." << std::endl;

    std::string cmd;
    std::getline (std::cin, cmd);

    std::cout << "Exit" << std::endl;

    return 0;
}
