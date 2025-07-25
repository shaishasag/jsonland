#ifndef __jsonland_json_node_h__
#define __jsonland_json_node_h__

#include <cassert>
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

#include "jsonland_defs.h"
#include "string_and_view.h"

// #define JSONLAND_DEBUG

#ifdef JSONLAND_ASSERT_TYPE_CONSISTENCY
#undef JSONLAND_ASSERT_TYPE_CONSISTENCY
#endif
#define JSONLAND_ASSERT_TYPE_CONSISTENCY 1

namespace jsonland
{

class DllExport json_node
{
public:

    /// Converts JSON to text representation and writes it to std::ostream.
    /// @param[out] os the stream to write to.
    /// @param[in] jn the JSON to dump.
    /// @return the stream #os
    friend std::ostream& operator<<(std::ostream& os, const json_node& jn)
    {
        jn.dump(os);
        return os;
    }

    /// Decide if two #json_node object are equal.
    /// @param[in] other the #json_node to compare.
    /// @return true if the two given #json_node's values are identical, i.e. both have same JSON value type and same value. <br> return false otherwise.
    /// Note: converted to member function because VisualStudio C++20 does not recognize friend operator==
    [[nodiscard]] bool operator==(const json_node& other) const;

    /// Decide if two #json_node object are not equal.
    /// @param[in] other the #json_node to compare.
    /// @return false if the two given #json_node's are identical, true otherwise
    [[nodiscard]] bool operator!=(const json_node& other) const;


public:
    /// Type declaration of mapping of a string to the index of a value in #m_values.
    using KeyToIndex = std::unordered_map<string_and_view, int>;

    /// Type declaration for a vector of #json_nodes.
    using ArrayVec = std::vector<json_node>;

private:
    const json_node& const_uninitialized_json_node() const;

    /// JSON value type of this instance, can be any of the values in enum #value_type.
    /// Initilay it is set to #null_t
    value_type m_value_type{null_t};
    /// String representation of the JSON value.
    mutable string_and_view m_value{the_null_string_view, 0};

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
    mutable string_and_view m_key{the_empty_string_view, 0};


    mutable hint_carrier m_hints;

    friend class parser_impl::Parser;

public:
    /// Default constructor.
    /// JSON value type (#m_value_type) is set to #null_t.
    explicit json_node() = default;
    /// Default destructor.
    ~json_node() = default;

    /// Copy constructor
    /// @param in_node the #json_node to copy
    explicit json_node(const json_node& in_node) = default;

    /// Move constructor
    /// @param in_node the #json_node to move
    explicit json_node(json_node&& in_node) = default;

    /// Copy assignment: deleted to avoid ownership confusion
    /// To copy a json_node:
    /// json_node j1 = j0.clone();
    /// To move a json_node:
    /// json_node j1 = std::move(j0);
    json_node& operator=(const json_node& in_node) = delete;

    /// Move assignment
    /// @param in_node the #json_node to move
    json_node& operator=(json_node&& in_node) noexcept = default;

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
    explicit json_node(jsonland::value_type in_type, size_t in_reserve=0) noexcept;

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
    inline json_node& operator=(const std::string_view in_str_value) noexcept
    {
        clear(string_t);
        m_value = in_str_value;
        return *this;
    }


    /// Constructor with string value, but #value_type to set to #in_type not necessarily #string_t.
    /// @param in_str_value the new string value.
    /// @param in_type the #value_type.
    /// Example:<br>
    /// @code
    /// jsonland::json_node n1("1234", jsonland::number_t);
    /// @codeend
    explicit json_node(const std::string_view in_str_value, jsonland::value_type in_type) noexcept;

    /// Construct with std::string_view, #value_type to set to #string_t
    /// @param in_str_value the new string value.<br>
    /// Efficiency note: a copy of in_str_value will be created, requiring an allocation.
    explicit json_node(const std::string_view in_str_value) noexcept
    : m_value_type(string_t)
    , m_value(in_str_value)
    {
    }

    /// Assign a new value from std::string, #value_type to set to #string_t
    /// @param in_str_value the new string value.<br>
    /// Efficiency note: a copy of in_str_value will be created, requiring an allocation (most likely).
    inline json_node& operator=(const std::string& in_str_value) noexcept
    {
        clear(string_t);
        m_value = in_str_value;

        return *this;
    }


