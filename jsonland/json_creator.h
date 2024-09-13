#ifndef __json_creator_h__
#define __json_creator_h__

#include <utility>
#include <string.h>
#include <string>
#include <string_view>
#include <charconv>

#include "fstring/fstring.h"
#include "fstring/fstringstream.h"

/* Copy to include
#include "jsonland/json_creator.h"
*/


#ifndef DllExport
    #define DllExport
#endif

namespace jsonland
{


#ifndef OBJECT_WHITESPACE_AFTER_COMMA
    #define OBJECT_WHITESPACE_AFTER_COMMA " "
#endif
constexpr std::string_view ___object_whitespace_after_comma{OBJECT_WHITESPACE_AFTER_COMMA};

#ifndef ARRAY_WHITESPACE_AFTER_COMMA
    #define ARRAY_WHITESPACE_AFTER_COMMA " "
#endif
constexpr std::string_view ___array_whitespace_after_comma{ARRAY_WHITESPACE_AFTER_COMMA};

namespace internal
{
constexpr fstr::fstr7 _TRUE{"true"};
constexpr fstr::fstr7 _FALSE{"false"};
constexpr fstr::fstr7 _NULL{"null"};
constexpr fstr::fstr7 _COMMA{","};
constexpr fstr::fstr7 _KEY_VAL_SEP{R"|(": )|"};

template <typename TStr>
void copy_and_escape(const std::string_view in_text, TStr& in_json_str)
{
    for (auto ch : in_text)
    {
        switch (ch)
        {
            case '"': in_json_str += "\\\""; break;
            case '\\': in_json_str += "\\\\"; break;
            case '\b': in_json_str += "\\b"; break;
            case '\f': in_json_str += "\\f"; break;
            case '\n': in_json_str += "\\n"; break;
            case '\r': in_json_str += "\\r"; break;
            case '\t': in_json_str += "\\t"; break;
            default:
                in_json_str += ch;
        }
    }
}

template <typename TValue, typename TStr>
inline void write_value(const TValue in_value, TStr& in_json_str)
{
    if constexpr (std::is_same_v<bool, TValue>)
    {
        in_json_str += in_value ? _TRUE : _FALSE;
    }
    else if constexpr (std::is_same_v<char, TValue>)
    {
        in_json_str += '"';
        in_json_str += in_value;
        in_json_str += '"';
    }
    else if constexpr (std::is_same_v<std::byte, TValue> ||
                       std::is_integral_v<TValue> ||
                       std::is_floating_point_v<TValue>)
    {
        fstr::fstr127 fs;
        fs.printf(in_value);
        in_json_str += fs;
    }
    else if constexpr (std::is_null_pointer_v<TValue>)
    {
        in_json_str += _NULL;
    }
    else if constexpr (std::is_convertible_v<TValue, std::string_view>)
    {
        in_json_str += '"';
        copy_and_escape(std::string_view(in_value), in_json_str);
        in_json_str += '"';
    }
    else
    {
        // if you got a compilation error here, it means
        // append_value was call on type that is string/number/boolean/null
        // to fix, convert to string before calling append_value
        in_json_str += '"';
        in_json_str += in_value;
        in_json_str += '"';
    }
}

template<typename TStr>
class DllExport base_json_creator
{
    template<typename> // yes, a templated class has to be a friend of itself in order to access protected memebers
    friend class base_json_creator;

private:
    static const size_t t_max_levels{15};

protected:

    // for use by sub-classes that provide their own fstring member
    class PreventAmbiguityOnConstructorCalls{}; // dummy class to help compiler to mark construction from inheriting classes object_json_creator, array_json_creator

    base_json_creator(TStr& in_json_str, PreventAmbiguityOnConstructorCalls) noexcept
    : m_json_str(in_json_str)
    {}

    base_json_creator(TStr& in_json_str, size_t start_level)  noexcept
    : m_json_str(in_json_str)
    , m_level(start_level)
    {}

    TStr m_json_str;

    size_t m_num_subs{0};
    size_t m_level{0};

    std::string_view save_end();
    void restore_end(std::string_view in_end);

    class DllExport save_restore_end
    {
    public:
        save_restore_end(base_json_creator<TStr>& to_save) noexcept;
        ~save_restore_end();

    private:
        base_json_creator<TStr>& m_creator_to_save;
        std::string_view m_saved_end_chars;

        char m_save_buf[t_max_levels+1];
    };

public:

    void reserve( size_t new_cap )
    {
        if constexpr (std::is_same_v<std::string, std::decay_t<TStr>>)
        {
            m_json_str.reserve(new_cap);
        }
        else if constexpr (std::is_same_v<fstr::fstr_ref, TStr>)
        {
            //assert(new_cap <= m_json_str.capacity());
        }
    }


