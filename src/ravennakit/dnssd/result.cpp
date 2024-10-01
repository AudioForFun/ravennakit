/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#include "ravennakit/dnssd/result.hpp"

#include <sstream>

rav::dnssd::result::result(DNSServiceErrorType error) noexcept {
    if (error != kDNSServiceErr_NoError) {
        std::stringstream errorMessage;
        errorMessage << "DNSServiceError: (" << result::DNSServiceErrorDescription(error) << ")";
        error_msg_ = errorMessage.str();
    }

    error_ = error;
}

rav::dnssd::result::result (const std::string& errorMsg) noexcept
{
    error_msg_ = errorMsg;
}

std::string rav::dnssd::result::description() const noexcept
{
    if (error_msg_.empty())
    {
        return "no error";
    }
    return error_msg_;
}

const char* rav::dnssd::result::DNSServiceErrorDescription (const DNSServiceErrorType error) noexcept
{
    switch (error)
    {
    case kDNSServiceErr_NoError:
        return "kDNSServiceErr_NoError";
    case kDNSServiceErr_Unknown:
        return "kDNSServiceErr_Unknown";
    case kDNSServiceErr_NoSuchName:
        return "kDNSServiceErr_NoSuchName";
    case kDNSServiceErr_NoMemory:
        return "kDNSServiceErr_NoMemory";
    case kDNSServiceErr_BadParam:
        return "kDNSServiceErr_BadParam";
    case kDNSServiceErr_BadReference:
        return "kDNSServiceErr_BadReference";
    case kDNSServiceErr_BadState:
        return "kDNSServiceErr_BadState";
    case kDNSServiceErr_BadFlags:
        return "kDNSServiceErr_BadFlags";
    case kDNSServiceErr_Unsupported:
        return "kDNSServiceErr_Unsupported";
    case kDNSServiceErr_NotInitialized:
        return "kDNSServiceErr_NotInitialized";
    case kDNSServiceErr_AlreadyRegistered:
        return "kDNSServiceErr_AlreadyRegistered";
    case kDNSServiceErr_NameConflict:
        return "kDNSServiceErr_NameConflict";
    case kDNSServiceErr_Invalid:
        return "kDNSServiceErr_Invalid";
    case kDNSServiceErr_Firewall:
        return "kDNSServiceErr_Firewall";
    case kDNSServiceErr_Incompatible:
        return "kDNSServiceErr_Incompatible (client library incompatible with daemon)";
    case kDNSServiceErr_BadInterfaceIndex:
        return "kDNSServiceErr_BadInterfaceIndex";
    case kDNSServiceErr_Refused:
        return "kDNSServiceErr_Refused";
    case kDNSServiceErr_NoSuchRecord:
        return "kDNSServiceErr_NoSuchRecord";
    case kDNSServiceErr_NoAuth:
        return "kDNSServiceErr_NoAuth";
    case kDNSServiceErr_NoSuchKey:
        return "kDNSServiceErr_NoSuchKey";
    case kDNSServiceErr_NATTraversal:
        return "kDNSServiceErr_NATTraversal";
    case kDNSServiceErr_DoubleNAT:
        return "kDNSServiceErr_DoubleNAT";
    case kDNSServiceErr_BadTime:
        return "kDNSServiceErr_BadTime";
    case kDNSServiceErr_BadSig:
        return "kDNSServiceErr_BadSig";
    case kDNSServiceErr_BadKey:
        return "kDNSServiceErr_BadKey";
    case kDNSServiceErr_Transient:
        return "kDNSServiceErr_Transient";
    case kDNSServiceErr_ServiceNotRunning:
        return "kDNSServiceErr_ServiceNotRunning (background daemon not running)";
    case kDNSServiceErr_NATPortMappingUnsupported:
        return "kDNSServiceErr_NATPortMappingUnsupported (NAT doesn't support PCP, NAT-PMP or UPnP)";
    case kDNSServiceErr_NATPortMappingDisabled:
        return "kDNSServiceErr_NATPortMappingDisabled (NAT supports PCP, NAT-PMP or UPnP, but it's disabled by the "
               "administrator)";
    case kDNSServiceErr_NoRouter:
        return "kDNSServiceErr_NoRouter (No router currently configured (probably no network connectivity)";
    case kDNSServiceErr_PollingMode:
        return "kDNSServiceErr_PollingMode";
    case kDNSServiceErr_Timeout:
        return "kDNSServiceErr_Timeout";
#ifndef _WIN32
    case kDNSServiceErr_DefunctConnection:
        return "kDNSServiceErr_DefunctConnection (connection to daemon returned a SO_ISDEFUNCT error result)";
#endif
    default:
        return "Unknown error";
    }
}

bool rav::dnssd::result::has_error() const
{
    return error_ != kDNSServiceErr_NoError || !error_msg_.empty();
}

bool rav::dnssd::result::is_ok() const
{
    return !has_error();
}
