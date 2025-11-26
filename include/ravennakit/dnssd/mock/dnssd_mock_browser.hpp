// SPDX-License-Identifier: AGPL-3.0-or-later
/*
 * Project: RAVENNAKIT (RAVENNA / AES67 / ST2110-30 SDK)
 * Copyright (c) 2024-2025 Sound on Digital
 *
 * This file is part of RAVENNAKIT.
 *
 * RAVENNAKIT is dual-licensed:
 *   1) Under the terms of the GNU Affero General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version (the "AGPL License"); and
 *   2) Under a commercial license from Sound on Digital, for customers who
 *      cannot (or do not wish to) comply with the AGPL License terms.
 *
 * If you obtained this file under the AGPL License, you may redistribute it
 * and/or modify it under the terms of the AGPL License. See the LICENSE
 * file in the project root for details.
 *
 * For commercial licensing, support, and other inquiries, please visit:
 *
 *     https://ravennakit.com
 *
 */

#pragma once
#include "ravennakit/dnssd/dnssd_browser.hpp"

namespace rav::dnssd {

class MockBrowser: public Browser {
  public:
    explicit MockBrowser(boost::asio::io_context& io_context);
    ~MockBrowser() override = default;

    /**
     * Mocks discovering a service.
     * @param fullname The fullname of the service. Should not contain spaces.
     * @param name The name of the service.
     * @param reg_type The registration type of the service (i.e. _http._tcp.).
     * @param domain The domain of the service (i.e. local.).
     */
    void
    mock_discovered_service(const std::string& fullname, const std::string& name, const std::string& reg_type, const std::string& domain);

    /**
     * Mocks resolving a service. Requires calling mock_discovered_service before.
     * @param fullname The fullname of the service which was discovered before.
     * @param host_target The host target of the service.
     * @param port The port of the service.
     * @param txt_record The txt record of the service.
     */
    void mock_resolved_service(const std::string& fullname, const std::string& host_target, uint16_t port, const TxtRecord& txt_record);

    /**
     * Mocks adding an address to a service. Requires calling mock_discovered_service before.
     * @param fullname The fullname of the service which was discovered before.
     * @param address The address to add.
     * @param interface_index
     */
    void mock_added_address(const std::string& fullname, const std::string& address, uint32_t interface_index);

    /**
     * Mocks removing an address from a service. Requires calling mock_discovered_service before.
     * @param fullname The fullname of the service which was discovered before.
     * @param address The address to remove.
     * @param interface_index The interface index of the address.
     */
    void mock_removed_address(const std::string& fullname, const std::string& address, uint32_t interface_index);

    /**
     * Mocks removing a service. Requires calling mock_discovered_service before.
     * @param fullname The fullname of the service which was discovered before.
     */
    void mock_removed_service(const std::string& fullname);

    // dnssd_browser overrides
    void browse_for(const std::string& service_type) override;
    [[nodiscard]] const ServiceDescription* find_service(const std::string& service_name) const override;
    [[nodiscard]] std::vector<ServiceDescription> get_services() const override;

  private:
    boost::asio::io_context& io_context_;
    std::map<std::string, ServiceDescription> services_;  // fullname -> service description
    std::set<std::string> browsers_;                      // reg_type
};

}  // namespace rav::dnssd
