#ifndef __jsonland_string_or_view_h__
#define __jsonland_string_or_view_h__

using namespace std::string_view_literals;
#include "string_variant.h"
namespace jsonland
{

#if 0
class string_or_view
{
public:
    
    std::string_view m_value{};
    std::string m_internal_store{};
    int m_num_escapes = -1;
    
    string_or_view() = default;
#if 0
    string_or_view(const char* in_str, const int in_num_escapes=-1) noexcept
    : m_internal_store()
    , m_value(in_str)
    , m_num_escapes(in_num_escapes)
    {
    }
    string_or_view(const size_t in_str_size, const char* in_str, const int in_num_escapes=-1) noexcept
    : m_internal_store()
    , m_value(in_str, in_str_size)
    , m_num_escapes(in_num_escapes)
    {
    }
#endif
    string_or_view(const std::string_view in_str, const int in_num_escapes=-1) noexcept
    {
        reference_value(in_str);
        m_num_escapes = in_num_escapes;
    }
#if 0
    string_or_view(const std::string& in_str, const int in_num_escapes=-1) noexcept
    {
        store_value(in_str);
        m_num_escapes = in_num_escapes;
    }
#endif

    string_or_view(const string_or_view& in_sov) noexcept
    {
        if (in_sov.is_value_referenced())
        {
            reference_value(in_sov.m_value);
        }
        else
        {
            store_value(in_sov.m_internal_store);
        }
        m_num_escapes = in_sov.m_num_escapes;
    }

    void store_value(const std::string& in_str, const int in_num_escapes=-1)
    {
        m_internal_store = in_str;
        m_value = m_internal_store;
        m_num_escapes = in_num_escapes;
    }
#if 0
    void store_value(std::string&& in_str, const int in_num_escapes=-1)
    {
        m_internal_store = std::move(in_str);
        m_value = m_internal_store;
        m_num_escapes = in_num_escapes;
    }
    void store_value(const std::string_view in_str, const int in_num_escapes=-1)
    {
        m_internal_store = in_str;
        m_value = m_internal_store;
        m_num_escapes = in_num_escapes;
    }
    
    void store_value(const char* in_str, const int in_num_escapes=-1)
    {
        m_internal_store = in_str;
        m_value = m_internal_store;
        m_num_escapes = in_num_escapes;
    }
    
    constexpr void reference_value(const std::string& in_str, const int in_num_escapes=-1)
    {
        m_internal_store.clear();
        m_value = std::string_view(in_str);
        m_num_escapes = in_num_escapes;
    }
#endif
    void reference_value(const std::string_view in_str, const int in_num_escapes=-1)
    {
        m_internal_store.clear();
        m_value = in_str;
        m_num_escapes = in_num_escapes;
    }
#if 0
    void reference_value(const string_or_view in_str)
    {
        m_internal_store.clear();
        m_value = in_str.m_value;
        m_num_escapes = in_str.m_num_escapes;
    }
    void reference_value(const char* in_str, const int in_num_escapes=-1)
    {
        m_internal_store.clear();
        m_value = std::string_view(in_str);
        m_num_escapes = in_num_escapes;
    }
    void reference_value(const char* in_str, size_t in_str_size, const int in_num_escapes=-1)
    {
        m_internal_store.clear();
        m_value = std::string_view(in_str, in_str_size);
        m_num_escapes = in_num_escapes;
    }
#endif
    
    void convert_referenced_value_to_stored()
    {
        m_internal_store = m_value;
        m_value = std::string_view(m_internal_store);
    }

#if 0
    string_or_view& operator=(const string_or_view& in_sov)
    {
        m_internal_store.clear();
        if (in_sov.is_value_referenced())
        {
            reference_value(in_sov.m_value);
        }
        else
        {
            store_value(in_sov.m_internal_store);
        }
        
        m_num_escapes = in_sov.m_num_escapes;
        return *this;
    }
    
    string_or_view(string_or_view&& in_sov) noexcept
    {
        if (in_sov.is_value_referenced())
        {
            reference_value(in_sov.m_value);
        }
        else
        {
            m_internal_store = std::move(in_sov.m_internal_store);
            m_value = m_internal_store;
        }
        m_num_escapes = in_sov.m_num_escapes;
    }
    
    string_or_view& operator=(string_or_view&& in_sov)
    {
        m_internal_store.clear();
        if (in_sov.is_value_referenced())
        {
            reference_value(in_sov.m_value);
        }
        else
        {
            m_internal_store = std::move(in_sov.m_internal_store);
            m_value = m_internal_store;
        }
        
        m_num_escapes = in_sov.m_num_escapes;
        return *this;
    }
#endif
    void store_value_deal_with_escapes(std::string_view in_str);
    
    // escape chars where needed and return true is escapes were found
    void unescape(std::string& out_unescaped) const;
    void unescape_internal();
    
    void clear()
    {
        store_value("");
        m_num_escapes = -1;
    }
    
    bool is_value_referenced() const { return m_internal_store.empty(); }
    bool is_value_stored() const { return !is_value_referenced(); }
    bool empty() const { return m_value.empty(); }
    const char* data() const { return m_value.data(); }
    std::string_view as_string_view() const { return m_value; }
    size_t size() { return m_value.size(); }
    friend struct string_or_view_hasher;
    friend bool operator==(const string_or_view& lhs, const string_or_view& rhs)
    {
        return lhs.m_value == rhs.m_value;
    }
    
    size_t allocation_size() const
    {
        size_t retVal{0};
        if (m_internal_store.capacity() > sizeof(std::string)) // std::string has small string optimization
        {
            retVal += m_internal_store.capacity()*sizeof(std::string::value_type);
        }
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
#endif
} // namespace jsonland


#endif // __jsonland_string_or_view_h__
