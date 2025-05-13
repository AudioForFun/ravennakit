/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#pragma once

#include "ravennakit/core/log.hpp"
#include "ravennakit/core/net/http/http_client.hpp"
#include "ravennakit/nmos/detail/nmos_fmt_adapters.hpp"

#include <string_view>

#include <boost/json.hpp>

namespace rav::nmos {

/**
 * NMOS Test API client.
 * This class is used to test the connection to the NMOS Test API.
 */
class NmosTestApiClient {
  public:
    NmosTestApiClient(boost::asio::io_context& io_context, const std::string_view test_api_url) :
        http_client_(io_context, test_api_url) {}

    /**
     * Tests the connection to the NMOS Test API.
     * @return A result which indicates success or failure.
     */
    [[nodiscard]] boost::system::result<void, std::string> test_connection() const {
        auto result = http_client_.get("/api");
        if (result.has_error()) {
            return result.error().message();
        }

        RAV_TRACE("NMOS Test api: {}", *result);

        const auto j = boost::json::parse(result->body());
        if (!j.is_object()) {
            return "NMOS Test api: expected a JSON object";
        }

        if (j.try_as_object().value().at("suite").try_as_string().value() != "IS-04-01") {
            return "NMOS Test api: expected a JSON array";
        }

        return {};
    }

    /**
     * @return A list of available test suites, or an error if the connectin fails.
     */
    [[nodiscard]] boost::system::result<std::vector<std::string>, std::string> get_suites() const {
        auto result = http_client_.post("/api", R"({"list_suites":true})", "application/json");
        if (result.has_error()) {
            return result.error().message();
        }

        if (result->base().result() != http::status::ok) {
            return result->body();
        }

        auto j = boost::json::parse(result->body());

        auto array = j.try_as_array();
        if (!array) {
            return "NMOS Test api: expected a JSON array";
        }

        std::vector<std::string> suites;
        suites.reserve(array->size());
        for (const auto& item : *array) {
            if (item.is_string()) {
                suites.emplace_back(item.as_string());
            } else {
                return "NMOS Test api: expected a JSON array of strings";
            }
        }

        return suites;
    }

    /**
     * Makes a request to the test api to run a test suite.
     * @param suite The name of the test suite to run ("IS-04-01").
     * @param host The host to test against.
     * @param port The port to test against.
     * @param versions The list of API versions to test against.
     * @return A result which indicates success or failure.
     */
    [[nodiscard]] boost::system::result<void, std::string> run_test_suite(
        std::string_view suite, std::string_view host, uint16_t port, const std::vector<std::string>& versions
    ) const {
        boost::json::array version_array;
        for (const auto& v : versions) {
            version_array.emplace_back(v);
        }

        boost::json::value jv = {
            {"suite", suite},
            {"host", boost::json::array {host}},
            {"port", boost::json::array {port}},
            {"version", version_array}
        };

        auto result = http_client_.post("/api", serialize(jv), "application/json");
        if (result.has_error()) {
            return result.error().message();
        }

        if (result->base().result() != http::status::ok) {
            return result->body();
        }

        return {};
    }

    /**
     * @return The NMOS Test API URL from the environment variable NMOS_TEST_API_URL, or an empty optional if not set.
     */
    [[nodiscard]] static std::optional<std::string> get_test_api_url_from_env() {
        return get_env("NMOS_TEST_API_URL");
    }

  private:
    HttpClient http_client_;
};

}  // namespace rav::nmos
