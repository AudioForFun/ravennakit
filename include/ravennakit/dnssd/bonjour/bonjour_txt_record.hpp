#pragma once

#include "bonjour.hpp"

#if RAV_HAS_APPLE_DNSSD

#include "ravennakit/dnssd/dnssd_service_description.hpp"

    #include <map>
#include <string>

namespace rav::dnssd
{

/**
 * Class for holding and working with a TXTRecordRef
 */
class BonjourTxtRecord
{
public:
    explicit BonjourTxtRecord (const TxtRecord& txtRecord);
    ~BonjourTxtRecord();

    /**
     * Sets a value inside the TXT record.
     * @param key Key.
     * @param value Value.
     * @return A result indicating success or failure.
     */
    void set_value(const std::string& key, const std::string& value);

    /**
     * Sets an empty value for key inside the TXT record.
     * @param key Key.
     * @return An Result indicating success or failure.
     */
    void set_value(const std::string& key);

    /**
     * @return Returns the length of the TXT record.
     */
    [[nodiscard]] uint16_t length() const noexcept;

    /**
     * @return Returns a pointer to the TXT record data. This pointer will be valid for as long as this instance lives.
     */
    [[nodiscard]] const void* bytes_ptr() const noexcept;

    /**
     * Creates a TxtRecord from raw TXT record bytes.
     * @param txt_record The txt record data.
     * @param txt_record_length The length of the txt record.
     * @return The filled TxtRecord.
     */
    static TxtRecord get_txt_record_from_raw_bytes (const unsigned char* txt_record, uint16_t txt_record_length) noexcept;

private:
    TXTRecordRef txt_record_ref_{};
};

} // namespace dnssd

#endif
