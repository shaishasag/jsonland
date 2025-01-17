#ifndef __escape_h__
#define __escape_h__

#include <string>
#include <string_view>
using namespace std::string_view_literals;

namespace jsonland
{
namespace escapism
{
namespace
{
std::int32_t parse_hex_digits(std::string_view digits)
{
    std::int32_t code_point{};

    for (const auto digit : digits)
    {
        code_point <<= 4;
        if (digit >= '0' and digit <= '9')
        {
            code_point |= (digit - '0');
        }
        else if (digit >= 'a' and digit <= 'f')
        {
            code_point |= (digit - 'a' + 10);
        }
        else if (digit >= 'A' and digit <= 'F')
        {
            code_point |= (digit - 'A' + 10);
        }
    }

    return code_point;
}

bool is_high_surrogate(std::int32_t code_point)
{
    return code_point >= 0xD800 && code_point <= 0xDBFF;
}

bool is_low_surrogate(std::int32_t code_point)
{
    return code_point >= 0xDC00 && code_point <= 0xDFFF;
}

void encode_utf8(std::int32_t code_point, std::string& rv)
{
    if (code_point <= 0x7F)
    {
    // Single-byte UTF-8
        rv += static_cast<char>(code_point & 0x7F);
    }
    else if (code_point <= 0x7FF)
    {
        // Two-byte UTF-8
        rv += static_cast<char>(0xC0 | ((code_point >> 6) & 0x1F));
        rv += static_cast<char>(0x80 | (code_point & 0x3F));
    }
    else if (code_point <= 0xFFFF)
    {
        // Three-byte UTF-8
        rv += static_cast<char>(0xE0 | ((code_point >> 12) & 0x0F));
        rv += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        rv += static_cast<char>(0x80 | (code_point & 0x3F));
    }
    else if (code_point <= 0x10FFFF)
    {
        // Four-byte UTF-8
        rv += static_cast<char>(0xF0 | ((code_point >> 18) & 0x07));
        rv += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
        rv += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        rv += static_cast<char>(0x80 | (code_point & 0x3F));
    }
}

enum unescape_result
{
    nothing_to_unescape,
    something_was_unescaped,
    error_during_unescape
};

/// @brief unescape_json_string is designed to efficiently process and unescape a JSON-formatted string,
///  represented as a std::string_view, into a std::string.
///  It avoids unnecessary allocations and copying if no escaped characters are found in the input string.
/// @param[in] sv The input JSON string to be unescaped.
/// @param[out] rv A reference to a std::string where the unescaped output will be stored.
/// The string is only modified if escaped characters are found in sv.
/// @return An unescape_result enum indicating the outcome of the operation:
///  #nothing_to_unescape: Returned if no escaped characters are found in sv. In this case, rv is not modified.
///  #something_was_unescaped: Returned if sv contains escaped characters, which are unescaped and appended to rv.
///  #error_during_unescape: Returned if an error occurs during unescaping, such as encountering invalid escape sequences or incomplete Unicode code points.
///
///  @note: since #rv is not changed if there are no escaped character caller must check the return value
///         and decide what to do with the string.
///         See usage in json_node::dump_tight() and string_and_view::unescape_json_string()
//
unescape_result unescape_json_string(std::string_view sv, std::string& rv)
{
    std::string::size_type index{0}; // current byte index in sv
    std::string::size_type length{sv.length()};
    while (index < length)
    {
        std::string::size_type jump{1};

        unsigned char byte = sv[index];
        if (byte == '\\')
        {
            break; // found something in need of escaping
        }
        if ((byte & 0x80) == 0)
        {
            // Single-byte code point
            if (byte <= 0x1F)
            {
                break;
            }
            // this is the normal unescaped case
        }
        else if ((byte & 0xE0) == 0xC0)
        {
            // Two-byte code point
            if (index + 1 > length)
            {
                return error_during_unescape;
            }
            jump = 2;
        }
        else if ((byte & 0xF0) == 0xE0)
        {
            // Three-byte code point
            if (index + 2 > length)
            {
                return error_during_unescape;
            }
            jump = 3;
        }
        else if ((byte & 0xF8) == 0xF0)
        {
            // Four-byte code point
            if (index + 3 > length)
            {
                return error_during_unescape;
            }
            jump = 4;
        }
        index += jump;
    }

    if (index > length) // code point was cut short
    {
        return error_during_unescape;
    }
    if (index == length)
    {
        return nothing_to_unescape;
    }

    // if we got here, there was some escaped char

    // copy the front characters that did not need escaping
    rv.append(sv, 0, index);

    std::int32_t code_point; // decoded \uXXXX or \uXXXX\uXXXX escape sequence

    while (index < length)
    {
        unsigned char byte = sv[index++];

        if (byte == '\\')
        {
            if (index < length)
            {
                byte = sv[index++];
            }
            else
            {
                return error_during_unescape;
            }

            switch (byte)
            {
                case '"': rv.push_back('"'); break;
                case '\\': rv.push_back('\\'); break;
                case '/': rv.push_back('/'); break;
                case 'b': rv.push_back('\b'); break;
                case 'f': rv.push_back('\f'); break;
                case 'n': rv.push_back('\n'); break;
                case 'r': rv.push_back('\r'); break;
                case 't': rv.push_back('\t'); break;
                case 'u':
                {
                    // Decode 4 hex digits.
                    if (index + 4 > length)
                    {
                        return error_during_unescape;
                    }

                    code_point = parse_hex_digits(sv.substr(index, 4));
                    index += 4;

                    if (is_low_surrogate(code_point))
                    {
                        return error_during_unescape;
                    }

                    if (is_high_surrogate(code_point))
                    {
                        if (!(index + 6 <= length && sv[index] == '\\' && sv[index + 1] == 'u'))
                        {
                            return error_during_unescape;
                        }

                        std::int32_t low_surrogate = parse_hex_digits(sv.substr(index + 2, 4));
                        index += 6;

                        if (!is_low_surrogate(low_surrogate))
                        {
                            return error_during_unescape;
                        }

                        // Combine high and low surrogates into a Unicode code point.
                        code_point = 0x10000 + (((code_point & 0x03FF) << 10) | (low_surrogate & 0x03FF));
                    }

                    encode_utf8(code_point, rv);
                }
                break;
                default:
                    return error_during_unescape;
            }
        }
        else
        {
            // Find invalid characters.
            // Bytes that are less than 0x1f and not a continuation byte.
            if ((byte & 0x80) == 0)
            {
                // Single-byte code point
                if (byte <= 0x1F)
                {
                    return error_during_unescape;
                }
                rv.push_back(byte); // <<< this is the normal unescaped case
            }
            else if ((byte & 0xE0) == 0xC0)
            {
                // Two-byte code point
                if (index + 1 > length)
                {
                    return error_during_unescape;
                }
                rv.push_back(byte);
                rv.push_back(sv[index++]);
            }
            else if ((byte & 0xF0) == 0xE0)
            {
                // Three-byte code point
                if (index + 2 > length)
                {
                    return error_during_unescape;
                }
                rv.push_back(byte);
                rv.push_back(sv[index++]);
                rv.push_back(sv[index++]);
            }
            else if ((byte & 0xF8) == 0xF0)
            {
                // Four-byte code point
                if (index + 3 > length)
                {
                    return error_during_unescape;
                }
                rv.push_back(byte);
                rv.push_back(sv[index++]);
                rv.push_back(sv[index++]);
                rv.push_back(sv[index++]);
            }
            else
            {
                return error_during_unescape;
            }
        }
    }

    return something_was_unescaped;
}

inline bool is_a_char_that_must_be_escaped(const char in_c)
{
    const bool retVal = '"' == in_c ||
                        '\\' == in_c ||
                        '\n' == in_c ||
                        '\r' == in_c ||
                        '\t' == in_c ||
                        '\b' == in_c ||
                        '\f' == in_c ;
    return retVal;
}

enum escape_result
{
    nothing_to_escape,
    something_was_escaped,
    error_during_escape
};

escape_result escape_json_string(std::string_view sv, std::string& rv)
{
    std::string::size_type index{0}; // current byte index in sv
    std::string::size_type length{sv.length()};
    while (index < length)
    {
        if (is_a_char_that_must_be_escaped(sv[index]))
        {
            break;
        }
        ++index;
    }
    if (index > length) // went too far
    {
        return error_during_escape;
    }
    if (index == length)
    {
        return nothing_to_escape;
    }

    // copy the front characters that did not need escaping
    rv.append(sv, 0, index);


    while (index < length)
    {
        char next_char = sv[index++];
        switch(next_char)
        {
            case '"':  rv+= "\\\""sv; break;
            case '\\': rv+= "\\\\"sv; break;
            case '\n': rv+= "\\n"sv; break;
            case '\r': rv+= "\\r"sv; break;
            case '\t': rv+= "\\t"sv; break;
            case '\b': rv+= "\\b"sv; break;
            case '\f': rv+= "\\f"sv; break;
                break;
            default:
                rv+= next_char;
                break;
        }
    }

    return something_was_escaped;
}

} // namespace
} // namespace escapism
} // namespace jsonland

#endif // __escape_h__
