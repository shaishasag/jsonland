#ifndef __jsonland_json_node_h__
#define __jsonland_json_node_h__

#include <vector>
#include <unordered_map>
#include <string_view>
#include <string>
#include <iostream>
#include <sstream>
#include <charconv>
#include <algorithm>

/* Copy to #include
#include "jsonland/json_node.h"
*/

#ifndef JSONLAND_LIKELY
#if defined(__GNUC__) || defined(__clang__)
#define JSONLAND_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define JSONLAND_LIKELY(x) (x)
#endif
#endif
#ifndef JSONLAND_UNLIKELY
#if defined(__GNUC__) || defined(__clang__)
#define JSONLAND_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define JSONLAND_UNLIKELY(x) (x)
#endif
#endif

#include "string_or_view.h"

// #define JSONLAND_DEBUG

namespace jsonland
{


enum node_type : uint32_t
{
    uninitialized_t = 0,
    null_t = 1<<0,
    bool_t = 1<<1,
    number_t = 1<<2,
    string_t = 1<<3,
    array_t = 1<<4,
    object_t = 1<<5,
};


namespace parser_impl { class Parser; }

class json_node
{
public:

    friend std::ostream& operator<<(std::ostream &os, const json_node& jn);
    friend bool operator==(const json_node& lhs, const json_node& rhs);
    friend bool operator!=(const json_node& lhs, const json_node& rhs);

    // class string_or_view is a helper class that stores a string as either
    // - a reference to external memory (in member of type std::string_view)
    // OR
    // - internaly  (in member of type std::string)
    // in both cases a std::string_view always refers to the string
    // escape situation can be:
    // m_num_escapes == -1 - unknown
    // m_num_escapes == 0 - no escapes were found
    // m_num_escapes > 0 - some escapes were found

    
public:
    using KeyToIndex = std::unordered_map<string_or_view, int, string_or_view_hasher>;
    using ArrayVec = std::vector<json_node>;

//private:
    
    node_type m_node_type = null_t;
    mutable string_or_view m_value{the_null_string_view};
    double m_num = 0.0;

    ArrayVec m_values;
    KeyToIndex m_obj_key_to_index;
    mutable string_or_view m_key{the_empty_string_view};

    enum hints : uint32_t
    {
        hint_none = 0,
        _num_is_int = 1<<1,
    };
    hints m_hints = hint_none;

    static constexpr std::string_view the_empty_string_view{""};
    static constexpr std::string_view the_false_string_view{"false"};
    static constexpr std::string_view the_true_string_view{"true"};
    static constexpr std::string_view the_null_string_view{"null"};

    

private:

    template<typename NUM> using IsNum = std::enable_if_t<(std::is_integral<NUM>::value && !std::is_same<bool, NUM>::value && !std::is_same<char, NUM>::value) || std::is_floating_point<NUM>::value>;
    
    template<typename NUM> using IsInteger = std::enable_if_t<(std::is_integral<NUM>::value && !std::is_same<bool, NUM>::value && !std::is_same<char, NUM>::value) && !std::is_floating_point<NUM>::value>;
    
    template<typename NUM> using IsFloat = std::enable_if_t<std::is_floating_point<NUM>::value>;

    template<typename TBOOL> using IsBool = std::enable_if_t<std::is_same<bool, TBOOL>::value>;
    template<typename TCHAR> using IsChar = std::enable_if_t<std::is_same<char, TCHAR>::value>;
    template<typename TNULLPTR> using IsNullPtr = std::enable_if_t<std::is_same<nullptr_t, TNULLPTR>::value>;

    friend class parser_impl::Parser;

public:
    json_node() = default;
    ~json_node() = default;
    json_node(const json_node& in_node);
    json_node(json_node&& in_node);
    json_node& operator=(const json_node& in_node);
    json_node& operator=(json_node&& in_node);