    [[nodiscard]] size_t size() const { return m_json_str.size(); }
    [[nodiscard]] size_t capacity() const { return m_json_str.capacity(); }
    [[nodiscard]] size_t max_size() const { return m_json_str.max_size(); }
    [[nodiscard]] const char* c_str() const { return m_json_str.c_str(); }
    [[nodiscard]] operator std::string_view() const { return m_json_str;}
};

template<typename>
class sub_array_json_creator;

template<typename TStr>
class DllExport sub_object_json_creator : public base_json_creator<TStr>
{
    using target_str_t = TStr;
    using base_creator_t = base_json_creator<TStr>;

    template<typename>
    friend class sub_array_json_creator;

protected:
    using base_type = base_json_creator<TStr>;

    constexpr static inline std::string_view empty_json_object{"{}"};

    void prepare_for_additional_value(const std::string_view in_key);

    // this constructor should only be called from object_json_creator::append_object or array_json_creator::append_object
    sub_object_json_creator(TStr& the_str, size_t start_level) noexcept
    : base_type(the_str, start_level)
    {
        this->m_json_str += empty_json_object;
    }

    // this constructor should only be called from object_json_creator::object_json_creator
    // the purpose of this constructor is to avoid using m_json_fstring_ref before it was intialized
    // by object_json_creator
    // for use by sub-classes that provide their own fstring member
    sub_object_json_creator(TStr&& in_fstr_to_refer_to,
                            typename base_type::PreventAmbiguityOnConstructorCalls in_unambig) noexcept
    : base_type(in_fstr_to_refer_to, in_unambig)
    {}

public:

    [[nodiscard]] sub_object_json_creator append_object(std::string_view in_key);
    [[nodiscard]] sub_array_json_creator<TStr> append_array(std::string_view in_key);

    // add a value that is already formated as json to the end of the object
    // useful in case you have a string containing a number and you want to
    // append it as number (i.e. without quotes).
    void append_json_str(const std::string_view in_key, const std::string_view in_value);

    // add a value that is already formated as json to the begening of the object
    // Warning: less efficient than append_json_str!
    void prepend_json_str(const std::string_view in_key, const std::string_view in_value);

    void append_value() {}

    template <typename TValue>
    void append_value(const std::string_view in_key, const TValue& in_value)
    {
        typename base_json_creator<TStr>::save_restore_end sv(*this);
        prepare_for_additional_value(in_key);
        internal::write_value(in_value, this->m_json_str);
    }

    void append_value(const std::string_view in_key, const char* in_value)
    {
        std::string_view as_sv(in_value);
        append_value<std::string_view>(in_key, as_sv);
    }

    template<typename TValue, typename... Args>
    void append_value(std::string_view in_key, const TValue& in_value, Args&&... args)
    {
        append_value(in_key, in_value);
        append_value(std::forward<Args>(args)...);
    }

    template<typename TContainer>
    void extend(const TContainer& in_val_key_container)
    {
        for (auto& [name, value] : in_val_key_container)
        {
            append_value(std::string_view(name), value);
        }
    }

    void append_values_from(const sub_object_json_creator& in_to_merge_from);
    void prepend_values_from(const sub_object_json_creator& in_to_merge_from);

    [[nodiscard]] bool empty() const { return this->m_json_str[1] == empty_json_object[1]; }
    void clear() { this->m_json_str = empty_json_object; this->m_num_subs = 0; }

    // reset with string containing json
    void reset(std::string_view in_value)
    {
        if (!in_value.empty())
        {
            this->m_json_str = in_value;
        }
        else
        {
            clear();
        }
    }


    class DllExport operator_square_brackets_helper
    {
        friend class sub_object_json_creator;
    private:
        sub_object_json_creator& m_creator;
        std::string_view m_key;
    protected:
        operator_square_brackets_helper(sub_object_json_creator& in_creator,
                                        std::string_view in_key)
        : m_creator(in_creator)
        , m_key(in_key) {}
    public:
        template <typename TValue>
        void operator=(const TValue& in_value)
        {
            m_creator.append_value(m_key, in_value);
        }
    };

    operator_square_brackets_helper operator[](std::string_view in_key)
    {
        return operator_square_brackets_helper(*this, in_key);
    }
};



template<typename TStr>
class DllExport sub_array_json_creator : public base_json_creator<TStr>
{
    template<typename>
    friend class sub_object_json_creator;

protected:
    using base_type = base_json_creator<TStr>;

    constexpr static inline std::string_view empty_json_array{"[]"};

    void prepare_for_additional_value();

    sub_array_json_creator(TStr& the_str, size_t start_level) noexcept
    : base_type(the_str, start_level)
    {
        this->m_json_str += empty_json_array;
    }

