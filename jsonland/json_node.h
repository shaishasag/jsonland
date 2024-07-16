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

#ifdef JSONLAND_ASSERT_TYPE_CONSISTENCY
#undef JSONLAND_ASSERT_TYPE_CONSISTENCY
#endif
#define JSONLAND_ASSERT_TYPE_CONSISTENCY 1

namespace jsonland
{

/// JSON value type
enum value_type : uint32_t
{
    /// JSON value type was not set
    uninitialized_t = 0,
    /// JSON value type is null
    null_t = 1<<0,
    /// JSON value type is boolean
    bool_t = 1<<1,
    /// JSON value type is a number
    number_t = 1<<2,
    /// JSON value type is a string
    string_t = 1<<3,
    /// JSON value type is an array
    array_t = 1<<4,
    /// JSON value type is an object
    object_t = 1<<5,
};


namespace parser_impl { class Parser; }

class json_node
{
public:

    /// Converts JSON to text representation and writes it to std::ostream.
    /// @param[out] os the stream to write to.
    /// @param[in] jn the JSON to dump.
    /// @return the stream #os
    friend std::ostream& operator<<(std::ostream& os, const json_node& jn);

    /// Decide if two #json_node object are equal.
    /// @param[in] lhs the first #json_node to compare.
    /// @param[in] rhs the second #json_node to compare.
    /// @return true if the two given #json_node's values are identical, i.e. both have same JSON value type and same value. <br> return false otherwise.
    friend bool operator==(const json_node& lhs, const json_node& rhs);

    /// Decide if two #json_node object are not equal.
    /// @param[in] lhs the first #json_node to compare.
    /// @param[in] rhs the second #json_node to compare.
    /// @return false if the two given #json_node's are identical, true otherwise
    friend bool operator!=(const json_node& lhs, const json_node& rhs);

    // class string_or_view is a helper class that stores a string as either
    // - a reference to external memory (in member of type std::string_view)
    // OR
    // - internaly  (in member of type std::string)
    // in both cases a std::string_view always refers to the string
    // escape situation can be:
    // m_num_escapes == 0 - no escapes were found
    // m_num_escapes > 0 - some escapes were found


public:
    /// Type declaration of mapping of a string to the index of a value in #m_values.
    using KeyToIndex = std::unordered_map<string_or_view, int, string_or_view_hasher>;
    /// Type declaration for a vector of #json_nodes.
    using ArrayVec = std::vector<json_node>;

private:

    /// JSON value type of this instance, can be any of the values in enum #value_type.
    /// Initilay it is set to #null_t
    value_type m_value_type{null_t};
    /// String representation of the JSON value.
    mutable string_or_view m_value{the_null_string_view};
    /// Holds the JSON value if the value type is #number_t - but only when the number was changed from code.
    /// If the number value was parsed from string, #m_value will hold the number as text
    /// and #m_num will <b>not be assigned</b>.
    /// This will delay the conversion to binary number until if and when the number is actually needed.
    double m_num = 0.0;

    /// List of values when #m_value_type is #array_t or #object_t.
    /// Values are kept in the order they were parsed or programatically added to the object or array.
    ArrayVec m_values;

    /// Mapping of keys to the JSON values in #m_values. Only relevant if #m_value_type is #object_t.
    KeyToIndex m_obj_key_to_index;

    /// The key of this JSON value in case it is a part of another object.
    mutable string_or_view m_key{the_empty_string_view};

    enum hints : uint32_t
    {
        _hint_none = 0,
        _num_is_int = 1<<0,
        _num_in_string = 1<<1,
    };

    hints m_hints{_hint_none};

    /// Default value for a string.
    static constexpr std::string_view the_empty_string_view{""};
    /// String representation of a boolean with value 'false'.
    static constexpr std::string_view the_false_string_view{"false"};
    /// String representation of a boolean with value 'true'.
    static constexpr std::string_view the_true_string_view{"true"};
    /// String representation of a null value.
    static constexpr std::string_view the_null_string_view{"null"};


    /// template to decide if a type is a number (floating point or integer)
    template<typename NUM> using IsNum = std::enable_if_t<(std::is_integral<NUM>::value && !std::is_same<bool, NUM>::value && !std::is_same<char, NUM>::value) || std::is_floating_point<NUM>::value>;

    /// template to decide if a type is an integer
    template<typename NUM> using IsInteger = std::enable_if_t<(std::is_integral<NUM>::value && !std::is_same<bool, NUM>::value && !std::is_same<char, NUM>::value) && !std::is_floating_point<NUM>::value>;

