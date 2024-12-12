#include "ravennakit/core/log.hpp"
#include "ravennakit/dnssd/bonjour/bonjour.hpp"

#if RAV_HAS_APPLE_DNSSD

    #include "ravennakit/dnssd/bonjour/bonjour_txt_record.hpp"

    #include <dns_sd.h>
    #include <map>

rav::dnssd::bonjour_txt_record::bonjour_txt_record(const txt_record& txtRecord) {
    // By passing 0 and nullptr, TXTRecordCreate will arrange allocation for a buffer.
    TXTRecordCreate(&txt_record_ref_, 0, nullptr);

    for (auto& kv : txtRecord) set_value(kv.first, kv.second);
}

rav::dnssd::bonjour_txt_record::~bonjour_txt_record() {
    TXTRecordDeallocate(&txt_record_ref_);
}

void rav::dnssd::bonjour_txt_record::set_value(const std::string& key, const std::string& value) {
    DNSSD_THROW_IF_ERROR(
        TXTRecordSetValue(&txt_record_ref_, key.c_str(), static_cast<uint8_t>(value.length()), value.c_str()),
        "Failed to set txt record value"
    );
}

void rav::dnssd::bonjour_txt_record::set_value(const std::string& key) {
    DNSSD_THROW_IF_ERROR(TXTRecordSetValue(&txt_record_ref_, key.c_str(), 0, nullptr), "Failed to set txt record key");
}

uint16_t rav::dnssd::bonjour_txt_record::length() const noexcept {
    return TXTRecordGetLength(&txt_record_ref_);
}

const void* rav::dnssd::bonjour_txt_record::bytes_ptr() const noexcept {
    return TXTRecordGetBytesPtr(&txt_record_ref_);
}

std::map<std::string, std::string> rav::dnssd::bonjour_txt_record::get_txt_record_from_raw_bytes(
    const unsigned char* txt_record, const uint16_t txt_record_length
) noexcept {
    std::map<std::string, std::string> txtRecord;

    uint8_t value_len;
    const void* value;

    for (uint16_t i = 0; i < TXTRecordGetCount(txt_record_length, txt_record); i++) {
        constexpr int key_buf_len = 256;
        char key[key_buf_len];
        auto result = TXTRecordGetItemAtIndex(txt_record_length, txt_record, i, key_buf_len, key, &value_len, &value);
        if (result != kDNSServiceErr_NoError) {
            RAV_ERROR("Failed to get txt record item at index: {}", result);
            continue;
        }
        if (value_len == 0) {
            continue;
        }
        std::string str_key(key);
        std::string str_value(static_cast<const char*>(value), value_len);
        txtRecord.insert({str_key, str_value});
    }

    return txtRecord;
}

#endif
