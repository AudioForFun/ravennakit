#pragma once

#include "bonjour/scoped_dns_service_ref.hpp"

#include <exception>
#include <string>

namespace dnssd
{

/**
 * Class for indicating success or failure. It can hold a DNSServiceErrorType and an error message where either will
 * make the Result indicate an error.
 */
class result
{
public:
    result() = default;

    /**
     * Construct this Result with given DNSServiceErrorType.
     * @param error The DNSServiceErrorType to store.
     */
    explicit result (DNSServiceErrorType error) noexcept;

    /**
     * Constructs this Result with an error message.
     * @param errorMsg The error message to store.
     */
    explicit result (const std::string& errorMsg) noexcept;

    /**
     * @return Returns true if this result holds an error.
     */
    [[nodiscard]] bool has_error() const;

    /**
     * @return Returns true if this result doesn't hold an error.
     */
    [[nodiscard]] bool is_ok() const;

    /**
     * @return Returns a description of this Result, which will either return the error message or a description of the stored
     * DNSServiceErrorType.
     */
    [[nodiscard]] std::string description() const noexcept;

private:
    DNSServiceErrorType error_ = kDNSServiceErr_NoError;
    std::string error_msg_;

    static const char* DNSServiceErrorDescription (DNSServiceErrorType error) noexcept;
};

} // namespace dnssd