    /// template to decide if a type is a floating point
    template<typename NUM> using IsFloat = std::enable_if_t<std::is_floating_point<NUM>::value>;

    /// template to decide if a type is a boolean
    template<typename TBOOL> using IsBool = std::enable_if_t<std::is_same<bool, TBOOL>::value>;

    /// template to decide if a type is a char
    template<typename TCHAR> using IsChar = std::enable_if_t<std::is_same<char, TCHAR>::value>;

    /// template to decide if a type is null
    template<typename TNULLPTR> using IsNullPtr = std::enable_if_t<std::is_same<nullptr_t, TNULLPTR>::value>;

    friend class parser_impl::Parser;

public:
    /// Default constructor.
    /// JSON value type (#m_value_type) is set to #null_t.
    json_node() noexcept = default;
    /// Default destructor.
    ~json_node() = default;

    /// Copy constructor
    /// @param in_node the #json_node to copy
    json_node(const json_node& in_node) noexcept = default;

    /// Move constructor
    /// @param in_node the #json_node to move
    json_node(json_node&& in_node) noexcept = default;

    /// Copy assignment
    /// @param in_node the #json_node to copy
    json_node& operator=(const json_node& in_node) noexcept;

    /// Move assignment
    /// @param in_node the #json_node to move
    json_node& operator=(json_node&& in_node) noexcept;

    /// Constructor by value_type
    /// @param in_type the JSON value type this instance will have.
    /// @in_reserve num of reserved places in if in_type os object_t or array_t
    /// <br>Value is set to the default value which is:
    /// Value type  | Default value
    /// ----------- | -------------
    /// null_t      | null
    /// bool_t      | false
    /// number_t    | 0.0
    /// string_t    | ""
    /// array_t     | []
    /// object_t    | {}
    inline explicit json_node(jsonland::value_type in_type, size_t in_reserve=0) noexcept
    : m_value_type(in_type)
    {
        switch (m_value_type)
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
            case object_t:
                reserve(in_reserve);
            break;
            case array_t:
                reserve(in_reserve);
            break;
            default:
                m_value = the_empty_string_view;
            break;
        }
   }

    /// Assign a new #value_type with a default value.
    /// @param in_type the new #value_type to set.<br>
    /// If in_type is the same as the currently assigned type, <B>the value will not change</B>.<br>
    /// To clear the value without changing the type call #clear()<br>
    /// <br>
    /// If in_type is different from the currently assigned type, <B>previous value is erased</B>, and:<br>
    /// If in_type == #string_t, value will be set to the empty string.<br>
    /// If in_type == #bool_t, value will be set to false.<br>
    /// If in_type == #number_t, value will be set to 0.0 and the number type is floating point.<br>
    /// If in_type == #array_t, value will be set to empty array.<br>
    /// If in_type == #object_t, value will be set to empty object.<br>
    /// If in_type == #null_t, value will be set to null.<br>
    inline json_node& operator=(jsonland::value_type in_type) noexcept
    {
        if (in_type != m_value_type) {
            clear(in_type);
        }
        return *this;
    }

    /// Assign a new value string, JSON value type is set to #string_t
    /// @param in_str_value the new string value.<br>
    /// Life time note: since in_str_value is a std::string_view, life time of this #json_node depends on the life time of #in_str_value.
    json_node& operator=(const std::string_view in_str_value) noexcept
    {
        clear(string_t);
        m_value.reference_value(in_str_value);
        return *this;
    }


    /// Constructor with string value, but #value_type to set to #in_type not necessarily #string_t.
    /// @param in_str_value the new string value.
    /// @param in_type the #value_type.
    /// Example:<br>
    /// @code
    /// jsonland::json_node n1("1234", jsonland::number_t);
    /// @codeend
    inline json_node(const std::string_view in_str_value, jsonland::value_type in_type) noexcept
    : m_value_type(in_type)
    , m_value(in_str_value)
    {
        if (m_value_type == number_t) {
            m_hints = json_node::_num_in_string;
            // (C++23): in_str_value.contains('.')
            if (auto pos = in_str_value.find('.'); pos==std::string_view::npos)
            {
                m_hints = static_cast<hints>(m_hints | json_node::_num_is_int);
            }
        }
    }

