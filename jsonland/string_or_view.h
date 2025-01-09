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


class DllExport string_or_view
{
public:
#if JSONLAND_DEBUG==1
    static size_t num_allocations;
#endif

    using value_type = std::variant<std::string_view, std::string>;
    value_type m_value{};

    int m_num_escapes{0};

    string_or_view() = default;
    string_or_view(const string_or_view& in_sov) = default;
    string_or_view(string_or_view&& in_sov) = default;
    string_or_view& operator=(const string_or_view& in_sov) = default;
    string_or_view& operator=(string_or_view&& in_sov) = default;

    string_or_view(const std::string_view in_str, const int in_num_escapes=0) noexcept
    {
        reference_value(in_str);
        m_num_escapes = in_num_escapes;
    }

    void store_value(const std::string& in_str, const int in_num_escapes=0) noexcept
    {
        m_value = in_str;
        m_num_escapes = in_num_escapes;
#if JSONLAND_DEBUG==1
    ++num_allocations;
#endif
    }

    void reference_value(const std::string_view in_str, const int in_num_escapes=0) noexcept
    {
        m_value = in_str;
        m_num_escapes = in_num_escapes;
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

    void store_value_deal_with_escapes(std::string_view in_str)  noexcept;


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
        m_num_escapes = 0;
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

    void dump_with_quotes(std::string& str) const noexcept
    {
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::same_as<T, std::string_view>)
             {
                 str += '"';
                 str.append(arg);
                 str += '"';
             }
             else if constexpr (std::same_as<T, std::string>)
             {
                 str += '"';
                 str.append(arg);
                 str += '"';
             }
             else
             {
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
             }
         }, m_value);

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

    void unescape()  noexcept
    {
        if (0 != m_num_escapes)
        {
#if JSONLAND_DEBUG==1
            if (is_value_referenced()) {  ++num_allocations; }
#endif
            convert_referenced_value_to_stored();
            unescape_internal(std::get<std::string>(m_value));
            m_num_escapes = 0;
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
