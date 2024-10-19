/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2024 Owllab. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <optional>

namespace rav {

/**
 * Baseclass for classes that want to provide stream-like access to data.
 */
class stream {
  public:
    stream() = default;
    virtual ~stream() = default;

    /**
     * Reads data from the stream into the given buffer.
     * @param buffer The buffer to read data into.
     * @param size The number of bytes to read.
     * @return The number of bytes read.
     */
    virtual size_t read(uint8_t* buffer, size_t size) = 0;

    /**
     * Writes data from the given buffer to the stream.
     * @param buffer The buffer to write data from.
     * @param size The number of bytes to write.
     * @return The number of bytes written.
     */
    virtual size_t write(const uint8_t* buffer, size_t size) = 0;

    /**
     * Sets the read position in the stream.
     * @param position The new read position.
     * @return True if the read position was successfully set.
     */
    virtual bool set_read_position(size_t position) = 0;

    /**
     * @return The current read position in the stream.
     */
    [[nodiscard]] virtual size_t get_read_position() const = 0;

    /**
     * Sets the write position in the stream.
     * @param position The new write position.
     * @return True if the write position was successfully set.
     */
    virtual bool set_write_position(size_t position) = 0;

    /**
     * @return The current write position in the stream.
     */
    [[nodiscard]] virtual size_t get_write_position() const = 0;

    /**
     * @return The total number of bytes in this stream. Not all streams support this operation, in which case an empty
     * optional is returned.
     */
    [[nodiscard]] virtual std::optional<size_t> size() const = 0;

    /**
     * @return The number of bytes remaining to read in this stream. Not all streams support this operation, in which
     * case an empty optional is returned.
     */
    [[nodiscard]] std::optional<size_t> remaining() const;

    /**
     * @return True if the stream has no more data to read.
     */
    [[nodiscard]] virtual bool exhausted() const;

    /**
     * Flushes the stream, ensuring that all data is written to the underlying storage. Not all streams support this
     * operation.
     */
    virtual void flush() = 0;
};

}  // namespace rav
