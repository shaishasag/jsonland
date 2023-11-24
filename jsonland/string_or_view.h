#ifndef __jsonland_variant_h__
#define __jsonland_variant_h__

#include <variant>

using namespace std::string_view_literals;

namespace jsonland
{
template<class> constexpr bool always_false_v = false;

class string_or_view
{
public:
    
    using value_type = std::variant<std::string_view, std::string>;
    value_type m_value;

//    std::string_view m_value{};
//    std::string m_internal_store{};
    int m_num_escapes = -1;
    
    string_or_view() = default;

    string_or_view(const std::string_view in_str, const int in_num_escapes=-1) noexcept
    {
        reference_value(in_str);
        m_num_escapes = in_num_escapes;
    }

    string_or_view(const string_or_view& in_sov)  = default;

    void store_value(const std::string& in_str, const int in_num_escapes=-1)
    {
        m_value = in_str;
        m_num_escapes = in_num_escapes;
    }

    void reference_value(const std::string_view in_str, const int in_num_escapes=-1)
    {
        m_value = in_str;
        m_num_escapes = in_num_escapes;
    }

    void convert_referenced_value_to_stored()
    {
        if (std::holds_alternative<std::string_view>(m_value))
        {
            m_value = std::string(std::get<std::string_view>(m_value));
        }
    }

    void store_value_deal_with_escapes(std::string_view in_str);
    
    // escape chars where needed and return true is escapes were found
    void unescape(std::string& out_unescaped) const;
    void unescape_internal();
    
    void clear()
    {
        store_value("");
        m_num_escapes = -1;
    }

    bool is_value_referenced() const { return std::holds_alternative<std::string_view>(m_value); }
    bool is_value_stored() const { return std::holds_alternative<std::string>(m_value); }
    bool empty() const
    {
        bool retVal{false};
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::is_same_v<T, std::string_view>)
                 retVal = arg.empty();
             else if constexpr (std::is_same_v<T, std::string>)
                 retVal = arg.empty();
             else
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
         }, m_value);
        
        return retVal;
    }

    const char* data() const
    {
        return  as_string_view().data();
    }

    std::string_view as_string_view() const
    {
        std::string_view retVal;
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::is_same_v<T, std::string_view>)
                 retVal = arg;
             else if constexpr (std::is_same_v<T, std::string>)
                 retVal = arg;
             else
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
         }, m_value);
        
        return retVal;
    }

    size_t size() const
    {
        size_t retVal{0};
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::is_same_v<T, std::string_view>)
                 retVal = arg.size();
             else if constexpr (std::is_same_v<T, std::string>)
                 retVal = arg.size();
             else
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
         }, m_value);
        
        return retVal;
    }
    friend struct string_or_view_hasher;
    friend bool operator==(const string_or_view& lhs, const string_or_view& rhs)
    {
        return lhs.m_value == rhs.m_value;
    }
    
    size_t allocation_size() const
    {
        size_t retVal{0};
        std::visit([&](auto&& arg)
        {
             using T = std::decay_t<decltype(arg)>;
             if constexpr (std::is_same_v<T, std::string_view>)
                 retVal = 0;
             else if constexpr (std::is_same_v<T, std::string>) {
                 if (arg.capacity() > sizeof(std::string)) {
                     retVal = arg.capacity()*sizeof(std::string::value_type);
                 }
             }
             else
                 static_assert(always_false_v<T>, "non-exhaustive visitor!");
         }, m_value);
        
        return retVal;
    }
};

struct string_or_view_hasher
{
    std::size_t operator()(const string_or_view& in_string_or_view_to_hash) const noexcept
    {
        return std::hash<std::string_view>()(in_string_or_view_to_hash.as_string_view());
    }
};

} // namespace jsonland


#endif // __jsonland_variant_h__