    inline explicit json_node(jsonland::node_type in_type)
    : m_node_type(in_type)
    {
        switch (m_node_type)
        {
            case null_t:
                m_value = the_null_string_view;
            break;
            case number_t:
                m_value = the_empty_string_view;
            break;
            case bool_t:
                m_value = the_false_string_view;
            break;
            default:
                m_value = the_empty_string_view;
            break;
        }
        m_value.m_num_escapes = 0;
   }

    inline json_node& operator=(jsonland::node_type in_type)
    {
        clear(in_type);
        return *this;
    }

    json_node& operator=(const std::string_view in_str)
    {
        clear(string_t);
        m_value.reference_value(in_str);
        return *this;
    }

    json_node(const std::string& in_string)
    : m_node_type(string_t)
    {
        m_value.store_value_deal_with_escapes(in_string.c_str());
    }
    json_node& operator=(const std::string& in_string)
    {
        clear(string_t);
        m_value.store_value_deal_with_escapes(in_string.c_str());
        return *this;
    }

    template <typename TCHAR, IsChar<TCHAR>* = nullptr >
    explicit json_node(const TCHAR in_str[], jsonland::node_type in_type=string_t)
    : m_node_type(in_type)
    {
        m_value.store_value_deal_with_escapes(in_str);
    }
    template <typename TCHAR, IsChar<TCHAR>* = nullptr >
    json_node& operator=(const TCHAR in_str[])
    {
        clear(string_t);
        m_value.store_value_deal_with_escapes(in_str);
        return *this;
    }

    //--- integer constructor
    template <typename NUM, IsInteger<NUM>* = nullptr >
    json_node(const NUM in_num)
    : m_node_type(number_t)
    , m_value()
    , m_num(static_cast<double>(in_num))
    , m_hints(_num_is_int)
    {}
    // assign number
    template <typename NUM, IsInteger<NUM>* = nullptr >
    json_node& operator=(const NUM in_num)
    {
        clear(number_t);
        m_num = static_cast<double>(in_num);
        m_hints = static_cast<hints>(m_hints | _num_is_int);
        
        return *this;
    }
    //...
    //--- float constructor
    template <typename NUM, IsFloat<NUM>* = nullptr >
    json_node(const NUM in_num)
    : m_node_type(number_t)
    , m_value()
    , m_num(static_cast<double>(in_num))
    {}
    // assign number
    template <typename NUM, IsFloat<NUM>* = nullptr >
    json_node& operator=(const NUM in_num)
    {
        clear(number_t);
        m_num = static_cast<double>(in_num);
        m_hints = static_cast<hints>(m_hints & ~_num_is_int);

        return *this;
    }
    //...

    //--- bool constructor
    template <typename TBOOL, IsBool<TBOOL>* = nullptr >
    explicit json_node(const TBOOL in_bool)
    : m_node_type(bool_t)
    , m_value(in_bool ? the_true_string_view : the_false_string_view)
    {}
    // assign bool
    template <typename TBOOL, IsBool<TBOOL>* = nullptr >
    json_node& operator=(const TBOOL in_bool)
    {
        clear(bool_t);
        m_value.reference_value(in_bool ? the_true_string_view : the_false_string_view);
        return *this;
    }
    //...


    //--- null constructor
    template <typename TNULLPTR, IsNullPtr<TNULLPTR>* = nullptr >
    explicit json_node(TNULLPTR) : json_node()
    {}
    // assign null
    template <typename TNULLPTR, IsNullPtr<TNULLPTR>* = nullptr >
    json_node& operator=(TNULLPTR)
    {
        clear(null_t);
        return *this;
    }
    //...

    void clear(const node_type in_new_type=null_t)
    {
        m_node_type = in_new_type;
        m_obj_key_to_index.clear();
        m_values.clear();
        m_value.clear();
        switch (m_node_type)
        {
            case null_t:
                m_value = the_null_string_view;
            break;
            case number_t:
                m_value = the_empty_string_view;
            break;
            case bool_t:
                m_value = the_false_string_view;
            break;
            default:
                m_value = the_empty_string_view;
            break;
        }
        m_value.m_num_escapes = 0;
        m_num = 0.0;
    }