    /// Assign a new value from a string, #value_type to set to #string_t.
    /// @param in_str the new value.<br>
    /// Example:<br>
    /// @code
    /// json_node n1("123");
    /// n1.is_number();  // false, it's not a number it's a string
    /// n1.get_int<int32_t>() == 123;  // false, value is the string "123"
    /// @codeend

    // VisualStudio 2022 cannot understand "const IsChar auto in_str[]"
    // so need explicit template decleration here
    template <typename TCHAR>
    requires IsChar<TCHAR>
    json_node& operator=(const TCHAR in_str []) noexcept
    {
        clear(string_t);
        m_value = in_str;

        return *this;
    }

    //--- integer constructor
    inline json_node(const IsInteger auto in_num) noexcept
    : m_value_type(number_t)
    , m_value()
    , m_num(static_cast<double>(in_num))
    , m_hints(_num_is_int)
    {}

    // assign number
    json_node& operator=(const IsInteger auto in_num) noexcept
    {
        clear(number_t);
        m_num = static_cast<double>(in_num);
        m_hints.set_hint(_num_is_int);

        return *this;
    }

    //--- enum constructor, converts to undelying integral type
    json_node(const IsEnum auto in_enum) noexcept
    : json_node(static_cast<typename std::underlying_type_t<decltype(in_enum)>>(in_enum))
    {}

    // assign enum
    inline json_node& operator=(const IsEnum auto in_enum) noexcept
    {
        operator=(static_cast<typename std::underlying_type_t<decltype(in_enum)>>(in_enum));
        return *this;
    }

    //--- float constructor
    json_node(const IsFloat auto in_num) noexcept
    : m_value_type(number_t)
    , m_value()
    , m_num(static_cast<double>(in_num))
    {}
    // assign number
    inline json_node& operator=(const IsFloat auto in_num) noexcept
    {
        clear(number_t);
        m_num = static_cast<double>(in_num);

        return *this;
    }

    //--- bool constructor
    inline explicit json_node(const IsBool auto in_bool) noexcept
    : m_value_type(bool_t)
    , m_value(in_bool ? the_true_string_view : the_false_string_view, 0)
    {}

    // assign bool
    json_node& operator=(const IsBool auto in_bool) noexcept
    {
        clear(bool_t);
        m_value.set_without_ownership(in_bool ? the_true_string_view : the_false_string_view);

        return *this;
    }

    //--- null constructor
    explicit json_node(IsNullPtr auto) noexcept : json_node()
    {}

    // assign null
    json_node& operator=(IsNullPtr auto)
    {
        clear(null_t);
        return *this;
    }

    /// Clears the JSON value, deallocate child elemets, if any, and sets the value type to the given param.
    /// @param in_new_type the new type to set. Deafults to #null_t.
    ///
    void clear(const value_type in_new_type=null_t) noexcept;

    /// Returns the #value_type of this instance
    [[nodiscard]] inline jsonland::value_type get_value_type() const noexcept {return m_value_type;}

