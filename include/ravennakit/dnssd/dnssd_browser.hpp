#pragma once

#include "service_description.hpp"
#include "ravennakit/core/events.hpp"
#include "ravennakit/core/linked_node.hpp"

#include <asio.hpp>

#include <functional>
#include <memory>

namespace rav::dnssd {

/**
 * Interface class which represents a Bonjour browser.
 */
class dnssd_browser {
  public:
    /**
     * Event for when a service was discovered.
     * Note: this event will be emitted asynchronously from a background thread.
     */
    struct service_discovered {
        /// The service description of the discovered service.
        const service_description& description;
    };

    /**
     * Event for when a service was removed.
     * Note: this event will be emitted asynchronously from a background thread.
     */
    struct service_removed {
        /// The service description of the removed service.
        const service_description& description;
    };

    /**
     * Event for when a service was resolved (i.e. address information was resolved).
     * Note: this event will be emitted asynchronously from a background thread.
     */
    struct service_resolved {
        /// The service description of the resolved service.
        const service_description& description;
    };

    /**
     * Event for when the service became available on given address.
     * Note: this event will be emitted asynchronously from a background thread.
     */
    struct address_added {
        /// The service description of the service for which the address was added.
        const service_description& description;
        /// The address which was added.
        const std::string& address;
        /// The index of the interface on which the address was added.
        uint32_t interface_index;
    };

    /**
     * Event for when the service became unavailable on given address.
     * Note: this event will be emitted asynchronously from a background thread.
     */
    struct address_removed {
        /// The service description of the service for which the address was removed.
        const service_description& description;
        /// The address which was removed.
        const std::string& address;
        /// The index of the interface on which the address was removed.
        uint32_t interface_index;
    };

    /**
     * Event for when an error occurred during browsing for a service.
     * Note: this event might be emitted asynchronously from a background thread.
     */
    struct browse_error {
        const std::string& error_message;
    };

    using subscriber = linked_node<
        events<service_discovered, service_removed, service_resolved, address_added, address_removed, browse_error>>;

    virtual ~dnssd_browser() = default;

    /**
     * Starts browsing for a service.
     * This function is not thread safe.
     * @param service_type The service type (i.e. _http._tcp.).
     * @return Returns a result indicating success or failure.
     */
    virtual void browse_for(const std::string& service_type) = 0;

    /**
     * Creates the most appropriate dnssd_browser implementation for the platform.
     * @return The created dnssd_browser instance, or nullptr if no implementation is available.
     */
    static std::unique_ptr<dnssd_browser> create(asio::io_context& io_context);

    /**
     * Tries to find a service by its name.
     * @param service_name The name of the service to find.
     * @return The service description if found, otherwise nullptr.
     */
    [[nodiscard]] virtual const service_description* find_service(const std::string& service_name) const = 0;

    /**
     * @returns A list of existing services.
     */
    [[nodiscard]] virtual std::vector<service_description> get_services() const = 0;

    /**
     * Subscribes given subscriber to the browser. The subscriber will receive future events as well event to get
     * up-to-date with the current state (i.e. existing discovered services).
     * @param s The subscriber to subscribe.
     */
    virtual void subscribe(subscriber& s) = 0;
};

}  // namespace rav::dnssd
