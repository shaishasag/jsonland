#ifndef __jsonland_string_or_view_h__
#define __jsonland_string_or_view_h__

#include <variant>
#include <cstdint>

using namespace std::string_view_literals;

#ifndef DllExport
    #define DllExport
#endif

namespace jsonland
{
template<class> constexpr bool always_false_v = false;

static inline char hex2char(const char hi, const char lo)
{
    uint8_t hi_t = (hi <= 57)? hi - 48 : (hi <= 70)? (hi - 65) + 0x0a : (hi - 97) + 0x0a;
    uint8_t lo_t = (lo <= 57)? lo - 48 : (lo <= 70)? (lo - 65) + 0x0a : (lo - 97) + 0x0a;
    return hi_t*16+lo_t;
}

static char32_t hexToCodepoint(std::string_view hex)
{
//    if (hex.size() != 4) {
//        throw std::invalid_argument("Hex string must be exactly 4 characters");
//    }

    char32_t codepoint = 0;

    for (char c : hex)
    {
        codepoint <<= 4; // Shift left by 4 bits to make room for the next hex digit
        if (c >= '0' && c <= '9')
        {
            codepoint += c - '0';
        }
        else if (c >= 'A' && c <= 'F')
        {
            codepoint += c - 'A' + 10;
        }
        else if (c >= 'a' && c <= 'f')
        {
            codepoint += c - 'a' + 10;
        }
//        else
//        {
//            throw std::invalid_argument("Invalid hex digit in Unicode escape");
//        }
    }

    return codepoint;
}

static void unicodeToUTF8(const char32_t codepoint, std::string& out_utf8)
{
    if (codepoint <= 0x7F)
    {
        out_utf8 += static_cast<char>(codepoint);
    }
    else if (codepoint <= 0x7FF)
    {
        out_utf8 += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
        out_utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    else if (codepoint <= 0xFFFF)
    {
        out_utf8 += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
        out_utf8 += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        out_utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    else if (codepoint <= 0x10FFFF)
    {
        out_utf8 += static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07));
        out_utf8 += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        out_utf8 += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        out_utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
//        else {
//        throw std::invalid_argument("Invalid Unicode code point");
//    }
}

[[maybe_unused]] static const char* escapable_to_escaped(const char* in_c, size_t& out_chars_to_copy)
{
    const char* retVal = in_c;
    out_chars_to_copy = 2;

    switch(*in_c)
    {
        case '"':  retVal = "\\\""; break;
        case '\\':  retVal = "\\\\"; break;
        case '\n': retVal = "\\n"; break;
        case '\r': retVal = "\\r"; break;
        case '\t': retVal = "\\t"; break;
        case '\b': retVal = "\\b"; break;
        case '\f': retVal = "\\f"; break;
        default:
            out_chars_to_copy = 1;
            break;
    }

    return retVal;
}


class DllExport string_or_view
{
public:
#if JSONLAND_DEBUG==1
    static size_t num_allocations;
#endif

    using value_type = std::variant<std::string_view, std::string>;
    value_type m_value{};

    enum source : char
    {
        parsed,             // string was parsed from json file or text, might contain chars escaped by backward slash
        assigned            // string was assigned progrmatically, chars are not escaped
    };
    source m_source{assigned};

    string_or_view() = default;
    string_or_view(const string_or_view& in_sov) = default;
    string_or_view(string_or_view&& in_sov) = default;
    string_or_view& operator=(const string_or_view& in_sov) = default;
    string_or_view& operator=(string_or_view&& in_sov) = default;

    string_or_view(const std::string_view in_str, const source in_source=assigned) noexcept
    {
        reference_value(in_str);
        m_source = in_source;
    }

    void store_value(const char* in_str, const source in_source=assigned) noexcept
    {
        m_value = std::string(in_str);
        m_source = in_source;
#if JSONLAND_DEBUG==1
    ++num_allocations;
#endif
    }
    void store_value(std::string_view in_str, const source in_source=assigned) noexcept
    {
        m_value = std::string(in_str);
        m_source = in_source;
#if JSONLAND_DEBUG==1
    ++num_allocations;
#endif
    }

    void store_value(std::string&& in_str, const source in_source=assigned) noexcept
    {
        m_value = std::move(in_str);
        m_source = in_source;
#if JSONLAND_DEBUG==1
    ++num_allocations;
#endif
    }

    void reference_value(const std::string_view in_str, const source in_source=assigned) noexcept
    {
        m_value = in_str;
        m_source = in_source;
    }

    void convert_referenced_value_to_stored() noexcept
    {
        if (std::holds_alternative<std::string_view>(m_value))
        {
            m_value = std::string(std::get<std::string_view>(m_value));
#if JSONLAND_DEBUG==1
    ++num_allocations;
#endif
        }
    }

    //void store_value_deal_with_escapes(std::string_view in_str)  noexcept;


    void clear() noexcept
    {
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::same_as<T, std::string_view>)
             {
                 arg = {};
             }
             else if constexpr (std::same_as<T, std::string>)
             {
                 arg.clear();
             }
             else
             {
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
             }
         }, m_value);
        m_source = assigned;
    }

    bool is_value_referenced() const  noexcept { return std::holds_alternative<std::string_view>(m_value); }
    bool is_value_stored() const  noexcept { return std::holds_alternative<std::string>(m_value); }
    bool empty() const  noexcept
    {
        bool retVal{false};
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::same_as<T, std::string_view>)
             {
                 retVal = arg.empty();
             }
             else if constexpr (std::same_as<T, std::string>)
             {
                 retVal = arg.empty();
             }
             else
             {
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
             }
         }, m_value);

        return retVal;
    }

    const char* data() const noexcept
    {
        return  as_string_view().data();
    }

    std::string_view as_string_view() const noexcept
    {
        std::string_view retVal;
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::same_as<T, std::string_view>)
             {
                 retVal = arg;
             }
             else if constexpr (std::same_as<T, std::string>)
             {
                 retVal = arg;
             }
             else
             {
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
             }
         }, m_value);

        return retVal;
    }

    void dump_with_quotes(std::string& out_str) const noexcept
    {
        out_str.reserve(out_str.size()+size()+2);
        
        out_str += '"';
        dump(out_str);
        out_str += '"';
    }

    void dump_no_quotes(std::string& str) const noexcept
    {
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::same_as<T, std::string_view>)
             {
                 str.append(arg);
             }
             else if constexpr (std::same_as<T, std::string>)
             {
                 str.append(arg);
             }
             else
             {
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
             }
         }, m_value);

    }

    size_t size() const noexcept
    {
        size_t retVal{0};
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::same_as<T, std::string_view>)
             {
                 retVal = arg.size();
             }
             else if constexpr (std::same_as<T, std::string>)
             {
                 retVal = arg.size();
             }
             else
             {
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
             }
         }, m_value);

        return retVal;
    }

    friend bool operator==(const string_or_view& lhs, const string_or_view& rhs) noexcept
    {
        bool retVal = lhs.as_string_view() == rhs.as_string_view();
        return retVal;
    }

    /// @return Estimation of amount of heap memory allocated by this object, not including the object itself.
    /// <br>If holding std::string, will try to take into account "small string optimization",
    /// in which case 0 will be reported.
    /// <br>If holding std::string_view, 0 will be reported.
    size_t allocation_size() const noexcept
    {
        size_t retVal{0};
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::same_as<T, std::string_view>)
             {
                 retVal = 0;
             }
             else if constexpr (std::same_as<T, std::string>)
             {
                 if (arg.capacity() > sizeof(std::string)) {
                     retVal = arg.capacity()*sizeof(std::string::value_type);
                 }
             }
             else
             {
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
             }
         }, m_value);

        return retVal;
    }

    // dump the string to json text escaping special chars if needed
    void dump(std::string& out_str) const
    {
        std::string_view original_str = as_string_view();
        if (parsed == m_source) // parsed: no need to escape
        {
            out_str += original_str;
        }
        else // string was assigned, need to escape chars, e.g. '\n'  -> "\n"
        {
            size_t original_size = original_str.size();
            for (size_t i = 0; i < original_size; ++i)
            {
                char next_char = original_str[i];
                switch(next_char)
                {
                    case '"':  out_str+= "\\\""; break;
                    case '\\': out_str+= "\\\\"; break;
                    case '\n': out_str+= "\\n"; break;
                    case '\r': out_str+= "\\r"; break;
                    case '\t': out_str+= "\\t"; break;
                    case '\b': out_str+= "\\b"; break;
                    case '\f': out_str+= "\\f"; break;
                        break;
                    default:
                        out_str+= next_char;
                        break;
                }
            }
        }
    }

    // write the string unescaping special chars if needed
    void print(std::string& out_str) const
    {
        if (assigned == m_source) // string was assigned: no need to unescape
        {
            out_str += as_string_view();
        }
        else // string was parsed, escaped chars should be unescaped, e.g. "\n" -> '\n'
        {
            std::string_view original_str = as_string_view();
            size_t original_size = original_str.size();
            for (size_t i = 0; i < original_size; ++i)
            {
                char next_char = original_str[i];
                if ('\\' == next_char) [[unlikely]]
                {
                    if (i < original_size - 1) [[likely]]
                    {
                        next_char = original_str[++i];
                    }
                    else [[unlikely]]
                    {
                        break; // should not happen string should not end with single backslash
                    }

                    switch (next_char)
                    {
                        case '\\':
                        case '/':
                        case '"':
                            out_str += next_char;
                        break;
                        case 'n': out_str += '\n'; break;
                        case 'r': out_str += '\r'; break;
                        case 't': out_str += '\t'; break;
                        case 'b': out_str += '\b'; break;
                        case 'f': out_str += '\f'; break;
                        case 'u':
                        {
                            if (i+4 < original_size) [[likely]]
                            {
                                std::string_view hex = original_str.substr(i+1, 4);
                                char32_t codepoint = hexToCodepoint(hex);
                                unicodeToUTF8(codepoint, out_str);
                            }
                            i += 4;
                        }
                        break;
                    }
                }
                else
                {
                    out_str += next_char;
                }
            }
        }
    }

