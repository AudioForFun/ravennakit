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

#include "rtsp_request.hpp"

namespace rav {

class rtsp_request_parser {
  public:
    enum class result {
        good,
        indeterminate,
        bad_method,
        bad_uri,
        bad_protocol,
        bad_version,
        bad_header,
        bad_end_of_headers
    };

    explicit rtsp_request_parser(rtsp_request& request) : request_(request) {}

    /**
     * Parses input and feeds the output to given request.
     * @tparam Iterator The interator type.
     * @param begin The beginning of the input.
     * @param end The end of the input.
     * @return A tuple with a result indicating the status, and an iterator indicating where the parsing stopped.
     */
    template<class Iterator>
    std::tuple<result, Iterator> parse(Iterator begin, Iterator end) {
        RAV_ASSERT(begin <= end, "Invalid input iterators");

        while (begin < end) {
            if (remaining_expected_data_ > 0) {
                const auto max_data = std::min(remaining_expected_data_, end - begin);
                request_.data.insert(request_.data.end(), begin, begin + max_data);
                remaining_expected_data_ -= max_data;
                begin += max_data;
                if (remaining_expected_data_ == 0) {
                    return std::make_tuple(result::good, begin);  // Reached the end of data, which means we are done
                }
                if (remaining_expected_data_ > 0) {
                    return std::make_tuple(result::indeterminate, begin); // Need more data
                }
                RAV_ASSERT_FALSE("remaining_expected_data_ is negative, which should never happen");
                return std::make_tuple(result::bad_header, begin);
            }

            RAV_ASSERT(begin < end, "Expecting to have data available at this point");
            RAV_ASSERT(remaining_expected_data_ == 0, "No remaining data should be expected at this point");

            while (begin < end) {
                result result = consume(*begin++);

                if (result == result::good) {
                    // Find out how much data we should get
                    if (const auto data_length = request_.get_content_length(); data_length.has_value()) {
                        remaining_expected_data_ = *data_length;
                    } else {
                        remaining_expected_data_ = 0;
                    }

                    if (remaining_expected_data_ > 0) {
                        break;  // Break out of this loop into outer loop to consume data
                    }

                    RAV_ASSERT(begin == end, "Expecting no more data left at this point");

                    return std::make_tuple(result, begin);
                }

                if (result != result::indeterminate) {
                    return std::make_tuple(result, begin);  // Error
                }
            }
        }

        return std::make_tuple(result::indeterminate, begin);
    }

    /**
     * Resets the parser to its initial state. Also resets the pointed at request.
     */
    void reset() {
        state_ = state::method_start;
        remaining_expected_data_ = 0;
        request_.reset();
    }

  private:
    enum class state {
        method_start,
        method,
        uri,
        rtsp_r,
        rtsp_t,
        rtsp_s,
        rtsp_p,
        rtsp_slash,
        version_major,
        version_dot,
        version_minor,
        expecting_newline_1,
        header_start,
        header_name,
        space_before_header_value,
        header_value,
    };

    rtsp_request& request_;
    state state_ {state::method_start};
    long remaining_expected_data_ {0};

    result consume(char c);

    /// Check if a byte is an HTTP character.
    static bool is_char(int c);

    /// Check if a byte is an HTTP control character.
    static bool is_ctl(int c);

    /// Check if a byte is defined as an HTTP tspecial character.
    static bool is_tspecial(int c);

    /// Check if a byte is a digit.
    static bool is_digit(int c);
};

}  // namespace rav
