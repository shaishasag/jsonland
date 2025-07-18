#ifndef __jsonland_defs_h__
#define __jsonland_defs_h__

#ifndef DllExport
    #define DllExport
#endif

#include <string_view>
using namespace std::string_view_literals;


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

enum hints : uint32_t
{
    _hint_none = 0,
    _num_is_int = 1<<0,
    _num_in_string = 1<<1,
    _might_contain_escaped_chars = 1<<2
};

struct hint_carrier
{
    mutable hints m_hints{_hint_none};

    hint_carrier() = default;
    hint_carrier(hints _in_hints)
    : m_hints(_in_hints)
    {}
    
    void clear() { m_hints = _hint_none; }
    
    inline void set_hint(hints in_hint) const noexcept
    {
        m_hints =  static_cast<hints>(m_hints | in_hint);
    }
    inline void unset_hint(hints in_hint) const noexcept
    {
        m_hints =  static_cast<hints>(m_hints & ~in_hint);
    }
    
    [[nodiscard]] inline bool get_hint(hints in_hint) const noexcept
    {
        return in_hint == (m_hints & in_hint);
    }
};

/// Default value for a string.
constexpr std::string_view the_empty_string_view{""};
/// String representation of a boolean with value 'false'.
constexpr std::string_view the_false_string_view{"false"};
/// String representation of a boolean with value 'true'.
constexpr std::string_view the_true_string_view{"true"};
/// String representation of a null value.
constexpr std::string_view the_null_string_view{"null"};

template <typename TBool>   concept IsBool = std::same_as<std::decay_t<TBool>, bool>;
template<typename TCHAR>    concept IsChar = std::same_as<std::decay_t<TCHAR>, char>;
template<typename NUM>      concept IsInteger = std::integral<NUM> && !IsBool<NUM> && !IsChar<NUM>;
template<typename FNUM>     concept IsFloat = std::floating_point<FNUM>;
template<typename FNUM>     concept IsNumber = IsInteger<FNUM> || IsFloat<FNUM>;
template<typename FSTR>     concept IsString = std::convertible_to<FSTR, std::string_view>;
template<typename TNULLPTR> concept IsNullPtr = std::is_null_pointer_v<TNULLPTR>;
template<typename TENUM>    concept IsEnum = std::is_enum_v<TENUM>;

template<typename TJSONABLE> concept IsJsonScalarType = IsBool<TJSONABLE>
|| IsInteger<TJSONABLE>
|| IsFloat<TJSONABLE>
|| IsNullPtr<TJSONABLE>
|| IsEnum<TJSONABLE>
|| IsString<TJSONABLE>;

// Concept to check if a type is iterable
template<typename T>
concept Iterable = requires(T container) {
    { container.begin() } -> std::same_as<typename T::iterator>;
    { container.end() } -> std::same_as<typename T::iterator>;
};

// Concept to check if a type is a key-value container
template<typename T>
concept KeyValueContainer = Iterable<T> &&
requires(T container) {
    typename T::value_type;
    { container.begin()->first } -> std::convertible_to<std::string_view>;
    { container.begin()->second };
};

// Concept to check if a type is a sequence container (but not a key-value container)
template<typename T>
concept SequenceContainer = Iterable<T> && !KeyValueContainer<T>;

namespace parser_impl { class Parser; }

enum class dump_style : int32_t
{
    tight = 0,
    pretty = 1,
};

#ifdef JSONLAND_INTERNAL_PARSING_FUNCTIONS

[[nodiscard]] inline bool is_white_space_not_new_line(const char in_c)
{
    return ' ' == in_c  || '\r' == in_c || '\t' == in_c;
}

[[nodiscard]] inline bool is_whitespace(const char in_c)
{
    return is_white_space_not_new_line(in_c) || '\n' == in_c;
}

[[nodiscard]] inline bool is_digit(const char in_c)
{
    const bool retVal = ('0' <= in_c && '9' >= in_c);
    return retVal;
}

[[nodiscard]] inline bool is_hex_digit(const char in_c)
{
    const bool retVal = is_digit(in_c) || ('a' <= in_c && 'f' >= in_c) || ('A' <= in_c && 'F' >= in_c);
    return retVal;
}

[[nodiscard]] inline bool is_num_char(const char in_c)
{
    const bool retVal = ('0' <= in_c && '9' >= in_c) || '.' == in_c || '-' == in_c || '+' == in_c || 'e' == in_c || 'E' == in_c;
    return retVal;
}

[[nodiscard]] inline bool is_separator(const char in_c)
{
    const bool retVal = in_c == '"' || in_c == ',' || in_c == ':' || in_c == ']' || in_c == '}';
    return retVal;
}
[[nodiscard]] inline bool is_illegal_string_char(const char in_c)
{
    const bool retVal = '\0' == in_c;
    return retVal;
}

[[nodiscard]] inline bool is_escapable_char(const char in_c)
{
    const bool retVal = '\\' == in_c ||
    '/' == in_c ||
    '"' == in_c ||
    'b' == in_c ||
    'f' == in_c ||
    'n' == in_c ||
    'r' == in_c ||
    't' == in_c ||
    'u' == in_c;
    return retVal;
}

[[nodiscard]] inline bool is_a_char_that_must_be_escaped(const char in_c)
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

[[nodiscard]] inline std::string_view escapable_to_escaped(const char* in_c)
{
    std::string_view retVal(in_c, 1);
    
    switch(*in_c)
    {
        case '"':  retVal = "\\\""sv; break;
        case '\\':  retVal = "\\\\"sv; break;
        case '\n': retVal = "\\n"sv; break;
        case '\r': retVal = "\\r"sv; break;
        case '\t': retVal = "\\t"sv; break;
        case '\b': retVal = "\\b"sv; break;
        case '\f': retVal = "\\f"sv; break;
        default:
            break;
    }
    
    return retVal;
}

[[nodiscard]] static std::string_view name_of_control_char(const char in_c)
{
    std::string_view retVal = "unknown"sv;
    switch(in_c)
    {
        case '\b': retVal = "backspace"sv; break;
        case '\f': retVal = "formfeed"sv; break;
        case '\n': retVal = "linefeed"sv; break;
        case '\r': retVal = "carriage return"sv; break;
        case '\t': retVal = "horizontal tab"sv; break;
    }
    return retVal;
}

constexpr size_t indent_factor = 4;
static inline void indent(std::string& out_str, size_t level)
{
    out_str.append(level*indent_factor, ' ');
}
static inline void nl_indent(std::string& out_str, size_t level)
{
    out_str += '\n';
    out_str.append(level*indent_factor, ' ');
}

#endif

}

#endif // __jsonland_defs_h__