    /// Construct with std::string_view, #value_type to set to #string_t
    /// @param in_str_value the new string value.<br>
    /// Efficiency note: a copy of in_str_value will be created, requiring an allocation.
    json_node(const std::string_view in_str_value) noexcept
    : m_value_type(string_t)
    {
        m_value.store_value_deal_with_escapes(in_str_value);
    }
    /// Assign a new value from std::string, #value_type to set to #string_t
    /// @param in_str_value the new string value.<br>
    /// Efficiency note: a copy of in_str_value will be created, requiring an allocation (most likely).
    json_node& operator=(const std::string& in_str_value) noexcept
    {
        clear(string_t);
        m_value.store_value_deal_with_escapes(in_str_value);
        return *this;
    }

    /// Assign a new value from std::string_view, but #value_type set to #in_type, not necessarily #string_t.
    /// Note: Calling with the string value mismatched with the #value_type could lead to inconsitant state:
    /// @code
    /// json_node n1("abc", number_t); // n1 is a number, value is 0
    /// @codeend
   /// @param in_str the new value.<br>
    /// Example:<br>
    /// @code
    /// json_node n1("123", number_t);
    /// n1.is_number();  // true, it's a number intialized with a string
    /// n1.get_int<int32_t>() == 123;  // true, value is now 123
    /// @codeend
    template <typename TCHAR, IsChar<TCHAR>* = nullptr >
    explicit json_node(std::string in_str, jsonland::value_type in_type) noexcept
    : m_value_type(in_type)
    {
        m_value.store_value_deal_with_escapes(in_str);
        if (number_t == in_type) {
            m_hints = _num_in_string;
        }
    }

    /// Assign a new value from a string, #value_type to set to #string_t.
    /// @param in_str the new value.<br>
    /// Example:<br>
    /// @code
    /// json_node n1("123");
    /// n1.is_number();  // false, it's not a number it's a string
    /// n1.get_int<int32_t>() == 123;  // false, value is the string "123"
    /// @codeend
    template <typename TCHAR, IsChar<TCHAR>* = nullptr >
    json_node& operator=(const TCHAR in_str[]) noexcept
    {
        clear(string_t);
        m_value.store_value_deal_with_escapes(in_str);
        return *this;
    }

    //--- integer constructor
    template <typename NUM, IsInteger<NUM>* = nullptr >
    json_node(const NUM in_num) noexcept
    : m_value_type(number_t)
    , m_value()
    , m_num(static_cast<double>(in_num))
    , m_hints(_num_is_int)
    {}

    // assign number
    template <typename NUM, IsInteger<NUM>* = nullptr >
    json_node& operator=(const NUM in_num) noexcept
    {
        clear(number_t);
        m_num = static_cast<double>(in_num);
        m_hints = _num_is_int;

        return *this;
    }

    //--- float constructor
    template <typename NUM, IsFloat<NUM>* = nullptr >
    json_node(const NUM in_num) noexcept
    : m_value_type(number_t)
    , m_value()
    , m_num(static_cast<double>(in_num))
    {}
    // assign number
    template <typename NUM, IsFloat<NUM>* = nullptr >
    json_node& operator=(const NUM in_num) noexcept
    {
        clear(number_t);
        m_num = static_cast<double>(in_num);

        return *this;
    }

    //--- bool constructor
    template <typename TBOOL, IsBool<TBOOL>* = nullptr >
    explicit json_node(const TBOOL in_bool) noexcept
    : m_value_type(bool_t)
    , m_value(in_bool ? the_true_string_view : the_false_string_view)
    {}
    // assign bool
    template <typename TBOOL, IsBool<TBOOL>* = nullptr >
    json_node& operator=(const TBOOL in_bool) noexcept
    {
        clear(bool_t);
        m_value.reference_value(in_bool ? the_true_string_view : the_false_string_view);
        return *this;
    }

    //--- null constructor
    template <typename TNULLPTR, IsNullPtr<TNULLPTR>* = nullptr >
    explicit json_node(TNULLPTR) noexcept : json_node()
    {}
    // assign null
    template <typename TNULLPTR, IsNullPtr<TNULLPTR>* = nullptr >
    json_node& operator=(TNULLPTR)
    {
        clear(null_t);
        return *this;
    }

    /// Clears the JSON value, deallocate child elemets, if any, and sets the value type to the given param.
    /// @param in_new_type the new type to set. Deafults to #null_t.
    ///
    void clear(const value_type in_new_type=null_t) noexcept
    {
        m_value_type = in_new_type;
        m_obj_key_to_index.clear();
        m_values.clear();
        //m_value.clear();
        switch (m_value_type)
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

        m_num = 0.0;
        m_hints = _hint_none;
    }


    /// Returns the #value_type of this instance
    inline jsonland::value_type value_type() const noexcept {return m_value_type;}