    inline jsonland::node_type type() const {return m_node_type;}
    inline bool is_type(jsonland::node_type in_type) {return in_type == m_node_type;}
    inline bool is_null() const {return null_t == m_node_type;}
    inline bool is_object() const {return object_t == m_node_type;}
    inline bool is_array() const {return array_t == m_node_type;}
    inline bool is_string() const {return string_t == m_node_type;}
    inline bool is_num() const {return number_t == m_node_type;}
    inline bool is_bool() const {return bool_t == m_node_type;}
    inline bool is_scalar() const {return is_string() || is_num() || is_bool();}
    inline bool is_valid() const {return is_scalar() || is_array() || is_object() || is_null();}

    size_t num_elements() const
    {
        size_t retVal = 0;
        switch (type())
        {
            case object_t: retVal = m_values.size(); break;
            case array_t: retVal = m_values.size(); break;
            case string_t: retVal = 1; break;  // one string, not the size of the string
            case number_t: retVal = 1; break;
            case bool_t: retVal = 1; break;
            case null_t:
            default:
                break;

        }
        return retVal;
    }

    size_t count(std::string_view in_key) const
    {
        size_t retVal = 0;
        if (is_object())
        {
            retVal = m_obj_key_to_index.count(in_key);
        }
        
        return retVal;
    }

    size_t memory_consumption() const;

    std::string   dump() const;
    std::ostream& dump(std::ostream& os) const;

    // return the string with escaped characters where needed
    inline const std::string_view as_string_view() const
    {
        return m_value.as_string_view();
    }
    
    const std::string_view as_resolved_string_view() const;
 
    std::string_view as_string(std::string_view in_default="") const
    {
        if (is_string())
        {
            if (0 != m_value.m_num_escapes) {
                m_value.unescape_internal();
            }
            return as_string_view();
        }
        else
            return in_default;
    }
    
    double as_double(const double in_default_fp=0.0) const
    {
        double retVal = in_default_fp;
        if (JSONLAND_LIKELY(is_num()))
        {
            if (is_number_assigned())
                return static_cast<double>(m_num);
            else
                retVal = std::atof(m_value.data());
        }

        return retVal;
    }

    template<typename TINT>
    TINT as_int(const TINT in_default_int=0) const
    {
        TINT retVal = in_default_int;
        if (JSONLAND_LIKELY(is_num()))
        {
            if (is_number_assigned())
                return static_cast<TINT>(m_num);
            else
                retVal = static_cast<TINT>(std::atoll(m_value.data()));
        }

        return retVal;
    }

    bool as_bool(const bool in_default_bool=false) const
    {
        if (JSONLAND_LIKELY(is_bool()))
            return m_value.data()[0] == 't';
        else
            return in_default_bool;
    }

    
    template<typename TASTYPE>
    TASTYPE as(const TASTYPE in_default={})
    {
        if constexpr (std::is_same<bool, TASTYPE>::value) {
            return as_bool(in_default);
        }
        else if constexpr (std::is_floating_point_v<TASTYPE>) {
            return as_double(in_default);
        }
        else if constexpr (std::is_integral_v<TASTYPE>) {
            return as_int(in_default);
        }
        else if (std::is_same<const char*, TASTYPE>::value) {
            return as_string(in_default);
        }
        else if (std::is_same<std::string_view, TASTYPE>::value) {
            return as_string(in_default);
        }
        else if constexpr (std::is_null_pointer_v<TASTYPE>) {
            return nullptr;
        }
    }
    
    std::string_view key() const
    {
        m_key.unescape_internal();
        return m_key.as_string_view().data();
    }
    
