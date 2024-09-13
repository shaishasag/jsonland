#ifndef __jsonland_variant_h__
#define __jsonland_variant_h__

#include <variant>

using namespace std::string_view_literals;

#ifndef DllExport
    #define DllExport
#endif

namespace jsonland
{
template<class> constexpr bool always_false_v = false;


class DllExport solve_escapes_iter
{
private:
    const std::string_view m_str;
    int m_str_i = -1;
    char m_curr_char = '\0';
    char m_waiting_char = '\0';

    size_t m_num_escapes_found = 0;

public:
    solve_escapes_iter(std::string_view in_str)
    : m_str(in_str)
    {
        if (!m_str.empty())
            operator++();
    }

    operator bool()
    {
        return m_str_i < static_cast<int>(m_str.size()) && '\0' != m_curr_char;
    }

    size_t num_escapes_found()
    {
        return m_num_escapes_found;
    }

    char hex2char(const char hi, const char lo)
    {
        uint8_t hi_t = (hi <= 57)? hi - 48 : (hi <= 70)? (hi - 65) + 0x0a : (hi - 97) + 0x0a;
        uint8_t lo_t = (lo <= 57)? lo - 48 : (lo <= 70)? (lo - 65) + 0x0a : (lo - 97) + 0x0a;
        return hi_t*16+lo_t;
    }


    solve_escapes_iter& operator++()
    {
        if (m_str_i >= static_cast<int>(m_str.size()) && m_str_i != -1)
            return *this;

        if ('\0' != m_waiting_char)
        {
            m_curr_char = m_waiting_char;
            m_waiting_char = '\0';
        }
        else
        {
            ++m_str_i;
            m_curr_char = m_str[m_str_i];

            if ('\\' == m_curr_char)
            {
                ++m_num_escapes_found;
                m_curr_char = m_str[++m_str_i];
                switch (m_curr_char)
                {
                    case '\\':
                    case '/':
                    case '"':
                        break;
                    case 'n': m_curr_char = '\n'; break;
                    case 'r': m_curr_char = '\r'; break;
                    case 't': m_curr_char = '\t'; break;
                    case 'b': m_curr_char = '\b'; break;
                    case 'f': m_curr_char = '\f'; break;
                    case 'u':
                    {
                        if (m_str_i+4 < static_cast<int>(m_str.size()))
                        {
                            char c1st = m_curr_char;
                            char c2nd = m_str[++m_str_i];
                            m_curr_char = hex2char(c1st, c2nd);
                            c1st = m_str[++m_str_i];
                            c2nd = m_str[++m_str_i];
                            m_waiting_char = hex2char(c1st, c2nd);
                        }
                        else
                        {
                            m_curr_char = '\0';
                            m_str_i = (int)m_str.size();
                        }
                    }
                }
            }
        }

        return *this;
    }

    char operator*()
    {
        return m_curr_char;
    }

};

class DllExport string_or_view
{
public:
#if JSONLAND_DEBUG==1
    static size_t num_allocations;
#endif

    using value_type = std::variant<std::string_view, std::string>;
    value_type m_value;

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

    void store_value(const std::string& in_str) noexcept
    {
        m_value = in_str;
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
             if constexpr (std::is_same_v<T, std::string_view>)
             {
                 arg = {};
             }
             else if constexpr (std::is_same_v<T, std::string>)
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
             if constexpr (std::is_same_v<T, std::string_view>)
             {
                 retVal = arg.empty();
             }
             else if constexpr (std::is_same_v<T, std::string>)
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
             if constexpr (std::is_same_v<T, std::string_view>)
             {
                 retVal = arg;
             }
             else if constexpr (std::is_same_v<T, std::string>)
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
             if constexpr (std::is_same_v<T, std::string_view>)
             {
                 str += '"';
                 str.append(arg);
                 str += '"';
             }
             else if constexpr (std::is_same_v<T, std::string>)
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
             if constexpr (std::is_same_v<T, std::string_view>)
             {
                 str.append(arg);
             }
             else if constexpr (std::is_same_v<T, std::string>)
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
             if constexpr (std::is_same_v<T, std::string_view>)
             {
                 retVal = arg.size();
             }
             else if constexpr (std::is_same_v<T, std::string>)
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
             if constexpr (std::is_same_v<T, std::string_view>)
             {
                 retVal = 0;
             }
             else if constexpr (std::is_same_v<T, std::string>)
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

    void unescape(std::string& out_unescaped) const  noexcept
    {
        out_unescaped.reserve(size());

        solve_escapes_iter iter(as_string_view());

        while (iter)
        {
            out_unescaped += *iter;
            ++iter;
        }
    }

    void unescape_internal()  noexcept
    {
        if (0 != m_num_escapes)
        {
            std::string temp_str(as_string_view());
            unescape(temp_str);
            m_num_escapes = 0;
            m_value = std::move(temp_str);
    #if JSONLAND_DEBUG==1
            ++num_allocations;
    #endif
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


#endif // __jsonland_variant_h__
