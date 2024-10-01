#pragma once

#include "result.hpp"
#include "service_description.hpp"
#include "bonjour/service.hpp"

#include <functional>

namespace rav::dnssd
{

/**
 * Interface class which represents a Bonjour browser.
 */
class dnssd_browser
{
public:
    using ServiceDiscoveredAsyncCallback = std::function<void (const service_description& serviceDescription)>;
    using ServiceRemovedAsyncCallback = std::function<void (const service_description& serviceDescription)>;
    using ServiceResolvedAsyncCallback =
        std::function<void (const service_description& serviceDescription, uint32_t interfaceIndex)>;
    using AddressAddedAsyncCallback = std::function<
        void (const service_description& serviceDescription, const std::string& address, uint32_t interfaceIndex)>;
    using AddressRemovedAsyncCallback = std::function<
        void (const service_description& serviceDescription, const std::string& address, uint32_t interfaceIndex)>;
    using BrowseErrorAsyncCallback = std::function<void (const result& error)>;

    dnssd_browser() = default;
    virtual ~dnssd_browser() = default;

    /**
     * Starts browsing for a service
     * @param service_type The service type (i.e. _http._tcp.).
     * @return Returns a result indicating success or failure.
     */
    virtual result browse_for (const std::string& service_type) = 0;

    /**
     * Sets a callback which gets called when a service was discovered.
     * Note: this call will be made from a background thread and wil not be synchronised.
     * @param callback Callback with a reference to the ServiceDescription of this service.
     */
    virtual void on_service_discovered (ServiceDiscoveredAsyncCallback callback)
    {
        onServiceDiscoveredCallback = std::move (callback);
    }

    /**
     * Sets a callback which gets called when a service was removed.
     * Note: this call will be made from a background thread and wil not be synchronised.
     * @param callback Callback with the ServiceDescription of the removed service, which will also get removed after
     * this call.
     */
    virtual void on_service_removed (ServiceRemovedAsyncCallback callback)
    {
        onServiceRemovedCallback = std::move (callback);
    }

    /**
     * Sets a callback which gets called when a service was resolved (i.e. address information was resolved).
     * Note: this call will be made from a background thread and wil not be synchronised.
     * @param callback Callback with the service description and the index of the interface on which the service was
     * resolved on.
     */
    virtual void on_service_resolved (ServiceResolvedAsyncCallback callback)
    {
        onServiceResolvedCallback = std::move (callback);
    }

    /**
     * Sets a callback which gets called when an address became available (i.e. service became reachable on this
     * address). Note: this call will be made from a background thread and wil not be synchronised.
     * @param callback Callback with the service description, the added address and the interface index.
     */
    virtual void on_address_added (AddressAddedAsyncCallback callback)
    {
        onAddressAddedCallback = std::move (callback);
    }

    /**
     * Sets a callback which gets called when an address became unavailable (i.e. service no longer reachable on this
     * address). Note: this call will be made from a background thread and wil not be synchronised.
     * @param callback Callback with the service description, the added address and the interface index.
     */
    virtual void on_address_removed (AddressRemovedAsyncCallback callback)
    {
        onAddressRemovedCallback = std::move (callback);
    }

    /**
     * Sets a callback which gets called when there was an error during browsing for a service.
     * Note: this call will be made from a background thread and wil not be synchronised.
     * @param callback A callback with a Result indicating what problem occurred.
     */
    virtual void on_browse_error (BrowseErrorAsyncCallback callback)
    {
        onBrowseErrorCallback = std::move (callback);
    }

protected:
    friend service;

    ServiceDiscoveredAsyncCallback onServiceDiscoveredCallback;
    ServiceResolvedAsyncCallback onServiceResolvedCallback;
    ServiceRemovedAsyncCallback onServiceRemovedCallback;
    AddressAddedAsyncCallback onAddressAddedCallback;
    AddressRemovedAsyncCallback onAddressRemovedCallback;
    BrowseErrorAsyncCallback onBrowseErrorCallback;
};

} // namespace dnssd
