#ifndef __JsOn_h__
#define __JsOn_h__

#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <format>
#include <sstream>

/* Copy to #include
 #include "jsonland/JsOn.h"
 */

#include "jsonland_defs.h"

namespace jsonland
{
namespace parser_helper { class JsOn_parser; }

class DllExport JsOn
{
    
private:
    friend class jsonland::parser_helper::JsOn_parser;
    
    value_type m_value_type{uninitialized_t};
    std::string_view m_value;
    union { int64_t m_int; double m_float; };
    std::unordered_map<std::string_view, JsOn> m_obj_values;
    std::vector<JsOn> m_array_values;
    mutable hint_carrier m_hints;

public:
    
    // #constructors#
    JsOn() = default;
    JsOn(const JsOn&) = default;
    JsOn(JsOn&&) = default;
    JsOn& operator=(const JsOn&) = default;
    JsOn& operator=(JsOn&&) = default;

    explicit JsOn(value_type in_type, size_t in_reserve=0) noexcept;

    JsOn(const std::string_view in_text);
    explicit JsOn(const std::string_view in_str_value, jsonland::value_type in_type) noexcept;

    // #parse from / dump to text#
    int parse_inplace(const std::string_view in_text) noexcept;
    void dump(std::string& out_j_text, size_t level=0) const;
    
    // #modifiers#
    // ##operator=##
    JsOn& operator=(jsonland::value_type in_type) noexcept;
    JsOn& operator=(const std::string_view in_str_value) noexcept;
    
    JsOn(const IsInteger auto in_num) noexcept
    : m_value_type{number_t}
    , m_int{in_num}
    , m_hints{_num_is_int}
    {
    }
    //--- enum constructor, converts to undelying integral type
    JsOn(const IsEnum auto in_enum) noexcept
    : JsOn(static_cast<typename std::underlying_type_t<decltype(in_enum)>>(in_enum))
    {}

    JsOn(const IsFloat auto in_num) noexcept
    : m_value_type(number_t)
    , m_float{in_num}
    {
        m_hints.unset_hint(_num_is_int);
    }
    
    inline explicit JsOn(const IsBool auto in_bool) noexcept
    : m_value_type(bool_t)
    , m_value(in_bool ? the_true_string_view : the_false_string_view)
    {}

    explicit JsOn(IsNullPtr auto) noexcept
    : m_value_type(null_t)
    {}

    JsOn& operator=(const IsInteger auto in_num) noexcept
    {
        clear(number_t);
        m_int = static_cast<int64_t>(in_num);
        m_hints.set_hint(_num_is_int);
        return *this;
    }
    JsOn& operator=(const IsEnum auto in_enum) noexcept
    {
        operator=(static_cast<typename std::underlying_type_t<decltype(in_enum)>>(in_enum));
        return *this;
    }
    
    JsOn& operator=(const IsFloat auto in_num) noexcept
    {
        clear(number_t);
        m_float = static_cast<double>(in_num);

        return *this;
    }
    JsOn& operator=(const IsBool auto in_bool) noexcept
    {
        clear(bool_t);
        m_value = (in_bool ? the_true_string_view : the_false_string_view);
        
        return *this;
    }

    void clear(const value_type in_new_type=null_t) noexcept;

    // #push_back/copy for array_t#
    void push_back(const JsOn& value);
    // #push_back/move for array_t#
    void push_back(JsOn&& value);
    
    // emplace_back
    template<typename... Args>
    JsOn& emplace_back(Args&&... args)
    {
        assert(is_array() && "emplace_back() called on non-array JsOn");
        return m_array_values.emplace_back(std::forward<Args>(args)...);
    }

    // #Element access#
    // #get value#
    [[nodiscard]] std::string_view get_string(std::string_view in_default_str={}) const noexcept;
    [[nodiscard]] double get_double(const double in_default_fp=0.0) const noexcept;
    [[nodiscard]] float get_float(const float in_default_fp=0.0f) const noexcept;
    [[nodiscard]] int64_t get_int(const int64_t in_default_int=0) const noexcept;
    [[nodiscard]] bool get_bool(const bool in_default_bool=false) const noexcept;
    [[nodiscard]] nullptr_t get_null() const noexcept { return nullptr; }

    // #for object_t: range loop - modifiable#
    [[nodiscard]] auto& object_range() { return m_obj_values; }
    // #for array_t: range loop - modifiable#
    [[nodiscard]] auto& array_range() { return m_array_values; }

    // #for object_t: key-based access - modifiable#
    JsOn& operator[](std::string_view key);
    /// Erase from object by key and return the number of items erased (0 or 1)
    /// If key object does not contain the key, do nothing and return 0
    /// If not an object, do nothing and return 0
    size_t erase(std::string_view in_key);
 
    // #for array_t: index-based access - modifiable#
    JsOn& operator[](size_t index);

    /// Erase from array by index and return the number of items erased (0 or 1)
    /// If key array does not contain the index, do nothing and return 0
    /// If not an array, do nothing and return 0
    size_t erase(size_t in_index);

    // #get type#
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

    [[nodiscard]] bool is_type(const value_type in_type) const noexcept {return in_type == m_value_type;}
 
    // #range loop const#
    // #for object_t: range loop - non modifiable#
    [[nodiscard]] const auto& object_range() const { return m_obj_values; }
    // #for array_t: range loop - non modifiable#
    [[nodiscard]] const auto& array_range() const { return m_array_values; }

    // #size#
    [[nodiscard]] size_t size_as(const value_type in_expected_type) const noexcept;
    [[nodiscard]] bool  empty_as(const value_type in_expected_type) const noexcept;
    [[nodiscard]] size_t num_elements() const noexcept;

    // #for object_t: key-based access - non modifiable#
    const JsOn& operator[](std::string_view key) const;
    [[nodiscard]] inline size_t count(std::string_view in_key) const noexcept;
    [[nodiscard]] inline bool contains(std::string_view in_key) const noexcept;
    [[nodiscard]] bool contains_as(std::string_view in_key, const enum value_type in_expected_type) const noexcept;

    // #for array_t: key-based access - non modifiable#
    const JsOn& operator[](size_t index) const;
};
}

#endif // __JsOn_h__