    KeyToIndex& get_key_to_index_map()
    {
        return m_obj_key_to_index;
    }

    const KeyToIndex& get_key_to_index_map() const
    {
        return m_obj_key_to_index;
    }

    ArrayVec& as_array()
    {
        return m_values;
    }

    const ArrayVec& as_array() const
    {
        return m_values;
    }

    void reserve(const size_t in_num_to_reserve)
    {
        if (JSONLAND_LIKELY(is_array() || is_object()))
        {
            m_values.reserve(in_num_to_reserve);
            if (is_object())
                m_obj_key_to_index.reserve(in_num_to_reserve);
        }
    }

//    json_node& operator[](const string_or_view& in_str)
//    {
//        if (JSONLAND_LIKELY(is_object()))
//        {
//            int index = -1;
//            string_or_view key(in_str);
//            if (0 == m_obj_key_to_index.count(key))
//            {
//                m_values.push_back({});
//                index = static_cast<int>(m_values.size() - 1);
//                m_obj_key_to_index[key] = index;
//            }
//            else
//            {
//                index = m_obj_key_to_index[key];
//            }
//            return m_values[index];
//        }
//        else
//            return *this;  // what to return here?
//    }

    json_node& operator[](std::string_view in_key)
    {
        if (JSONLAND_LIKELY(is_object()))
        {
            int index = -1;
            string_or_view key;
            key.store_value_deal_with_escapes(in_key);
            
            if (0 == m_obj_key_to_index.count(key))
            {
                index = static_cast<int>(m_values.size());
                json_node& value = m_values.emplace_back();
                value.m_key = std::move(key);

                m_obj_key_to_index[value.m_key] = index;
            }
            else
            {
                index = m_obj_key_to_index[key];
            }
            return m_values[index];
        }
        else
            return *this;  // what to return here?
    }

    const json_node& operator[](std::string_view in_str) const
    {
        if (JSONLAND_LIKELY(is_object()))
        {
            string_or_view key;
            key.reference_value(in_str);
            return m_values[m_obj_key_to_index.at(key)];
        }
        else
            return *this;  // what to return here?
    }


    json_node& push_back(const json_node& in_node)
    {
        m_values.push_back(in_node);
        return m_values.back();
    }

    template<typename TNODEVAL>
    json_node& push_back(const TNODEVAL in_val)
    {
        m_values.emplace_back(in_val);
        return m_values.back();
    }

    template <typename INT, IsInteger<INT>* = nullptr >
    json_node& operator[](const INT in_dex)
    {
        if (JSONLAND_LIKELY(is_array()))
        {
            return m_values.at(in_dex);
        }
        else
            return *this;  // what to return here?
    }

    template <typename INT, IsInteger<INT>* = nullptr >
    const json_node& operator[](INT in_dex) const
    {
        if (JSONLAND_LIKELY(is_array()))
        {
            return m_values.at(in_dex);
        }
        else
            return *this;  // what to return here?
    }

    template<typename TNode>
    class iterator_template
    {
    public:
        iterator_template(TNode& in_parent, const size_t in_index)
        : m_parent(in_parent)
        , m_index(in_index) {}
        
        TNode& m_parent;
        size_t     m_index = 0;
        
        iterator_template  operator++(int) /* postfix */         { ++m_index; return iterator_template(m_parent, m_index-1);  }
        iterator_template& operator++()    /* prefix */          { ++m_index; return *this; }
        TNode& operator* () const                    { return m_parent.m_values[m_index]; }
        TNode* operator->() const                    { return &m_parent.m_values[m_index]; }
        iterator_template  operator+ (int v)   const { return iterator_template(m_parent, m_index+v); }
        bool      operator==(const iterator_template& rhs) const { return &m_parent == &rhs.m_parent && m_index == rhs.m_index; }
        bool      operator!=(const iterator_template& rhs) const { return !(*this == rhs); }