    // this constructor should only be called from object_json_creator::object_json_creator
    // the purpose of this constructor is to avoid using m_json_fstring_ref before it was intialized
    // by object_json_creator
    // for use by sub-classes that provide their own fstring member
    sub_array_json_creator(TStr&& in_fstr_to_refer_to,
                           typename base_json_creator<TStr>::PreventAmbiguityOnConstructorCalls in_unambig) noexcept
    : base_type(in_fstr_to_refer_to, in_unambig)
    {}

public:

    [[nodiscard]] sub_array_json_creator append_array();
    [[nodiscard]] sub_object_json_creator<TStr> append_object();

    // add a value that is already formated as json
    void append_json_str(const std::string_view in_value);

    // add a value that is already formated as json to the begening of the array
    // Warning: less efficient than append_json_str!
    void prepend_json_str(const std::string_view in_value);

    void append_value() {}
    void append_value(const char* in_value);

    template <typename TValue>
    void append_value(const TValue& in_value)
    {
        typename sub_array_json_creator::save_restore_end sv(*this);
        prepare_for_additional_value();
        internal::write_value(in_value, this->m_json_str);
    }

    template<typename TValue, typename... Args>
    void append_value(const TValue& in_value, Args&&... args)
    {
        append_value(in_value);
        append_value(std::forward<Args>(args)...);
    }

    template<typename TContainer>
    void extend(const TContainer& in_values_container)
    {
        for (const auto& value : in_values_container)
        {
            append_value(value);
        }
    }

    void append_values_from(const sub_array_json_creator& in_to_merge_from);
    void prepend_values_from(const sub_array_json_creator& in_to_merge_from);

    template <typename TValue>
    void push_back(const TValue& in_value)  // alias to append_value for compatibility with code converted from other json creators
    {
        append_value(in_value);
    }

    [[nodiscard]] bool empty() const { return this->m_json_str[1] == empty_json_array[1]; }
    void clear()
    {
        this->m_json_str = empty_json_array;
        this->m_num_subs = 0;
    }

    // reset with string containing json
    void reset(std::string_view in_value)
    {
        if (!in_value.empty())
        {
            this->m_json_str = in_value;
        }
        else
        {
            clear();
        }
    }

};
} // namespace internal

namespace fixed // creators based on fstring
{
using sub_object_json_creator_t = internal::sub_object_json_creator<fstr::fstr_ref>;
using sub_array_json_creator_t = internal::sub_array_json_creator<fstr::fstr_ref>;

template<size_t STRING_CAPACITY=511>
class DllExport object_json_creator : public sub_object_json_creator_t
{
public:
    object_json_creator() noexcept
    : sub_object_json_creator_t(m_fstr,
                              base_json_creator<fstr::fstr_ref>::PreventAmbiguityOnConstructorCalls())
    {}

protected:
    fstr::fstring_base<STRING_CAPACITY, char> m_fstr{empty_json_object};
};

template<size_t STRING_CAPACITY=511>
class DllExport array_json_creator : public sub_array_json_creator_t
{
public:
    array_json_creator() noexcept
    : sub_array_json_creator_t(fstr::fstr_ref(m_fstr),
                             base_json_creator<fstr::fstr_ref>::PreventAmbiguityOnConstructorCalls())
    {}

protected:
    fstr::fstring_base<STRING_CAPACITY, char> m_fstr{empty_json_array};
};

} // namespace fixed

template<typename TStr>
inline std::ostream& operator<<(std::ostream& os,
                                const internal::base_json_creator<TStr>& in_j_c)
{
    const std::string_view as_sv(in_j_c);
    os << as_sv;
    return os;
}

namespace dyna
{

using sub_object_json_creator_t = internal::sub_object_json_creator<std::string&>;
using sub_array_json_creator_t = internal::sub_array_json_creator<std::string&>;

class DllExport object_json_creator : public sub_object_json_creator_t
{
public:
    object_json_creator() noexcept
    : sub_object_json_creator_t(m_fstr,
                              base_json_creator<std::string&>::PreventAmbiguityOnConstructorCalls())
    {}

protected:
    std::string m_fstr{empty_json_object};
};

class DllExport array_json_creator : public sub_array_json_creator_t
{
public:
    array_json_creator() noexcept
    : sub_array_json_creator_t(m_fstr,
                               base_json_creator<std::string&>::PreventAmbiguityOnConstructorCalls())
    {}

protected:
    std::string m_fstr{empty_json_array};
};

} // namespace dyna

} // namespace jsonland

namespace jl_fixed = jsonland::fixed;
namespace jl_dyna = jsonland::dyna;

#endif // __json_creator_h__
