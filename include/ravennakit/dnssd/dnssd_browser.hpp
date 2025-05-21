#pragma once

#include "dnssd_service_description.hpp"
#include "ravennakit/core/util/safe_function.hpp"

#include <boost/asio.hpp>

#include <memory>

namespace rav::dnssd {

/**
 * Base class for dnssd browser implementations.
 */
class Browser {
  public:
    /**
     * Called when a service was discovered.
     * @param description The service description of the discovered service.
     */
    SafeFunction<void(const ServiceDescription& description)> on_service_discovered;

    /**
     * Called when a service was removed.
     * @param description The service description of the removed service.
     */
    SafeFunction<void(const ServiceDescription& description)> on_service_removed;

    /**
     * Called when a service was resolved.
     * @param description The service description of the resolved service.
     */
    SafeFunction<void(const ServiceDescription& description)> on_service_resolved;

    /**
     * Called when a service became available on given address.
     * @param description The service description of the service for which the address was added.
     * @param address The address which was added.
     * @param interface_index The index of the interface on which the address was added.
     */
    SafeFunction<void(const ServiceDescription& description, const std::string& address, uint32_t interface_index)>
        on_address_added;

    /**
     * Called when a service became unavailable on given address.
     * @param description The service description of the service for which the address was removed.
     * @param address The address which was removed.
     * @param interface_index The index of the interface on which the address was removed.
     */
    SafeFunction<void(const ServiceDescription& description, const std::string& address, uint32_t interface_index)>
        on_address_removed;

    /**
     * Called when an error occurred during browsing for a service.
     * @param error_message A message describing the error.
     */
    SafeFunction<void(const std::string& error_message)> on_error;

    virtual ~Browser() = default;

    /**
     * Starts browsing for a service.
     * This function is not thread safe.
     * @param reg_type The service type (i.e. _http._tcp.).
     * @return Returns a result indicating success or failure.
     */
    virtual void browse_for(const std::string& reg_type) = 0;

    /**
     * Creates the most appropriate dnssd_browser implementation for the platform.
     * @return The created dnssd_browser instance, or nullptr if no implementation is available.
     */
    static std::unique_ptr<Browser> create(boost::asio::io_context& io_context);

    /**
     * Tries to find a service by its name.
     * @param service_name The name of the service to find.
     * @return The service description if found, otherwise nullptr.
     */
    [[nodiscard]] virtual const ServiceDescription* find_service(const std::string& service_name) const = 0;

    /**
     * @returns A list of existing services.
     */
    [[nodiscard]] virtual std::vector<ServiceDescription> get_services() const = 0;
};

}  // namespace rav::dnssd