        TNode& value() const                    { return m_parent.m_values[m_index]; }
        //json_node& key() const                    { return m_parent.m_obj_key_to_index[m_index]; }

    };
    
    using iterator = iterator_template<json_node>;
    using const_iterator = iterator_template<const json_node>;

    iterator begin()
    {
        return iterator(*this, 0);
    }
    
    iterator end()
    {
        return iterator(*this, m_values.size());
    }
    const_iterator begin() const
    {
        return const_iterator(*this, 0);
    }
    
    const_iterator end() const
    {
        return const_iterator(*this, m_values.size());
    }

protected:

    inline json_node(char* in_c_str, size_t in_str_size, jsonland::node_type in_type)
    : m_node_type(in_type)
    , m_value(in_str_size, in_c_str)
    {
    }
    inline json_node(const std::string_view in_string_view, jsonland::node_type in_type)
    : m_node_type(in_type)
    , m_value(in_string_view)
    {}

    // for parser use, *this is assumed to be freshly constructed, so no need to call clear
    inline void parser_direct_set(string_or_view&& in_str, jsonland::node_type in_type)
    {
        // add asserts that m_obj_key_to_index & m_values are empty
        m_node_type = in_type;
        m_value = in_str;
    }
    inline void parser_direct_set(char* in_c_str, size_t in_str_size, jsonland::node_type in_type)
    {
        // add asserts that m_obj_key_to_index & m_values are empty
        m_node_type = in_type;
        m_value.reference_value(in_c_str, in_str_size);
    }
    inline void parser_direct_set(jsonland::node_type in_type)
    {
        // add asserts that m_obj_key_to_index & m_values are empty
        m_node_type = in_type;
    }
    
private:
    // null, bool, and string are all represented in m_str_view, even when new value is assigned programatically.
    // the only place where there could be a value without string representation is when a number value is assigned
    // programatically (i.e. not when read from a string).
    // One solution could be to convert the number to text upon assignment, but this could be inefficiant if as_double()
    // is called - requiring the number to be converted again from text to binary form. Another inefficiantiancly could
    // occur in the same json_node is repeatadly assigned number values. The compramise is to store the number as it was
    // given (text when parsed, binary when assigned) and treat it accodingly.
    inline bool is_number_assigned() const {return m_value.empty();}
    inline bool is_string_assigned() const {return m_value.is_value_stored();}

};

class json_doc : public json_node
{
public:
    
    inline explicit json_doc(jsonland::node_type in_type) :  json_node(in_type) {}
    json_doc() = default;
    ~json_doc() = default;
//    json_doc(const json_node& in_node);
//    json_doc(json_node&& in_node);
//    json_doc& operator=(const json_node& in_node);
//    json_node& operator=(json_node&& in_node);

    
    inline json_doc& operator=(jsonland::node_type in_type)
    {
        json_node::operator=(in_type);
        return *this;
    }
    
    friend class jsonland::parser_impl::Parser;


    int parse_insitu(char* in_json_str, char* in_json_str_end);
    int parse_insitu(char* in_json_str, const size_t in_json_str_size);
    int parse_insitu(std::string& in_json_str);
    int parse_insitu(std::string_view in_json_str);

    int parse(const char* in_json_str, const char* in_json_str_end=nullptr);
    int parse(const char* in_json_str, const size_t in_json_str_size);
    int parse(const std::string& in_json_str);
    int parse(const std::string_view in_json_str);

    void set_max_nesting_level(const size_t in_nesting_level);
    void dump_tokens(std::ostream& os, char* in_json_str, char* in_json_str_end);

    int parse_error() { return m_parse_error; }
    std::string parse_error_message() { return m_parse_error_message; }
    
    size_t memory_consumption();

private:
    std::string m_json_text;
    size_t m_max_nesting_level = 64;
    int m_parse_error = 0;
    std::string m_parse_error_message;
};


} // namespace jsonland2

#endif // __jsonland_json_node_h__