#if 0
    void unescape_internal(std::string& out_unescaped)   noexcept
    {
        size_t original_size = out_unescaped.size();
        size_t i_unescaped = 0;
        for (size_t i = 0; i < original_size; ++i)
        {
            char next_char = out_unescaped[i];
            if ('\\' == next_char)
            {
                next_char = out_unescaped[++i];
                switch (next_char)
                {
                    case '\\':
                    case '/':
                    case '"':
                        break;
                    case 'n': next_char = '\n'; break;
                    case 'r': next_char = '\r'; break;
                    case 't': next_char = '\t'; break;
                    case 'b': next_char = '\b'; break;
                    case 'f': next_char = '\f'; break;
                    case 'u':
                    {
                        if (i+4 < original_size)
                        {
                            char c1st = next_char;
                            char c2nd = out_unescaped[++i];
                            next_char = hex2char(c1st, c2nd);
                            out_unescaped[i_unescaped++] = next_char;

                            c1st = out_unescaped[++i];
                            c2nd = out_unescaped[++i];
                            next_char = hex2char(c1st, c2nd);
                        }
                        else
                        {
                            i += 4;
                        }
                    }
                }
            }
            out_unescaped[i_unescaped++] = next_char;
        }
        out_unescaped.resize(i_unescaped);
    }
#endif

    void unescape()  noexcept
    {
        if (parsed == m_source)
        {
#if JSONLAND_DEBUG==1
            if (is_value_referenced()) {  ++num_allocations; }
#endif
            convert_referenced_value_to_stored();
            std::string unescaped;
            unescaped.reserve(size());
            print(unescaped); // can we do it inplace?
            store_value(std::move(unescaped));
        }
    }
};

struct string_or_view_hasher
{
    std::size_t operator()(const string_or_view& in_string_or_view_to_hash) const noexcept
    {
        std::size_t the_hash = std::hash<std::string_view>()(in_string_or_view_to_hash.as_string_view());
        return the_hash;
    }
};

} // namespace jsonland


#endif // __jsonland_string_or_view_h__