    /// Returns true if and only if the JSON value type is #null_t.
    inline bool is_null() const noexcept {return null_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #object_t.
    inline bool is_object() const noexcept {return object_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #array_t.
    inline bool is_array() const noexcept {return array_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #string_t.
    inline bool is_string() const noexcept {return string_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #number_t (float or integer).
    inline bool is_number() const noexcept {return number_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #number_t, and the value is an integer (signed or unsigned).
    inline bool is_int() const noexcept {return number_t == m_value_type && (m_hints & _num_is_int);}
    /// Returns true if and only if the JSON value type is #number_t, and the value is floating point (float, double).
    inline bool is_float() const noexcept {return number_t == m_value_type && (0==(m_hints & _num_is_int));}
    /// Returns true if and only if the JSON value type is #bool_t.
    inline bool is_bool() const noexcept {return bool_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #bool_t, #number_t or #string_t.
    inline bool is_scalar() const noexcept {return is_string() || is_number() || is_bool();}
    /// Returns true if and only if the JSON value is set, i.e. not #uninitialized_t.
    inline bool is_valid() const noexcept {return is_scalar() || is_array() || is_object() || is_null();}
    /// Returns true if and only if the JSON type is structured (#array_t or #object_t).
    inline bool is_structured() const noexcept { return is_array() || is_object(); }

    /// Returns true if and only if the JSON type is the same as given in param #in_type
    /// @param[in] check_for_type the type in question.
    inline bool is_type(jsonland::value_type check_for_type) const noexcept {return check_for_type == m_value_type;}

#if 0
    template<typename TISTYPE>
    bool is_type() const noexcept
    {
        if constexpr (std::is_same<bool, TISTYPE>::value) {
            return is_bool();
        }
        else if constexpr (std::is_floating_point_v<TISTYPE>) {
            return is_float();
        }
        else if constexpr (std::is_integral_v<TISTYPE>) {
            return is_int();
        }
//        else if (std::is_same<const char*, TISTYPE>::value) {
//            return get_string(in_default);
//        }
        // if can be converted to string_views...
        else if (std::is_same<std::string_view, TISTYPE>::value) {
            return is_string();
        }
        else if constexpr (std::is_null_pointer_v<TISTYPE>) {
            return is_null();
        }

        return false;
    }
#endif

    /// functions for JSON value types #object_t or array_t

    /// Number of elements in a object or array
    /// @return the number of elements if JSON value type is #object_t or #array_t. 0 for all other types.
    size_t num_elements() const noexcept
    {
        size_t retVal = 0;
        switch (value_type())
        {
            case object_t: retVal = m_values.size(); break;
            case array_t: retVal = m_values.size(); break;
            default:
                break;

        }
        return retVal;
    }

    /// @brief Returns the number of elements that the object or array has currently allocated space for, similiar to std::vector::capacity().
    /// @return Capacity of the currently allocated storage.
    size_t capacity() noexcept
    {
        size_t capa = 0;
        if (JSONLAND_LIKELY(is_array() || is_object()))
        {
            capa = m_values.capacity();
        }
        return capa;
    }

    /// @brief Increase the capacity of the object or array, similiar to std::vector::reserve().
    /// Increase the capacity of the object or array (the total number of elements that the object or array can hold without requiring reallocation) to a value that's greater or equal to new_cap. If new_cap is greater than the current capacity(), new storage is allocated, otherwise the function does nothing.
    /// reserve() does not change the size of the object or array.
    /// <br>Does nothing if JSON value type is not #object_t or #array_t.
    /// @param new_cap new capacity of the object or array, in number of elements
    void reserve(const size_t new_cap) noexcept
    {
        if (JSONLAND_LIKELY(is_array() || is_object()))
        {
            m_values.reserve(new_cap);
            if (is_object())
                m_obj_key_to_index.reserve(new_cap);
        }
    }

    /// functions for JSON value type #object_t

    /// @brief Return the value of an object's member.
    /// @param key the key to the member
    /// @param in_default the value that will be returned in case the object does not contain #key or JSON value type is not #object_t.
    /// @return The value of the member:
    /// If called on an object with JSON value type #object_t,
    /// and the object contains a member by given name
    /// and the member type is compatible with #TGETTYPE.
    /// <br> Otherwise the default value #in_default will be returned.
    /// <br> To make sure the object contains the member call <code> contains(key)</code>
    /// <br>Example:
    /// @code
    /// jsonland::json_node an_object;
    /// an_object["one"] = 1;
    /// int i1 = an_object.member_value("one")  // i1 == 1
    /// std::string s1 = an_object.member_value("one", "not found")  // s1 == "not found" since object does contains "one" but it's type is int not string, and so default value is returned.
    /// int i2 = an_object.member_value("one-by-one", 123)  // i2 == 123 since since object does contains "one", and so default value is returned.
    /// @endcode
    template<typename TGETTYPE>
    TGETTYPE member_value(std::string_view key, const TGETTYPE in_default={}) noexcept
    {
        TGETTYPE retVal = in_default;
        if (contains(key))
        {
            json_node& theJ = m_values[m_obj_key_to_index.at(key)];
            if constexpr (std::is_same_v<bool, TGETTYPE>) {
                retVal = theJ.get_bool(in_default);
            }
            else if constexpr (std::is_floating_point_v<TGETTYPE>) {
                retVal = theJ.get_float(in_default);
            }
            else if constexpr (std::is_integral_v<TGETTYPE>) {
                return theJ.get_int(in_default);
            }
             else  if constexpr (std::is_convertible_v<TGETTYPE, std::string_view>) {
                return theJ.get_string(in_default);
            }
            else if constexpr (std::is_null_pointer_v<TGETTYPE>) {
                return nullptr;
            }
        }

        return retVal;
    }

    /// @return
    /// Number of child elements with given key if JSON value type of this object is #object_t. This is either 1 or 0 since #object_t does not allow duplicates.
    /// <br>Otherwise: 0.
    size_t count(std::string_view in_key) const noexcept
    {
        size_t retVal = 0;
        if (is_object())
        {
            retVal = m_obj_key_to_index.count(in_key);
        }

        return retVal;
    }

    /// Checks if there is a child element with given key.
    /// @return <b>true</b> if JSON value type of this object is #object_t AND the object contains
    /// child element with the given key.
    /// <br> <b>false</b> otherwise
    bool contains(std::string_view in_key) const noexcept
    {
        bool retVal = 1 == count(in_key);
        return retVal;
    }

    json_node& operator[](std::string_view in_key) noexcept
    {
        if (is_null())
        {
            clear(object_t);
        }

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

    const json_node& operator[](std::string_view in_str) const noexcept
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

    /// Erase from object by key and return the number of items erased (0 or 1)
    /// If key object does not contain the key, do nothing and return 0
    /// If not an object, do nothing and return 0
    size_t erase(std::string_view in_key)
    {
        size_t retVal{0};
        if (JSONLAND_LIKELY(is_object()))
        {
            string_or_view key;
            key.store_value_deal_with_escapes(in_key);
            if (auto iKey = m_obj_key_to_index.find(key);
                iKey != m_obj_key_to_index.end())
            {
                int index = iKey->second;
                m_obj_key_to_index.erase(iKey);
                m_values.erase(m_values.begin() + index);
                retVal = 1;
            }
        }

        return retVal;
    }

    /// Erase from array by index and return the number of items erased (0 or 1)
    /// If key array does not contain the index, do nothing and return 0
    /// If not an array, do nothing and return 0
    size_t erase(size_t in_index)
    {
        size_t retVal{0};
        if (JSONLAND_LIKELY(is_array()))
        {
            if (in_index < m_values.size()) {
                m_values.erase(m_values.begin() + in_index);
                retVal = 1;
            }
        }

        return retVal;
    }

    json_node& append_array(std::string_view in_key, size_t in_reserve=0)
    {
        if (JSONLAND_LIKELY(is_object()))
        {
            json_node& retVal = this->operator[](in_key);
            retVal = array_t;
            retVal.reserve(in_reserve);

            return retVal;
        }
        else
            return *this;  // what to return here?
    }

    /// @return a list of the object's keys in the order they were parsed or inserted,
    /// or empty list if #this is not an #object_t
    std::vector<std::string_view> keys()
    {
        std::vector<std::string_view> retVal;
        for (auto& key2indexItem : m_obj_key_to_index) {
            retVal.push_back(key2indexItem.first.as_string_view());
        }
        return retVal;
    }

    /// functions for JSON value type #array_t

    json_node& push_back(const json_node& in_node) noexcept
    {
        m_values.push_back(in_node);
        return m_values.back();
    }

    json_node& push_back(json_node&& in_node) noexcept
    {
        m_values.emplace_back(std::move(in_node));
        return m_values.back();
    }

    template<typename TNODEVAL>
    json_node& push_back(const TNODEVAL in_val) noexcept
    {
#if JSONLAND_ASSERT_TYPE_CONSISTENCY != 0
        assert(is_array() || is_null());
#endif
        m_value_type = array_t;
        m_values.emplace_back(in_val);
        return m_values.back();
    }

    template <typename INT, IsInteger<INT>* = nullptr >
    json_node& operator[](const INT in_dex) noexcept
    {
        if (JSONLAND_LIKELY(is_array() || is_object()))
        {
            return m_values.at(in_dex);
        }
        else
            return *this;  // what to return here?
    }

    template <typename INT, IsInteger<INT>* = nullptr >
    const json_node& operator[](INT in_dex) const noexcept
    {
        if (JSONLAND_LIKELY(is_array()))
        {
            return m_values.at(in_dex);
        }
        else
            return *this;  // what to return here?
    }

    json_node& append_object(size_t in_reserve=0)
    {
        if (JSONLAND_LIKELY(is_array()))
        {
            json_node& retVal = m_values.emplace_back(object_t);
            retVal.reserve(in_reserve);

            return retVal;
        }
        else
            return *this;  // what to return here?
    }

    json_node& append_array(size_t in_reserve=0)
    {
        if (JSONLAND_LIKELY(is_array()))
        {
            json_node& retVal = m_values.emplace_back(array_t);
            retVal.reserve(in_reserve);

            return retVal;
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

    /// Estimates the amount of heap memory allocated by the object.
    size_t memory_consumption() const noexcept;

    /// Serialize json to text and appends the text to #out_str.
    /// @param out_str text will be appended to this string, previous content will not be erased.
    void dump(std::string& out_str) const noexcept;

    /// Serialize json to text and return as std::string.
    /// @return Text represntation of this json object.
    std::string dump() const noexcept;

    /// Serilize json to text and append the text to a stream.
    /// @param os Text will be appended to this stream.
    /// @return os.
    std::ostream& dump(std::ostream& os) const noexcept;

    /// Get the string value as std::string_view
    /// @return the string value with .
    /// <br>If JSON value type is #null_t: null
    /// <br>If JSON value type is #bool_t: true or false
    /// <br>If JSON value type is #string_t: the string, unquoted, with escaped characters if/where needed.
    /// <br>If JSON value type is #number_t: if the number was parsed as string, will return that string, otherwise empty string.
    /// <br>If JSON value type is #array_t or #object_t: empty string.
    inline const std::string_view as_string_view() const noexcept
    {
        return m_value.as_string_view();
    }

    /// @brief If JSON value type is #string_t, return the string value (without quoates). Otherwise returns the given default value.
    ///
    ///@note This function only returns string value for JSON value type #string_t, To get other value types (#number_t, #bool_t, #null_t, #array_t,object_t) as string use <b>json_node::dump()</b>.
    ///
    /// @return
    /// If JSON value type is #string_t, return the string value.
    /// <br>If JSON value type is not #string_t, return the given default value.
    /// @param in_default_str the value to return if JSON value type is not #string_t. If #in_default_str is not provided it will default to the empty string.

    /// <br><br>Example:
    /// @code
    /// jsonland::json_node string_node{"a string"};
    /// std::string_view s1 = string_node.get_string();         // s1 == "a string", since string_node hold a string value.
    /// std::string_view s2 = string_node.get_string("banana"); // s2 == "a string", since string_node hold a string value, and so explicit default value ("banana") is ignored.
    /// jsonland::json_node float_node{7.17};
    /// std::string_view s3 = float_node.get_string();         // s3 == "", since float_node does not hold a string value.
    /// std::string_view s4 = float_node.get_string("rama");   // s4 == "rama", since float_node does not hold a string value, so explicit default value ("rama") is ignored.
    /// @endcode
    std::string_view get_string(std::string_view in_default_str={}) const noexcept
    {
        if (is_string())
        {
            if (0 != m_value.m_num_escapes) {
                m_value.unescape_internal();
            }
            return as_string_view();
        }
        else
            return in_default_str;
    }

    /// @brief If JSON value type is #number_t, returns the number as a floating point. Otherwise returns the given default value.
    ///
    /// @return
    /// If JSON value type is #number_t, return the number value.
    /// <br>If JSON value type is not #number_t, return the given default value.
    /// @param in_default_fp the value to return if JSON value type is not #number_t. If #in_default_fp is not provided it will default to 0.0.
    /// @note this function can be called templated as any floating point type: get_float<float>(), get_float<double>(), etc
    /// <br><br>Example:
    /// @code
    /// jsonland::json_node float_node{17.18f};
    /// float f1 = float_node.get_float<float>();    // f1 == 17.18f, since float_node holds the number value 17.18f.
    /// double f2 = float_node.get_float<double>(19.18);  // f2 == 17.18, since float_node holds the number value 17.18, so given default value of 19 is ignored.
    ///
    /// jsonland::json_node string_node{"a string"};
    /// double f3 = string_node.get_float<double>();    // f3 == 0.0, since string_node does not hold a number value, and implicit default value (0.0) is used.
    /// float f4 = string_node.get_float<float>(23.23); // f4 == 23.23f, since string_node does not hold a number value, and explicit default value (23.23) is used.
    /// @endcode
    template<typename TFLOAT, IsFloat<TFLOAT>* = nullptr>
    TFLOAT get_float(const TFLOAT in_default_fp=0.0) const noexcept
    {
        double retVal = in_default_fp;
        if (JSONLAND_LIKELY(is_number()))
        {
            if (m_hints & json_node::_num_in_string)
                retVal = static_cast<TFLOAT>(std::atof(m_value.data()));
            else
                retVal = static_cast<TFLOAT>(m_num);
        }

        return retVal;
    }

    /// @brief If JSON value type is number, returns the number as an integer. Otherwise returns the given default value.
    ///
    /// @return
    /// If JSON value type is #number_t, return the number value.
    /// <br>If JSON value type is not #number_t, return the given default value.
    /// @param in_default_int the value to return if JSON value type is not #number_t. If #in_default_int is not provided it will default to 0.
    /// @note this function can be called templated as any integer type (signed or unsigned): get_int<int32_t>(), get_int<uint64_t>(), etc
    /// <br><br>Example:
    /// @code
    /// jsonland::json_node int_node{17};
    /// int16_t i1 = int_node.get_int();    // i1 == int16_t(17), since int_node holds the number value 17.
    /// uint32_t i2 = int_node.get_int(19);  // i2 == uint32_t(17), since int_node holds the number value 17, so given default value of 19 is ignored.
    ///
    /// jsonland::json_node string_node{"a string"};
    /// int32_t i3 = string_node.get_int();    // i3 == int32_t(0), since string_node does not hold a number value, and implicit default value (0) is used.
    /// uint64_t i4 = string_node.get_int(23); // i4 == uint64_t(23), since string_node does not hold a number value, and explicit default value (23) is used.
    /// @endcode
    template<typename TINT, IsInteger<TINT>* = nullptr>
    TINT get_int(const TINT in_default_int=0) const noexcept
    {
        TINT retVal = in_default_int;
        if (JSONLAND_LIKELY(is_number()))
        {
            if (m_hints & json_node::_num_in_string)
                retVal = static_cast<TINT>(std::atoll(m_value.data()));
            else
                retVal = static_cast<TINT>(m_num);
        }

        return retVal;
    }

    /// @brief If JSON value type is #bool_t, returns the boolean value (true/false). Otherwise returns the given default value.
    ///
    /// @return
    /// If JSON value type is #bool_t, return the boolean value.
    /// <br>If JSON value type is not #bool_t, return the given default value.
    /// @param in_default_bool the value to return if JSON value type is not #bool_t. If #in_default_bool is not provided it will default to false.
    /// <br>Example:
    /// @code
    /// jsonland::json_node bool_node{false};
    /// bool b1 = bool_node.get_bool();      // b1 == false, since bool_node holds the boolean value false.
    /// bool b2 = bool_node.get_bool(true);  // b2 == false, since bool_node holds the boolean value false, so given default value of true is ignored.
    ///
    /// jsonland::json_node string_node{"a string"};
    /// bool b3 = string_node.get_bool();    // b3 == false, since string_node does not hold a boolean value, and implicit default value (false) is used.
    /// bool b4 = string_node.get_bool(true); // b4 == true, since string_node does not hold a boolean value, and explicit default value (true) is used.
    /// @endcode
    bool get_bool(const bool in_default_bool=false) const noexcept
    {
        if (JSONLAND_LIKELY(is_bool()))
            return m_value.data()[0] == 't';
        else
            return in_default_bool;
    }

    /// @brief Return nullptr
    ///
    /// get_null() is provided for completeness with the other get_... functions such as #get_bool, #get_int, #get_float, etc
    /// @return
    /// nullptr is returned regardless of JSON value type.

    /// <br>Example:
    /// @code
    /// jsonland::json_node null_node{nullptr};
    /// nullptr n1 = bool_node.get_null();   // n1 == nullptr
    /// jsonland::json_node string_node{"a string"};
    /// auto n2 = string_node.get_null();    // n2 == nullptr, because nullptr is returned regardless of JSON value type.
    /// @endcode
    nullptr_t get_null() const noexcept
    {
        return nullptr;
    }

    /// @brief Get the value according the the type of #TASTYPE.
    ///
    /// If there is a mismatch between the JSON value type and the type of #TASTYPE, the gived default will be returned.
    /// <br>This function is shorthand for calling the explicit get_... functions
    /// and can be called from other templated functions without needing to explicitly call a get_... function.
    /// Type of TASTYPE  | JSON value type | will return |
    /// -------------    | -------------   | -------------
    /// nullptr_t       | #null_t | #get_null()
    /// bool            | #bool_t | #get_bool()
    /// any signed int  | #number_t | #get_int()
    /// any unsigned int  | #number_t | #get_int()
    /// any floating point  | #number_t | #get_float()
    /// any other type | any |  #in_default
    /// <br>Example:
    /// @code
    /// jsonland::json_node number_node{123.456};
    /// int f1 = number_node.get_as<int>();   // f1 == 123
    /// float f2 = number_node.get_as<float>();   // f2 == 123.456
    /// bool f3 = number_node.get_as<bool>();   // f3 == false
    /// @endcode

    template<typename TASTYPE>
    TASTYPE get_as(const TASTYPE in_default={}) noexcept
    {
        if constexpr (std::is_same<bool, TASTYPE>::value) {
            return get_bool(in_default);
        }
        else if constexpr (std::is_floating_point_v<TASTYPE>) {
            return get_float<TASTYPE>(in_default);
        }
        else if constexpr (std::is_integral_v<TASTYPE>) {
            return get_int<TASTYPE>(in_default);
        }
        else if (std::is_convertible_v<TASTYPE, std::string_view>) {

            TASTYPE retVal = static_cast<TASTYPE>(get_string(in_default));
            return retVal;
        }
//        else if (std::is_same<std::string_view, TASTYPE>::value) {
//            return get_string(in_default);
//        }
        else if constexpr (std::is_null_pointer_v<TASTYPE>) {
            return nullptr;
        }

        return in_default;
    }

    std::string_view key() const noexcept
    {
        m_key.unescape_internal();
        return m_key.as_string_view();
    }

private:

    KeyToIndex& get_key_to_index_map() noexcept
    {
        return m_obj_key_to_index;
    }

protected:

    // for parser use, *this is assumed to be freshly constructed, so no need to call clear
    inline void parser_direct_set(string_or_view&& in_str, jsonland::value_type in_type)
    {
        // add asserts that m_obj_key_to_index & m_values are empty
        m_value_type = in_type;
        m_value = std::move(in_str);
    }

    inline void parser_direct_set(const std::string_view in_str, jsonland::value_type in_type)
    {
        // add asserts that m_obj_key_to_index & m_values are empty
        m_value_type = in_type;
        m_value = in_str;
    }

    inline void parser_direct_set(jsonland::value_type in_type)
    {
        // add asserts that m_obj_key_to_index & m_values are empty
        m_value_type = in_type;
    }

private:
    // null, bool, and string are all represented in m_str_view, even when new value is assigned programatically.
    // the only place where there could be a value without string representation is when a number value is assigned
    // programatically (i.e. not when read from a string).
    // One solution could be to convert the number to text upon assignment, but this could be inefficiant if get_float()
    // is called - requiring the number to be converted again from text to binary form. Another inefficiantiancly could
    // occur in the same json_node is repeatadly assigned number values. The compramise is to store the number as it was
    // given (text when parsed, binary when assigned) and treat it accodingly.
    inline bool is_num_in_string() const {return m_hints & json_node::_num_in_string;}
    inline bool is_string_assigned() const {return m_value.is_value_stored();}

};

const char* value_type_name(jsonland::value_type in_type);

class json_doc : public json_node
{
public:

    inline explicit json_doc(jsonland::value_type in_type) noexcept :  json_node(in_type) {}
    json_doc() = default;
    ~json_doc() = default;

    inline json_doc& operator=(jsonland::value_type in_type)
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
    size_t byte_position() { return 0; }   // todo return the byte position where error was discovered
    size_t memory_consumption();

    void clear();

private:
    std::string m_json_text;
    size_t m_max_nesting_level = 64;
    int m_parse_error = 0;
    std::string m_parse_error_message;
};


} // namespace jsonland

#endif // __jsonland_json_node_h__