    /// Returns true if and only if the JSON value type is #null_t.
    [[nodiscard]] inline bool is_null() const noexcept {return null_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #object_t.
    [[nodiscard]] inline bool is_object() const noexcept {return object_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #array_t.
    [[nodiscard]] inline bool is_array() const noexcept {return array_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #string_t.
    [[nodiscard]] inline bool is_string() const noexcept {return string_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #number_t (float or integer).
    [[nodiscard]] inline bool is_number() const noexcept {return number_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #number_t, and the value is an integer (signed or unsigned).
    [[nodiscard]] inline bool is_int() const noexcept {return number_t == m_value_type && (m_hints.get_hint(_num_is_int));}
    /// Returns true if and only if the JSON value type is #number_t, and the value is floating point (float, double).
    [[nodiscard]] inline bool is_float() const noexcept {return number_t == m_value_type && (!m_hints.get_hint(_num_is_int));}
    /// Returns true if and only if the JSON value type is #bool_t.
    [[nodiscard]] inline bool is_bool() const noexcept {return bool_t == m_value_type;}
    /// Returns true if and only if the JSON value type is #bool_t, #number_t or #string_t.
    [[nodiscard]] inline bool is_scalar() const noexcept {return is_string() || is_number() || is_bool();}
    /// Returns true if and only if the JSON value is set, i.e. not #uninitialized_t.
    [[nodiscard]] inline bool is_valid() const noexcept {return is_scalar() || is_array() || is_object() || is_null();}
    /// Returns true if and only if the JSON type is structured (#array_t or #object_t).
    [[nodiscard]] inline bool is_structured() const noexcept { return is_array() || is_object(); }

    /// Returns true if and only if the JSON type is the same as given in param #in_type
    /// @param[in] check_for_type the type in question.
    [[nodiscard]] inline bool is_type(jsonland::value_type check_for_type) const noexcept {return check_for_type == m_value_type;}

    /// functions for JSON value types #object_t or array_t

    /// Number of elements in a object or array
    /// @return the number of elements if JSON value type is #object_t or #array_t. 0 for all other types.
    [[nodiscard]] size_t num_elements() const noexcept;

    // Return the "size" according to the type, where size means:
    // num_elements for object_t and array_t
    // length of string for string_t
    // 1 for number_t and bool_t, since they always hold some (possibly default) value
    // @param in_expected_type the type to be checked
    // @return size if this json_node is of expected type
    // num_elements for object_t
    // num_elements for array_t
    // length of string for string_t
    // 1 is always returned for number_t and bool_t
    // 0 is always returned for null_t
    // If the actual type does not match in_expected_type, 0 is returned

    template<enum value_type JTYPE>
    [[nodiscard]] size_t size_as() const noexcept
    {
        if constexpr (JTYPE == value_type::object_t || JTYPE == value_type::array_t) {
            return (m_value_type == JTYPE) ? m_values.size() : 0;
        }
        else if constexpr (JTYPE == value_type::string_t) {
            return (m_value_type == JTYPE) ? m_value.size() : 0;
        }
        else if constexpr (JTYPE == value_type::bool_t || JTYPE == value_type::number_t) {
            return (m_value_type == JTYPE) ? 1 : 0;
        }
        else if constexpr (JTYPE == value_type::null_t) {
            return (m_value_type == JTYPE) ? 0 : 0;
        }
        else {
            return 0;
        }

    }

    [[nodiscard]] size_t size_as(const enum value_type in_expected_type) const noexcept;

    // Return "empty" according to the type, where empty means:
    // num_elements == 0 for object_t and array_t
    // length of string == 0  for string_t
    // false is always returned for number_t and bool_t, since they always hold some value
    // true is always returned for null_t
    // @param in_expected_type the type to be checked
    // If the actual type does not match in_expected_type, true is returned
    template<enum value_type JTYPE>
    [[nodiscard]] inline bool empty_as() const noexcept
    {
        bool retVal = 0 == size_as<JTYPE>();
        return retVal;
    }
    [[nodiscard]] inline bool empty_as(const enum value_type in_expected_type) const noexcept
    {
        bool retVal = 0 == size_as(in_expected_type);
        return retVal;
    }

    // Return "empty" for the current json type, where empty means:
    // num_elements == 0 for object_t and array_t
    // length of string == 0  for string_t
    // false is always returned for number_t and bool_t, since they always hold some value
    // true is always returned for null_t, and for uninitialized_t
    [[nodiscard]] inline bool empty() const noexcept
    {
        bool retVal = empty_as(get_value_type());
        return retVal;
    }

    /// @brief Returns the number of elements that the object or array has currently allocated space for,
    /// similiar to std::vector::capacity().
    /// @return Capacity of the currently allocated storage.
    [[nodiscard]] inline size_t capacity() noexcept
    {
        size_t capa = 0;
        if (is_array() || is_object()) [[likely]]
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
    void reserve(const size_t new_cap) noexcept;

    /// functions for JSON value type #object_t

    /// @brief Return the value of an object's member.
    /// @param key the key to the member
    /// @param in_default the value that will be returned in case:
    ///     - JSON value type is not #object_t.
    ///     or
    ///     - the object does not contain #key
    ///     or
    ///     - the object does contain #key, but the value is not of type #TGETTYPE
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
    /// int i1 = an_object.get_value("one")  // i1 == 1
    /// std::string s1 = an_object.get_value("one", "not found")  // s1 == "not found" since object does contains "one" but it's type is int not string, and so default value is returned.
    /// int i2 = an_object.get_value("one-by-one", 123)  // i2 == 123 since since object does contains "one", and so default value is returned.
    /// @endcode
    template<typename TGETTYPE>
    [[nodiscard]] TGETTYPE get_value(std::string_view key, const TGETTYPE in_default={}) const noexcept
    {
        TGETTYPE retVal = in_default;
        if (contains(key))  // contains return false if this is not an object_t
        {
            const json_node& theJ = m_values[m_obj_key_to_index.at(string_and_view(key, 0))];
            if constexpr (std::same_as<bool, TGETTYPE>) {
                retVal = theJ.get_bool(in_default);
            }
            else if constexpr (IsFloat<TGETTYPE>) {
                retVal = theJ.get_float(in_default);
            }
            else if constexpr (IsInteger<TGETTYPE>) {
                return theJ.get_int(in_default);
            }
             else  if constexpr (std::convertible_to<TGETTYPE, std::string_view>) {
                return theJ.get_string(in_default);
            }
            else if constexpr (IsNullPtr<TGETTYPE>) {
                return nullptr;
            }
        }

        return retVal;
    }

    /// @return
    /// Number of child elements with given key if JSON value type of this object is #object_t.
    /// <br>Otherwise: 0.
    /// Return vlaue is always either 1 or 0 since #object_t does not allow duplicates, and all
    /// other value_types would return 0.
    [[nodiscard]] inline size_t count(std::string_view in_key) const noexcept
    {
        size_t retVal = 0;
        if (is_object())
        {
            retVal = m_obj_key_to_index.count(string_and_view(in_key, 0));
        }

        return retVal;
    }

    /// Checks if there is a child element with given key.
    /// @return <b>true</b> if JSON value type of this object is #object_t AND the object contains
    /// child element with the given key.
    /// <br> <b>false</b> otherwise
    [[nodiscard]] inline bool contains(std::string_view in_key) const noexcept
    {
        bool retVal = 1 == count(in_key);
        return retVal;
    }

    /// Checks if there is a child element with given key and given type.
    /// @return <b>true</b> if JSON value type of this object is #object_t AND the object contains
    /// child element with the given key, and the child element is of the given type.
    /// <br> <b>false</b> otherwise
    [[nodiscard]] bool contains_as(std::string_view in_key, const enum value_type in_expected_type) const noexcept;

    json_node& operator[](std::string_view in_key) noexcept;

    /// @brief Accesses the JSON node corresponding to the specified key.
    ///
    /// Provides read-only access to a JSON node based on the given key. If the current json_node
    /// is an object and contains the specified key, this function returns a reference to the node
    /// associated with that key. Otherwise, it returns a reference to an uninitialized json_node.
    ///
    /// @param in_str The key to look up, represented as a `std::string_view`.
    ///
    /// @return A reference to the `json_node` associated with the specified key if found;
    ///         otherwise, returns a reference to `const_uninitialized_json_node`.
    ///
    /// @note This function assumes the current `json_node` is an object. If it is not, or if the
    ///       specified key does not exist within the object, the function returns a reference
    ///       to `const_uninitialized_json_node`.
    ///
    /// @exception None This function is marked `noexcept` and will not throw.
    ///
    /// @warning Accessing a non-existent key returns `const_uninitialized_json_node`, which may
    ///          represent an invalid or empty state. The caller should verify the presence of the key
    ///          beforehand or check the returned node's validity before use.
    const json_node& operator[](std::string_view in_str) const noexcept;

    /// Erase from object by key and return the number of items erased (0 or 1)
    /// If key object does not contain the key, do nothing and return 0
    /// If not an object, do nothing and return 0
    size_t erase(std::string_view in_key);

    /// Erase from array by index and return the number of items erased (0 or 1)
    /// If key array does not contain the index, do nothing and return 0
    /// If not an array, do nothing and return 0
    size_t erase(size_t in_index);

    [[nodiscard]] inline json_node clone() const noexcept
    {
        return json_node(*this);
    }

    // append object to object
    json_node& append_object(std::string_view in_key, size_t in_reserve=0);

    // append array to object
    json_node& append_array(std::string_view in_key, size_t in_reserve=0);

    /// @return a list of the object's keys in the order they were parsed or inserted,
    /// or empty list if #this is not an #object_t
    [[nodiscard]] std::vector<std::string_view> keys();

    /// functions for JSON value type #array_t

    template<typename... Args>
    json_node& emplace_back(Args&&... args) noexcept
    {
        if (is_null())
        {
            clear(array_t);
        }
        m_values.emplace_back(std::forward<Args>(args)...);
        return m_values.back();
    }

    json_node& push_back(const json_node& in_node) noexcept;

    json_node& push_back(json_node&& in_node) noexcept;


    json_node& push_back(const IsJsonScalarType auto in_val) noexcept
    {
    #if JSONLAND_ASSERT_TYPE_CONSISTENCY != 0
        assert(is_array() || is_null());
    #endif
        m_value_type = array_t;
        m_values.emplace_back(in_val);
        return m_values.back();
    }

    [[nodiscard]] inline json_node& operator[](const IsInteger auto in_dex) noexcept
    {
        if (is_array() || is_object()) [[likely]]
        {
            return m_values.at(in_dex);
        }
        else
            return *this;  // what to return here?
    }

    /// @brief Accesses the JSON node at the specified index for array-type json_node.
    ///
    /// Provides read-only access to a json_node based on the given integer index. If the current
    /// json_node is an array and the specified index is within bounds, this function returns a
    /// reference to the node at that index. Otherwise, it returns a reference to an uninitialized
    /// json_node.
    ///
    /// @tparam INT An integer type, constrained by `IsInteger`.
    /// @param in_dex The index to access, represented as an integer of type `INT`.
    ///
    /// @return A reference to the `json_node` at the specified index if it is within bounds;
    ///         otherwise, returns a reference to `const_uninitialized_json_node`.
    ///
    /// @note This function assumes the current `json_node` is an array. If it is not, or if the
    ///       specified index is out of bounds, the function returns a reference to
    ///       `const_uninitialized_json_node`.
    ///
    /// @exception None This function is marked `noexcept` and will not throw.
    ///
    /// @warning Accessing an out-of-bounds index returns `const_uninitialized_json_node`, which
    ///          may represent an invalid or empty state. The caller should verify that the index is
    ///          within bounds or check the returned node's validity before use.

    [[nodiscard]] inline const json_node& operator[](IsInteger auto in_dex) const noexcept;

    /// @brief Appends a new empty JSON object to this array.
    ///
    /// If this node is an array, a new JSON object is appended to it,
    /// optionally reserving space for `in_reserve` key-value pairs.
    ///
    /// @param in_reserve Optional reservation size for the new object's key-value map.
    /// @return Reference to the newly appended JSON object if this node is an array;
    ///         otherwise, returns `*this` unchanged.
    json_node& append_object(size_t in_reserve=0);

    /// @brief Appends a new empty JSON array to this array.
    ///
    /// If this node is an array, a new JSON array is appended to it,
    /// optionally reserving space for `in_reserve` elements.
    ///
    /// @param in_reserve Optional reservation size for the new array's element vector.
    /// @return Reference to the newly appended JSON array if this node is an array;
    ///         otherwise, returns `*this` unchanged.
    json_node& append_array(size_t in_reserve=0);

    template<typename TNode>
    class DllExport iterator_template
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

    };

    using iterator = iterator_template<json_node>;
    using const_iterator = iterator_template<const json_node>;

    [[nodiscard]] inline iterator begin()
    {
        return iterator(*this, 0);
    }

    [[nodiscard]] inline iterator end()
    {
        return iterator(*this, m_values.size());
    }
    [[nodiscard]] inline const_iterator begin() const
    {
        return const_iterator(*this, 0);
    }

    [[nodiscard]] inline const_iterator end() const
    {
        return const_iterator(*this, m_values.size());
    }

    /// Estimates the amount of heap memory allocated by the this instance and its sub-values.
    [[nodiscard]] size_t memory_consumption() const noexcept;

    // ---

    /// Serialize json to text and appends the text to #out_str.
    /// @param out_str text will be appended to this string, previous content will not be erased.
    void dump(std::string& out_str,
              dump_style in_style=dump_style::tight) const noexcept;

    // ---

    /// Serialize json to text and return as std::string.
    /// @return Text represntation of this json object.
    std::string dump(dump_style in_style=dump_style::tight) const noexcept;

    // ---

    /// Serilize json to text and append the text to a stream.
    /// @param os Text will be appended to this stream.
    /// @return os.
    std::ostream& dump(std::ostream& os,
                       dump_style in_style=dump_style::tight) const noexcept;

    // ---

    /// Get the string value as std::string_view
    /// @return the string value
    /// <br>If JSON value type is #null_t: null
    /// <br>If JSON value type is #bool_t: true or false
    /// <br>If JSON value type is #string_t: the string, unquoted, with escaped characters if/where needed.
    /// <br>If JSON value type is #number_t: if the number was parsed as string, will return that string, otherwise empty string.
    /// <br>If JSON value type is #array_t or #object_t: empty string.
    [[nodiscard]] inline const std::string_view as_string_view() const noexcept
    {
        return m_value.sv();
    }

    // ---

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
    [[nodiscard]] std::string_view get_string(std::string_view in_default_str={}) const noexcept;

    // ---

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
    template<IsFloat TFLOAT=float>
    [[nodiscard]] TFLOAT get_float(const TFLOAT in_default_fp=0.0f) const noexcept;

    // ---

    // shortcut for get_float<double>
    [[nodiscard]] inline double get_double(const double in_default_dbl=0.0) const noexcept
    {
        return get_float<double>(in_default_dbl);
    }

    // ---

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
    template<IsInteger TINT=int>
    [[nodiscard]] TINT get_int(const TINT in_default_int=0) const noexcept;

    // ---

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
    [[nodiscard]] inline bool get_bool(const bool in_default_bool=false) const noexcept
    {
        if (is_bool()) [[likely]]
        {
            return m_value.sv().data()[0] == 't';
        }
        else
        {
            return in_default_bool;
        }
    }

    // ---

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
    [[nodiscard]] inline nullptr_t get_null() const noexcept
    {
        return nullptr;
    }

    // ---

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
    /// int f1 = number_node.get<int>();   // f1 == 123
    /// float f2 = number_node.get<float>();   // f2 == 123.456
    /// bool f3 = number_node.get<bool>();   // f3 == false
    /// @endcode

    template<IsJsonScalarType TASTYPE>
    [[nodiscard]] TASTYPE get(const TASTYPE in_default={}) const noexcept;

    // ---

    /// @brief Get the value according the the type of #TASTYPE or try to convert the value to #TASTYPE
    ///
    /// Useful, for example, when a json_node might hold a number as a string
    /// If there is a mismatch between the JSON value type and the type of #TASTYPE,
    /// try to convert the string_t to the required type. If no conversion can be made return the
    /// default initialization for #TASTYPE
    ///
    /// <br>Example:
    /// @code
    /// jsonland::json_node number_in_str_node{"123.456"};
    /// int f1 = number_node.get_as<int>();         // f1 == 123
    /// float f2 = number_node.get_as<float>();     // f2 == 123.456
    /// bool f3 = number_node.get_as<bool>();       // f3 == false
    /// @endcode
    template<IsJsonScalarType TASTYPE>
    [[nodiscard]] TASTYPE get_as() const noexcept;

    // ---

    [[nodiscard]] inline std::string_view key() const noexcept
    {
        return m_key.sv();
    }

    [[nodiscard]] bool refers_to_external_memory() const noexcept;
    [[nodiscard]] bool is_full_owner() const noexcept;
    void take_ownership() noexcept;

    // ---

    /// @brief Assigns this JSON node from a source node by deep copy.
    ///
    /// Copies all data fields from the source node `src` into this node.
    /// If `src` is the same object as `*this`, the function does nothing.
    ///
    /// @param src The source `json_node` to copy from.
    void assign_from(const json_node& src);

    // ---

    /// @brief Assigns this JSON node from another by move.
    ///
    /// Moves all data fields from the source node `src` into this node for efficiency:
    /// - Value type and scalar/numeric values are directly moved.
    /// - Internal vector of children is swapped for exception safety and performance.
    /// - Object key index map and hints are moved.
    ///
    /// After the move, `src` is cleared and left in a valid but empty state.
    ///
    /// If `src` is the same object as `*this`, the function does nothing.
    ///
    /// @param src The `json_node` to move from (rvalue reference).
    void assign_from(json_node&& src);

    // ---

    // Extend function for key-value containers (JSON objects)
    template<KeyValueContainer TContainer>
    void extend(const TContainer& container);

    // ---

    // Extend function for sequence containers (JSON arrays)
    template<SequenceContainer TContainer>
    void extend(const TContainer& container);

    // ---

    // Extract function for sequence containers (JSON arrays)
    template<SequenceContainer TContainer>
    void extract(TContainer& container) const;

    // ---

    template<KeyValueContainer TContainer>
    void extract(TContainer& container) const;

    // ---


    /// @brief Merges another JSON object into this one by copy.
    ///
    /// If both this node and the input node are objects, all key-value pairs from
    /// the input are inserted into this node. For existing keys, their values are
    /// replaced using `assign_from(const json_node&)`.
    ///
    /// This performs a deep copy of the values from `in_to_merge`.
    ///
    /// @param in_to_merge A constant reference to the JSON object to merge into this one.
    void merge_from(const json_node& in_to_merge);

    // ---

    /// @brief Merges another JSON object into this one by move.
    ///
    /// If both this node and the input node are objects, all key-value pairs from
    /// the input are moved into this node. For existing keys, their values are
    /// replaced using `assign_from(json_node&&)`.
    ///
    /// This allows efficient transfer of data from `in_to_merge` without deep copying.
    /// After the merge, `in_to_merge` is cleared and left in a valid but empty state.
    ///
    /// @param in_to_merge An rvalue reference to the JSON object to merge into this one.
    void merge_from(json_node&& in_to_merge);

protected:

    void parser_direct_set(const std::string_view in_str, jsonland::value_type in_type);
    void parser_direct_set(jsonland::value_type in_type);

    void dump_pretty(std::string& out_str, size_t level=0) const noexcept;
    void dump_tight(std::string& out_str) const noexcept;

private:
    // null, bool, and string are all represented in m_str_view, even when new value is assigned programatically.
    // the only place where there could be a value without string representation is when a number value is assigned
    // programatically (i.e. not when read from a string).
    // One solution could be to convert the number to text upon assignment, but this could be inefficient if get_float()
    // is called - requiring the number to be converted again from text to binary form. Another inefficiency could
    // occur in the same json_node is repeatadly assigned number values. The compromise is to store the number as it was
    // given (text when parsed, binary when assigned) and treat it accodingly.
    bool is_num_in_string() const {return m_hints.get_hint(_num_in_string);}
    bool is_string_assigned() const {return m_value.is_owner();}

};

[[nodiscard]] constexpr std::string_view value_type_name(jsonland::value_type in_type);

class DllExport json_doc : public json_node
{
public:

    explicit json_doc(jsonland::value_type in_type) noexcept :  json_node(in_type) {}
    json_doc() = default;
    ~json_doc() = default;

    json_doc& operator=(jsonland::value_type in_type)
    {
        json_node::operator=(in_type);
        return *this;
    }

    friend class jsonland::parser_impl::Parser;


    // parse json string without copying it. Original string lifetime must be at lease
    // as long as this json_doc lifetime is
    ParseResult parse_insitu(std::string_view in_json_str) { return parse_inplace(in_json_str); }
    ParseResult parse_inplace(std::string_view in_json_str);
    
    // parse a copy of a json string. Original string can be discarded.
    ParseResult parse(const std::string_view in_json_str);

    void set_max_nesting_level(const size_t in_nesting_level);
    void dump_tokens(std::ostream& os, char* in_json_str, char* in_json_str_end);

    size_t byte_position() { return 0; }   // todo return the byte position where error was discovered
    size_t memory_consumption();

    void clear();

private:
    std::string m_json_text;
    size_t m_max_nesting_level = 64;
};

template<IsFloat TFLOAT>
TFLOAT json_node::get_float(const TFLOAT in_default_fp) const noexcept
{
    TFLOAT retVal = in_default_fp;
    if (is_number())  [[likely]]
    {
        if (m_hints.get_hint(_num_in_string))
        {
            retVal = static_cast<TFLOAT>(std::atof(m_value.sv().data()));
        }
        else
        {
            retVal = static_cast<TFLOAT>(m_num);
        }
    }

    return retVal;
}
template<IsInteger TINT>
TINT json_node::get_int(const TINT in_default_int) const noexcept
{
    TINT retVal = in_default_int;
    if (is_number()) [[likely]]
    {
        if (m_hints.get_hint(_num_in_string))
        {
            retVal = static_cast<TINT>(std::atoll(m_value.sv().data()));
        }
        else
        {
            retVal = static_cast<TINT>(m_num);
        }
    }

    return retVal;
}

template<IsJsonScalarType TASTYPE>
TASTYPE json_node::get(const TASTYPE in_default) const noexcept
{
    if constexpr (IsBool<TASTYPE>) {
        return get_bool(in_default);
    }
    else if constexpr (IsFloat<TASTYPE>) {
        return get_float<TASTYPE>(in_default);
    }
    else if constexpr (IsInteger<TASTYPE>) {
        return get_int<TASTYPE>(in_default);
    }
    else if constexpr (std::convertible_to<TASTYPE, std::string_view>) {

        TASTYPE retVal = static_cast<TASTYPE>(get_string(in_default));
        return retVal;
    }
    else if constexpr (IsNullPtr<TASTYPE>) {
        return nullptr;
    }
    else {
        static_assert(!std::same_as<TASTYPE, TASTYPE>, "Unsupported type");
        return in_default;
    }
}

template<IsJsonScalarType TASTYPE>
[[nodiscard]] TASTYPE json_node::get_as() const noexcept
{
    TASTYPE retVal{};
    if constexpr (IsNullPtr<TASTYPE>) {
        retVal = nullptr;
    }
    else if constexpr (IsBool<TASTYPE>)
    {
        switch (m_value_type)
        {
            case uninitialized_t:
            case null_t:
                retVal = false;
            break;
            case bool_t:
                retVal = get_bool();
            break;
            case number_t:
                retVal = get_int<int>() == 0 ? false : true;
            break;
            case string_t:
            {
                std::string_view str_val = get_string();
                if (str_val == "true"sv) {
                    retVal = true;
                }
                else {
                    retVal = false;
                }
            }
            break;
            case array_t:
            case object_t:
            default:
                retVal = false;
            break;
        }
    }
    else if constexpr (IsFloat<TASTYPE>)
    {
        switch (m_value_type)
        {
            case uninitialized_t:
            case null_t:
                retVal = 0.0;
            break;
            case bool_t:
                retVal = get_bool() ? TASTYPE{1.0} : TASTYPE{0.0};
            break;
            case number_t:
                retVal = get_float<TASTYPE>();
            break;
            case string_t:
            {
                std::string_view str_val = get_string();
                retVal = static_cast<TASTYPE>(std::atof(str_val.data()));
            }
            break;
            case array_t:
            case object_t:
            default:
                retVal = 0.0;
            break;
        }
    }
    else if constexpr (IsInteger<TASTYPE>)
    {
        switch (m_value_type)
        {
            case uninitialized_t:
            case null_t:
                retVal = 0;
            break;
            case bool_t:
                retVal = get_bool() ? TASTYPE{1} : TASTYPE{0};
            break;
            case number_t:
                retVal = get_int<TASTYPE>();
            break;
            case string_t:
            {
                std::string_view str_val = get_string();
                retVal = static_cast<TASTYPE>(std::atoll(str_val.data()));
            }
            break;
            case array_t:
            case object_t:
            default:
                retVal = 0;
            break;
        }
    }
    else if constexpr (std::convertible_to<TASTYPE, std::string_view>)
    {
        switch (m_value_type)
        {
            case uninitialized_t:
                retVal = the_empty_string_view;
            break;
            case null_t:
                retVal = the_null_string_view;
            break;
            case bool_t:
                retVal = get_bool() ? the_true_string_view : the_false_string_view;
            break;
            case number_t:
                if (!is_num_in_string())
                {
                    std::string out_str;
                    dump(out_str);
                    m_value = std::move(out_str);
                    m_hints.set_hint(_num_in_string);
                }
                retVal = m_value.sv();
            break;
            case string_t:
            {
                retVal = static_cast<TASTYPE>(get_string());
            }
            break;
            case array_t:
            case object_t:
            default:
                retVal = the_empty_string_view;
            break;
        }
    }
    else {
        static_assert(!std::same_as<TASTYPE, TASTYPE>, "Unsupported type");
    }

    return retVal;
}

template<KeyValueContainer TContainer>
void json_node::extend(const TContainer& container)
{
    if (is_null())
    {
        clear(object_t);
    }

    for (const auto& [key, value] : container)
    {
        this->operator[](key) = value;
    }
}

template<SequenceContainer TContainer>
void json_node::extend(const TContainer& container)
{
    if (is_null())
    {
        clear(array_t);
    }

    for (const auto& value : container)
    {
        push_back(value);
    }
}

template<SequenceContainer TContainer>
void json_node::extract(TContainer& container) const
{
    if (is_array())
    {
        for (const auto& value_j : m_values)
        {
            container.push_back(value_j.get_as<typename TContainer::value_type>());
        }
    }
}

template<KeyValueContainer TContainer>
void json_node::extract(TContainer& container) const
{
    if (is_object())
    {
        for (const auto& value_j : m_values)
        {
            container[value_j.key()] = value_j.get_as<typename TContainer::mapped_type>();
        }
    }
}

inline const json_node& json_node::operator[](IsInteger auto in_dex) const noexcept
{
    if (size_as(array_t) > static_cast<size_t>(in_dex))
    {
        return m_values.at(in_dex);
    }
    else
        return const_uninitialized_json_node();
}

} // namespace jsonland


#endif // __jsonland_json_node_h__
