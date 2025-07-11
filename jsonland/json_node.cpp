#include "jsonland/json_node.h"

#include <array>
#include <iterator>
#include <cstdio>
#include <charconv>
#include <cstring>

using namespace jsonland;

#if JSONLAND_DEBUG==1
#endif

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

json_node::json_node(jsonland::value_type in_type, size_t in_reserve) noexcept
: m_value_type(in_type)
{
    switch (m_value_type)
    {
        case null_t:
            m_value.set_without_ownership(the_null_string_view);
        break;
        case number_t:
            m_value.set_without_ownership(the_empty_string_view);
        break;
        case bool_t:
            m_value.set_without_ownership(the_false_string_view);
        break;
        case object_t:
            reserve(in_reserve);
        break;
        case array_t:
            reserve(in_reserve);
        break;
        default:
            m_value.set_without_ownership(the_empty_string_view);
        break;
    }
}

json_node::json_node(const std::string_view in_str_value, jsonland::value_type in_type) noexcept
: m_value_type(in_type)
, m_value(in_str_value)
{
    if (m_value_type == number_t) {
        set_hint(_num_in_string);
        // (C++23): in_str_value.contains('.')
        if (auto pos = in_str_value.find('.');
            pos==std::string_view::npos)
        {
            set_hint(_num_is_int);
        }
    }
}

void json_node::clear(const value_type in_new_type) noexcept
{
    m_value_type = in_new_type;
    m_obj_key_to_index.clear();
    m_values.clear();

    switch (m_value_type)
    {
        case null_t:
            m_value.set_without_ownership(the_null_string_view);
        break;
        case number_t:
            m_value.set_without_ownership(the_empty_string_view);
        break;
        case bool_t:
            m_value.set_without_ownership(the_false_string_view);
        break;
        default:
            m_value.set_without_ownership(the_empty_string_view);
        break;
    }

    m_num = 0.0;
    m_hints = _hint_none;
}

size_t json_node::num_elements() const noexcept
{
    size_t retVal = 0;
    switch (get_value_type())
    {
        case object_t: retVal = m_values.size(); break;
        case array_t: retVal = m_values.size(); break;
        default:
            break;

    }
    return retVal;
}

size_t json_node::size_as(const enum value_type in_expected_type) const noexcept
{
    size_t retVal = 0;
    if (in_expected_type == get_value_type())
    {
        switch (get_value_type())
        {
            case object_t:
                retVal = size_as<object_t>(); break;
            case array_t:
                retVal =  size_as<array_t>(); break;
            case string_t:
                retVal = size_as<string_t>(); break;
            case bool_t:
            case number_t:
                retVal = 1; break;
            case null_t:
                retVal = 0; break;
            default:
                retVal = 0;
                break;

        }
    }
    return retVal;
}

void json_node::reserve(const size_t new_cap) noexcept
{
    if (is_array() || is_object()) [[likely]]
    {
        m_values.reserve(new_cap);
        if (is_object())
        {
            m_obj_key_to_index.reserve(new_cap);
        }
    }
}

bool json_node::contains_as(std::string_view in_key, const enum value_type in_expected_type) const noexcept
{
    bool retVal{false};

    if (contains(in_key))
    {
        const json_node& theJ = m_values[m_obj_key_to_index.at(string_and_view(in_key, 0))];
        retVal = in_expected_type == theJ.m_value_type;
    }

    return retVal;
}

json_node& json_node::operator[](std::string_view in_key) noexcept
{
    if (is_null())
    {
        clear(object_t);
    }

    if (is_object()) [[likely]]
    {
        int index = -1;
        string_and_view key(in_key, 0);

        if (0 == m_obj_key_to_index.count(key))
        {
            index = static_cast<int>(m_values.size());
            json_node& value = m_values.emplace_back();
            value.m_key = key;

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

 const json_node& json_node::operator[](std::string_view in_str) const noexcept
 {
     if (is_object()) [[likely]]
     {
         string_and_view key(in_str, 0);
         if (contains(key)) {
             return m_values[m_obj_key_to_index.at(key)];
         }
         else {
             return const_uninitialized_json_node();
         }
     }
     else
         return const_uninitialized_json_node();  // what to return here?
}

size_t json_node::erase(std::string_view in_key)
{
    size_t retVal{0};
    if (is_object()) [[likely]]
    {
        string_and_view key(in_key, 0);
        if (auto iKey = m_obj_key_to_index.find(key);
        iKey != m_obj_key_to_index.end())
        {
            int index = iKey->second;
            m_obj_key_to_index.erase(iKey);
            m_values.erase(m_values.begin() + index);

            // reassign indexis
            for (auto& [aKey, anIndex] : m_obj_key_to_index)
            {
                if (anIndex > index)
                {
                    --anIndex;
                }
            }

            retVal = 1;
        }
    }

    return retVal;
}

size_t json_node::erase(size_t in_index)
{
    size_t retVal{0};
    if (is_array()) [[likely]]
    {
        if (in_index < m_values.size())
        {
            m_values.erase(m_values.begin() + in_index);
            retVal = 1;
        }
    }

 return retVal;
}

// append object to object
json_node& json_node::append_object(std::string_view in_key, size_t in_reserve)
{
 if (is_object()) [[likely]]
 {
     json_node& retVal = this->operator[](in_key);
     retVal = object_t;
     retVal.reserve(in_reserve);

     return retVal;
 }
 else
     return *this;  // what to return here?
}

// append array to object
json_node& json_node::append_array(std::string_view in_key, size_t in_reserve)
{
    if (is_object()) [[likely]]
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
[[nodiscard]] std::vector<std::string_view> json_node::keys()
{
    std::vector<std::string_view> retVal;
    retVal.reserve(m_obj_key_to_index.size());
    for (auto& key2indexItem : m_obj_key_to_index)
    {
        retVal.push_back(key2indexItem.first.sv());
    }
    return retVal;
}

/// functions for JSON value type #array_t

json_node& json_node::emplace_back() noexcept
{
    if (is_null())
    {
        clear(array_t);
    }
    m_values.emplace_back();
    return m_values.back();
}

json_node& json_node::push_back(const json_node& in_node) noexcept
{
    if (is_null())
    {
        clear(array_t);
    }
    m_values.emplace_back(in_node);
    return m_values.back();
}

json_node& json_node::push_back(json_node&& in_node) noexcept
{
    if (is_null())
    {
        clear(array_t);
    }
    m_values.emplace_back(std::move(in_node));
    return m_values.back();
}




// dummy uninitialized json_node to be returned from operator[] const; where the
// object/array doe snot contain the key/index
const json_node& json_node::const_uninitialized_json_node() const
{
    static const json_node the_const_uninitialized_json_node(jsonland::uninitialized_t);
    return the_const_uninitialized_json_node;
}

json_node& json_node::append_object(size_t in_reserve)
{
    if (is_array()) [[likely]]
    {
        json_node& retVal = m_values.emplace_back(object_t);
        retVal.reserve(in_reserve);

        return retVal;
    }
    else
        return *this;  // what to return here?
}

json_node& json_node::append_array(size_t in_reserve)
{
    if (is_array())  [[likely]]
    {
        json_node& retVal = m_values.emplace_back(array_t);
        retVal.reserve(in_reserve);

        return retVal;
    }
    else
        return *this;  // what to return here?
}


std::ostream& json_node::dump(std::ostream& os,
                              dump_style in_style) const noexcept
{
    std::string str;
    dump(str, in_style);
    os.write(str.c_str(), str.size());
    return os;
}

template<typename TToPrintf>
static int __printf(char* in_buff,
                    const size_t in_buff_size,
                    const TToPrintf in_to_print,
                    const char* printf_format=nullptr) noexcept
{
    bool remove_zeros{false};
    if (nullptr == printf_format)
    {
        if constexpr (IsFloat<TToPrintf>) {
            if constexpr (std::is_same_v<TToPrintf, float>) {
                printf_format = "%.10f"; // std::numeric_limits<float>::max_digits10+1
            }
            else if constexpr (std::is_same_v<TToPrintf, double>) {
                printf_format = "%.18lf"; // std::numeric_limits<double>::max_digits10+1
            }
            else if constexpr (std::is_same_v<TToPrintf, long double>) {
                printf_format = "%.18Lf"; // std::numeric_limits<long double>::max_digits10+1
            }
            remove_zeros = true;
        }
        else if constexpr (IsInteger<TToPrintf> && std::signed_integral<TToPrintf>) {
            printf_format = "%lli";
        }
        else if constexpr (IsInteger<TToPrintf> && std::unsigned_integral<TToPrintf>) {
            printf_format = "%llu";
        }
    }

    int num_chars = std::snprintf(in_buff, in_buff_size, printf_format, in_to_print);
    if (in_buff_size == (size_t)num_chars)
    {
        // this means there was not enough room for the number
        // but final '\0' was printed anyway so actualy there are num_chars-1 chars
        --num_chars;
    }

    if (remove_zeros && num_chars > 1)// when num_chars==1 there are no extra '0's
    {
        const bool is_dot_found = (memchr(in_buff, '.', num_chars-1) != nullptr);
        if (is_dot_found)
        {
            while (*(in_buff+(num_chars-1)) == '0') {// remove trailing zeros - if any
                --num_chars;
            }
            if (*(in_buff+(num_chars-1)) == '.') {  // if . came before 0, leave the last 0 so number will look like 2.0
                ++num_chars;
            }
        }
    }


    return num_chars;
}


void json_node::dump(std::string& out_str,
                     dump_style in_style) const noexcept
{
    if (in_style == dump_style::pretty) {
        dump_pretty(out_str);
    }
    else {
        dump_tight(out_str);
    }
}

const size_t indent_factor = 4;
static void indent(std::string& out_str, size_t level)
{
    out_str.append(level*indent_factor, ' ');
}
static void nl_indent(std::string& out_str, size_t level)
{
    out_str += '\n';
    out_str.append(level*indent_factor, ' ');
}

void json_node::dump_pretty(std::string& out_str, size_t level) const noexcept
{
    if (is_object())
    {
        out_str += '{';

        if (!m_values.empty())
        {
            ++level;
            nl_indent(out_str, level);

            auto io = m_values.begin();
            out_str += '"';
            io->m_key.escape_json_string_external(out_str);
            out_str += '"';

            if (io->m_key.sv().compare("escaped_characters") == 0)
            {
                std::cout << io->m_key.sv() << std::endl;
            }
            out_str += ':';
            if (!io->empty_as<object_t>() || !io->empty_as<array_t>())
            {
                out_str += '\n';
                indent(out_str, level);
            }
            else
            {
                out_str += ' ';
            }
            io->dump_pretty(out_str, level);

            for (++io; io != m_values.end(); ++io)
            {
                out_str += ',';
                nl_indent(out_str, level);

                out_str += '"';
                io->m_key.escape_json_string_external(out_str);
                out_str += '"';

                out_str += ':';
                if (!io->empty_as<object_t>() || !io->empty_as<array_t>())
                {
                    out_str += '\n';
                    indent(out_str, level);
                }
                else
                {
                    out_str += ' ';
                }
                io->dump_pretty(out_str, level);
            }
            --level;
            nl_indent(out_str, level);
        }
        out_str += '}';
    }
    else if (is_array())
    {
        out_str += '[';
        if (!m_values.empty())
        {
            ++level;
            nl_indent(out_str, level);

            auto ai = m_values.begin();
            ai->dump_pretty(out_str, level);

            for (++ai; ai != m_values.end(); ++ai) {
                out_str += ',';
                nl_indent(out_str, level);
                ai->dump_pretty(out_str, level);
            }
            --level;
            out_str += '\n';
            indent(out_str, level);
        }
        out_str += ']';
    }
    else if (is_number())
    {  // where art thou Grisu2 ?
        if (is_num_in_string())
        {
            out_str += m_value.sv();
        }
        else
        {
            char buff[128]{'\0'};
            if (get_hint(_num_is_int)) {
                size_t num_chars = __printf(buff, 127, get_int<int64_t>());
                out_str.append(buff, num_chars);
            }
            else
            {
                // no to_chars(... double) in clang
                //auto res = std::to_chars(buff, buff+30, get_float(), std::chars_format::fixed);
                //out_str.append(buff, res.ptr-buff);
                // resort to printf...
                size_t num_chars = __printf(buff, 127, get_float<long double>());
                out_str.append(buff, num_chars);
            }
        }
    }
    else if (is_string())
    {
        out_str += '"';
        if (get_hint(_might_contain_escaped_chars))
        {
            out_str += m_value.sv();
        }
        else
        {
            m_value.escape_json_string_external(out_str);
        }
        out_str += '"';
    }
    else
    {
        out_str += m_value.sv();
    }
}

void json_node::dump_tight(std::string& out_str) const noexcept
{
    if (is_object())
    {
        out_str += '{';
        if (!m_values.empty())
        {
            auto io = m_values.begin();
            out_str += '"';
            io->m_key.escape_json_string_external(out_str);
            out_str += '"';

            out_str += ':';
            io->dump_tight(out_str);

            for (++io; io != m_values.end(); ++io)
            {
                out_str += ',';
                out_str += '"';
                io->m_key.escape_json_string_external(out_str);
                out_str += '"';
                out_str += ':';
                io->dump_tight(out_str);
            }
        }
        out_str += '}';
    }
    else if (is_array())
    {
        out_str += '[';
        if (!m_values.empty())
        {
            auto ai = m_values.begin();
            ai->dump_tight(out_str);

            for (++ai; ai != m_values.end(); ++ai) {
                out_str += ',';
                ai->dump_tight(out_str);
            }
        }
        out_str += ']';
    }
    else if (is_number())
    {  // where art thou Grisu2 ?
        if (is_num_in_string())
        {
            out_str += m_value.sv();
        }
        else
        {
            char buff[128]{'\0'};
            if (get_hint(_num_is_int)) {
                size_t num_chars = __printf(buff, 127, get_int<int64_t>());
                out_str.append(buff, num_chars);
            }
            else
            {
                // no to_chars(... double) in clang
                //auto res = std::to_chars(buff, buff+30, get_float(), std::chars_format::fixed);
                //out_str.append(buff, res.ptr-buff);
                // resort to printf...
                size_t num_chars = __printf(buff, 127, get_float<long double>());
                out_str.append(buff, num_chars);
            }
        }
    }
    else if (is_string())
    {
        out_str += '"';
        if (get_hint(_might_contain_escaped_chars))
        {
            out_str += m_value.sv();
        }
        else
        {
            m_value.escape_json_string_external(out_str);
        }
        out_str += '"';
    }
    else
    {
        out_str += m_value.sv();
    }
}

std::string json_node::dump(dump_style in_style) const noexcept
{
    std::string retVal;
    dump(retVal, in_style);
    return retVal;
}

std::string_view json_node::get_string(std::string_view in_default_str) const noexcept
{
    if (is_string())
    {
        if (get_hint(_might_contain_escaped_chars))
        {
            m_value.unescape_json_string_internal(); // will not allocate if string does not contain escapes
            unset_hint(_might_contain_escaped_chars);
        }
        return as_string_view();
    }
    else
        return in_default_str;
}

void json_node::parser_direct_set(const std::string_view in_str, jsonland::value_type in_type)
{
    // add asserts that m_obj_key_to_index & m_values are empty
    m_value_type = in_type;
    m_value.set_without_ownership(in_str);
}

void json_node::json_node::parser_direct_set(jsonland::value_type in_type)
{
    // add asserts that m_obj_key_to_index & m_values are empty
    m_value_type = in_type;
}

size_t json_node::memory_consumption() const noexcept
{
    size_t retVal{0};

    // add allocation by this object's own value
    retVal += m_value.allocation_size();
    retVal += m_key.allocation_size();

    // add allocations by child elements if array or object
    for (const json_node& val : m_values)
    {
        retVal += sizeof(ArrayVec::value_type);
        retVal += val.memory_consumption();
    }

    // if m_values has more capcity than is actually used...
    retVal += (m_values.capacity() - m_values.size()) * sizeof(ArrayVec::value_type);

    // add allocations by keys values if object
    for (const KeyToIndex::value_type& val : m_obj_key_to_index)
    {
        retVal += sizeof(KeyToIndex::value_type);
        retVal += val.first.allocation_size();
    }

    return retVal;
}

// return true if any of m_key or m_value or one of the m_values refers to external memory.
// refering to external memory happens when calling json_doc.parse_insitue or son_doc.parse.
// note that json_doc.parse will also result in this function returning true
// because the memory is held by the top json_doc object not each json_node.
bool json_node::refers_to_external_memory() const noexcept
{
    bool retVal{true};

    switch (m_value_type)
    {
        case jsonland::value_type::uninitialized_t:
        case jsonland::value_type::null_t:
        case jsonland::value_type::bool_t:
            retVal = false; // these types always refer to const string_view(s)
        break;
        case jsonland::value_type::number_t:
        case jsonland::value_type::string_t:
            retVal = (m_value.refers_to_external_memory())
                        || (m_key.refers_to_external_memory());
        break;
        case jsonland::value_type::array_t:
        case jsonland::value_type::object_t:
            retVal = std::any_of(m_values.begin(),
                                 m_values.end(),
                                 [](const auto& sub_val)
                                    {return sub_val.refers_to_external_memory();} );
        break;
    }

    return retVal;
}

// return true if all of m_key and m_value and all of the m_values do not refers to external memory
// unless json_doc.parse_insitue or json_doc.parse where called, json_node should always be full owner.
// calling json_doc/json_node.clone() should always produce a copy with full ownership.
// calling std::move(json_doc/json_node) will copy the ownership of the source object.
bool json_node::is_full_owner() const noexcept
{
    bool retVal{false};

    switch (m_value_type)
    {
        case jsonland::value_type::uninitialized_t:
        case jsonland::value_type::null_t:
        case jsonland::value_type::bool_t:
            retVal = true;
        break;
        case jsonland::value_type::number_t:
        case jsonland::value_type::string_t:
            retVal = ! m_value.refers_to_external_memory()
                    && ! m_key.refers_to_external_memory();
        break;
        case jsonland::value_type::array_t:
        case jsonland::value_type::object_t:
            retVal = std::all_of(m_values.begin(),
                                 m_values.end(),
                                 [](const auto& sub_val)
                                    {return sub_val.is_full_owner();} );
        break;
    }

    return retVal;
}

void json_node::take_ownership() noexcept
{
    m_key.take_ownership();
    m_value.take_ownership();

    switch (m_value_type)
    {
        case jsonland::value_type::uninitialized_t:
        case jsonland::value_type::null_t:
        case jsonland::value_type::bool_t:
        case jsonland::value_type::number_t:
        case jsonland::value_type::string_t:
        break;
        case jsonland::value_type::array_t:
        case jsonland::value_type::object_t:

            for (auto& k : m_obj_key_to_index)
            {
                const_cast<string_and_view&>(k.first).take_ownership();
            }

            for (auto& val : m_values)
            {
                val.take_ownership();
            }
        break;
    }
}

void json_node::assign_from(const json_node& src)
{
    if (this == std::addressof(src)) {
        return;
    }

    m_value_type = src.m_value_type;
    m_value = src.m_value;
    m_num = src.m_num;
    m_values.clear();
    m_values.reserve(src.m_values.size());
    for (auto& jn : src.m_values)
    {
        m_values.push_back(jn.clone());
    }

    m_obj_key_to_index = src.m_obj_key_to_index;
    m_hints = src.m_hints;
}

void json_node::assign_from(json_node&& src)
{
    if (this == std::addressof(src)) {
        return;
    }

    m_value_type = src.m_value_type;
    m_value = std::move(src.m_value);
    m_num = src.m_num;
    std::swap(m_values, src.m_values);
    m_obj_key_to_index = std::move(src.m_obj_key_to_index);
    m_hints = src.m_hints;

    src.clear();
}

void json_node::merge_from(const json_node& in_to_merge)
{
    if (this == std::addressof(in_to_merge) || !is_object() || !in_to_merge.is_object()) {
        return;
    }

    for (auto& item : in_to_merge)
    {
        this->operator[](item.key()).assign_from(item);
    }
}

void json_node::merge_from(json_node&& in_to_merge)
{
    if (this == std::addressof(in_to_merge) || !is_object() || !in_to_merge.is_object()) {
        return;
    }

    for (auto& item : in_to_merge)
    {
        this->operator[](item.key()).assign_from(std::move(item));
    }
    in_to_merge.clear();
}

namespace jsonland
{

constexpr std::string_view value_type_name(jsonland::value_type in_type)
{
    std::string_view retVal = "Unknown"sv;
    switch (in_type)
    {
        case jsonland::value_type::uninitialized_t: retVal = "uninitialized_t"sv; break;
        case jsonland::value_type::null_t: retVal = "null_t"sv; break;
        case jsonland::value_type::bool_t: retVal = "bool_t"sv; break;
        case jsonland::value_type::number_t: retVal = "number_t"sv; break;
        case jsonland::value_type::string_t: retVal = "string_t"sv; break;
        case jsonland::value_type::array_t: retVal = "array_t"sv; break;
        case jsonland::value_type::object_t: retVal = "object_t"sv; break;
    }
    return retVal;
}

namespace parser_impl
{


    // case_insensitive string compare
    // When this function is called, str_left must have at least str_right.size() before terminating '\0'
    inline bool str_compare_case_sensitive_n(const char* str_left, std::string_view str_right)
    {
        const char* str_leftp = str_left;
        const char* str_rightp = str_right.data();
        size_t chars_left = str_right.size();
        while (chars_left > 0) [[likely]]
        {
            if (*str_leftp != *str_rightp)
                break;
            ++str_leftp;
            ++str_rightp;
            --chars_left;
        }

        bool retVal = 0 == chars_left;
        return retVal;
    }

    // case_sensitive string compare
    // When this function is called, str_left must have at least str_right.size() before terminating '\0'
    // str_right is also assumed to contain only lower case chars
    inline bool str_compare_case_insensitive_n(const char* str_left, std::string_view str_right)
    {
        const char* str_leftp = str_left;
        const char* str_rightp = str_right.data();
        size_t chars_left = str_right.size();
        while (chars_left > 0) [[likely]]
        {
            if (std::tolower(*str_leftp) != *str_rightp)
                break;
            ++str_leftp;
            ++str_rightp;
            --chars_left;
        }

        bool retVal = 0 == chars_left;
        return retVal;
    }

    static const char end_mark = '\0';

    class Parser
    {
    public:

        struct location_in_string
        {
            size_t m_line = 0;
            size_t m_offset = 0;
        };

        enum parsing_value_type : uint32_t  // for parsing only
        {
            _uninitialized = jsonland::value_type::uninitialized_t,
            _null = jsonland::value_type::null_t,
            _bool = jsonland::value_type::bool_t,
            _num = jsonland::value_type::number_t,
            _str = jsonland::value_type::string_t,
            _array = jsonland::value_type::array_t,
            _obj = jsonland::value_type::object_t,

            _comma = 1 << 7,
            _colon = 1 << 8,
            _array_close = 1 << 9,
            _obj_close = 1 << 10,

            _scalar = _null|_bool|_num|_str,
            _value = _scalar|_array|_obj,

            _value_or_array_close = _value|_array_close,
            _comma_or_array_close = _comma|_array_close,

            _value_or_object_close = _value|_obj_close,
            _str_or_obj_close = _str|_obj_close,
            _comma_or_obj_close = _comma|_obj_close,

            _any = ~_uninitialized,
        };

        class parsing_exception : public std::exception
        {
        public:
            parsing_exception(const location_in_string& in_location,
                              std::initializer_list<std::string_view> in_message_parts)
            {
                for (auto message_part : in_message_parts)
                {
                    m_message += message_part;
                }

                m_message += "' line ";
                m_message += std::to_string(in_location.m_line);

                m_message += " at offset ";
                m_message += std::to_string(in_location.m_offset);

                m_message += ";";
            }

            const char* what() const noexcept override
            {
                return m_message.c_str();
            }

            std::string m_message;
        };

        class parsing_exception_maximum_nesting_level : public parsing_exception
        {
        public:
            parsing_exception_maximum_nesting_level(const location_in_string& in_location,
                                                    size_t max_nesting_level)
            : parsing_exception(in_location, {"Reached maximum nesting level of ", std::to_string(max_nesting_level)})
            {
            }
        };

        Parser(json_doc& in_top_node, char* in_start, char* in_end)
        : m_top_node(in_top_node)
        , m_start(in_start)
        , m_curr_char_p(in_start)
        , m_end(in_end)
        , m_str_size(in_end-in_start)
        , m_offset_of_line(in_start)
        {
            prepare_char_to_token_table();
            m_array_values_stack.reserve(1024);
            m_obj_keys_stack.reserve(1024);
        }

    private:
        json_doc& m_top_node;
        char m_curr_char{end_mark};
        [[maybe_unused]] const char* m_start = nullptr;
        char* m_curr_char_p = nullptr;
        const char* m_end = nullptr;

        [[maybe_unused]] const size_t m_str_size;

        size_t m_max_nesting_level = 64;
        size_t m_nesting_level = 0;

        std::string m_json_text;

        const char* m_offset_of_line = nullptr;
        size_t m_current_line = 1;
        int m_parse_error = 0;
        std::string m_parse_error_message;

        json_node::ArrayVec m_array_values_stack;
        std::vector<jsonland::string_and_view> m_obj_keys_stack;

        inline void skip_chars(size_t in_num_chars_to_skip)
        {
            for (int i = 0; i < static_cast<int>(in_num_chars_to_skip) && next_char(); ++i)
            {
            }
        }

        inline location_in_string curr_offset()
        {
            return location_in_string{m_current_line, (size_t)(m_curr_char_p - m_offset_of_line)};
        }

        inline bool is_there_more_data()
        {
            return m_curr_char_p < m_end;
        }

        inline size_t num_remaining_chars()
        {
           return m_end - m_curr_char_p;
        }

        struct func_type_pair
        {
            bool (Parser::*m_func)(json_node& out_node);
            parsing_value_type m_type;
        };
        func_type_pair m_char_to_token_table[256];

        void prepare_char_to_token_table()
        {
            for (int i = 0; i < 256; ++i)
                m_char_to_token_table[i] = {&Parser::skip_one_char, parsing_value_type::_uninitialized};

             m_char_to_token_table[(int)'['] = {&Parser::parse_array, parsing_value_type::_array};
             m_char_to_token_table[(int)']'] = {&Parser::parse_control_char, parsing_value_type::_array_close};
             m_char_to_token_table[(int)','] = {&Parser::parse_control_char, parsing_value_type::_comma};
             m_char_to_token_table[(int)'{'] = {&Parser::parse_obj, parsing_value_type::_obj};
             m_char_to_token_table[(int)'}'] = {&Parser::parse_control_char, parsing_value_type::_obj_close};
             m_char_to_token_table[(int)':'] = {&Parser::parse_control_char, parsing_value_type::_colon};
             m_char_to_token_table[(int)'"'] = {&Parser::parse_string, parsing_value_type::_str};
             m_char_to_token_table[(int)'0'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'1'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'2'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'3'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'4'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'5'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'6'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'7'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'8'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'9'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'-'] = {&Parser::parse_number, parsing_value_type::_num};
             m_char_to_token_table[(int)'f'] = {&Parser::parse_constant, parsing_value_type::_bool};
             m_char_to_token_table[(int)'n'] = {&Parser::parse_constant, parsing_value_type::_null};
             m_char_to_token_table[(int)'t'] = {&Parser::parse_constant, parsing_value_type::_bool};
             m_char_to_token_table[(int)'\n'] = {&Parser::skip_new_line, parsing_value_type::_uninitialized};
             m_char_to_token_table[(int)'\r'] = {&Parser::skip_whilespace, parsing_value_type::_uninitialized};
             m_char_to_token_table[(int)'\t'] = {&Parser::skip_whilespace, parsing_value_type::_uninitialized};
             m_char_to_token_table[(int)' '] = {&Parser::skip_whilespace, parsing_value_type::_uninitialized};
#if 0 // case insensitive constants
            // m_char_to_token_table[(int)'F'] =  m_char_to_token_table[(int)'f'];
            // m_char_to_token_table[(int)'N'] =  m_char_to_token_table[(int)'n'];
            // m_char_to_token_table[(int)'T'] =  m_char_to_token_table[(int)'t'];
#endif
        }

        inline bool get_next_node(json_node& out_node,
                                  parsing_value_type& out_type,
                                  parsing_value_type expecting=parsing_value_type::_any)
        {
            assert(m_curr_char != end_mark);

            bool retVal = false;
            while (!retVal)  [[likely]]
            {
                if (256 > (unsigned int)m_curr_char)
                {
                    func_type_pair& call_pair = m_char_to_token_table[(int)m_curr_char];
                    out_type = call_pair.m_type;
                    retVal = (this->*call_pair.m_func)(out_node);
                    if (retVal && (call_pair.m_type & expecting) == 0)
                    {
                        throw parsing_exception(curr_offset(), {"json syntax error: unexpected token"sv});
                    }
                }
                else
                {
                    func_type_pair& call_pair = m_char_to_token_table[0];
                    out_type = call_pair.m_type;
                    retVal = (this->*call_pair.m_func)(out_node);
                    if (retVal && (call_pair.m_type & expecting) == 0)
                    {
                        throw parsing_exception(curr_offset(), {"json syntax error: unexpected token"sv});
                    }
                }
                out_node.set_hint(json_node::_might_contain_escaped_chars);
            }
            return retVal;
        }

        bool parse_string(json_node& out_node)
        {
            assert(m_curr_char == '"');

            bool retVal = false;

            char* str_start = m_curr_char_p;

            out_node.m_value_type = jsonland::value_type::string_t;
            while (next_char())  [[likely]]
            {
                if (m_curr_char == '"') {
                    break;
                }
                if (m_curr_char == '\\')
                {
                    next_char();
                    if (! is_there_more_data()) [[unlikely]] // the '\' was  the last char
                    {
                        throw parsing_exception(curr_offset(), {"escape char '\\' is the last char"sv});
                    }
                    else if (! is_escapable_char(m_curr_char))
                    {
                        throw parsing_exception(curr_offset(), {"char '"sv,
                                                                std::string_view(&m_curr_char, 1),
                                                                "' should not be escaped"sv});
                    }
                    else if ('u' == m_curr_char)  // unicode escape
                    {
                        if (4 > num_remaining_chars()) [[unlikely]]
                        {
                            throw parsing_exception(curr_offset(),
                                                    {"Unicode escape should be followed by 4 characters"sv});
                        }
                        for (int i = 0; i < 4; ++i)
                        {
                            next_char();
                            if (! is_hex_digit(m_curr_char)) [[unlikely]]
                            {
                                throw parsing_exception(curr_offset(),
                                                        {"char '"sv,
                                                        std::string_view(&m_curr_char, 1),
                                                        "' is no a hex digit"sv});
                            }
                        }
                    }
                }
                else if (is_a_char_that_must_be_escaped(m_curr_char))
                {
                    throw parsing_exception(curr_offset(),
                                            {"char '"sv,
                                            name_of_control_char(m_curr_char),
                                            "' should be escaped"sv});
                }
                else if (is_illegal_string_char(m_curr_char))
                {
                    throw parsing_exception(curr_offset(), {"illegal char in string"sv});
                }
            }

            if (m_curr_char == '"')  [[likely]]
            {
                size_t str_size = m_curr_char_p-str_start-1;
                out_node.parser_direct_set(std::string_view(str_start, str_size), jsonland::value_type::string_t);
                next_char();
                retVal = true;
            }
            else [[unlikely]]
            {
                throw parsing_exception(curr_offset(), {"string was not terminated with '\"'"});
            }

            return retVal;
        }

        bool parse_number(json_node& out_node)
        {
            char* str_start = m_curr_char_p-1;
            bool num_is_int{true};

            // this function calls next_char2() without checking return value
            // this works before if ens-of-data was reached, m_curr_char==end_marker
            // and end_marker is not a valid char for a number

            // json number should start with '-' or a digit
            if ('-' == m_curr_char && !next_char())
            {
                throw parsing_exception(curr_offset(), {"unexpected end of data during number parsing"sv});
            }

            if ('0' == m_curr_char)
            {
                // number started with zero
                // so only '.' or 'e' or 'E' are expected, not more digits
                if (!next_char()) // single token single digit "0"
                    goto scan_number_done;

                if ('.' == m_curr_char)
                    goto after_decimal_point;
                else if ('e' == m_curr_char || 'E' == m_curr_char)
                    goto after_exponent;
                else if (isdigit(m_curr_char))
                {
                    throw parsing_exception(curr_offset(),
                                           {"number starting with 0 cannot have more digits after the 0"sv});
                }
                else
                    goto scan_number_done;
            }
            else if (isdigit(m_curr_char)) { // number started with 1-9
                while (next_char() && isdigit(m_curr_char)){} // skip digits

                if ('.' == m_curr_char)
                    goto after_decimal_point;
                else if ('e' == m_curr_char || 'E' == m_curr_char)
                    goto after_exponent;
                else
                    goto scan_number_done;
            }
            else  [[unlikely]]
            {
                throw parsing_exception(curr_offset(),
                                        {"expected a digit or '-' to start a number got '"sv,
                                        std::string_view(&m_curr_char, 1),
                                        "' instead"sv});
            }

after_decimal_point:
            num_is_int = false;
            // after a '.' only a digit is expected
            next_char();
            if (!isdigit(m_curr_char))  [[unlikely]]
            {
                throw parsing_exception(curr_offset(),
                                        {"expected a digit after '.' got '"sv,
                                        std::string_view(&m_curr_char, 1),
                                        "' instead"sv});
            }
            while (next_char() && isdigit(m_curr_char)){} // skip digits

            if ('e' == m_curr_char || 'E' == m_curr_char)
                goto after_exponent;
            else
                goto scan_number_done;

after_exponent:
            num_is_int = false;
            // after 'e' or 'E'
            next_char();
            if ('-' == m_curr_char || '+' == m_curr_char)
                next_char();

            if (isdigit(m_curr_char))
            {
                while (next_char() && isdigit(m_curr_char)){} // skip digits
                goto scan_number_done;
            }
            else  [[unlikely]]
            {
                throw parsing_exception(curr_offset(),
                                        {"expected a digit after '"sv,
                                        std::string_view(m_curr_char_p-1, 1),
                                        "' got '"sv,
                                        std::string_view(&m_curr_char, 1),
                                        "' instead"sv});
            }

scan_number_done:
            std::string_view sov(str_start, m_curr_char_p-str_start-1);
#if JSONLAND_DEBUG==1
// in release build the number is translated from text only when to_double/to_int is called
            out_node.m_num = std::atof((const char*)sov.data());
#endif
            out_node.parser_direct_set(sov, jsonland::value_type::number_t);
            out_node.set_hint(json_node::_num_in_string);
            if (num_is_int)
            {
                out_node.set_hint(json_node::_num_is_int);
            }
            return true;
        }

        bool parse_constant(json_node& out_node)
        {
            assert('f' == m_curr_char || 't' == m_curr_char || 'n' == m_curr_char);

            bool retVal = false;

            switch (m_curr_char)
            {
                case 'f':
                    out_node.parser_direct_set(json_node::the_false_string_view, jsonland::bool_t);
                break;
                case 't':
                    out_node.parser_direct_set(json_node::the_true_string_view, jsonland::bool_t);
                break;
                case 'n':
                    out_node.parser_direct_set(json_node::the_null_string_view, jsonland::null_t);
                break;
                default: [[unlikely]]
                {
                    throw parsing_exception(curr_offset(),
                                            {"word starting with '"sv,
                                            std::string_view(&m_curr_char, 1),
                                            "' is not the expected 'false', 'true', 'null'"});
                }
                break;
            }
            std::string_view actual_token(m_curr_char_p-1,
                                          std::min(out_node.as_string_view().size(), num_remaining_chars()+1));

            if (actual_token == out_node.as_string_view())  [[likely]]
            {
                skip_chars(out_node.as_string_view().size());
                retVal = true;
            }
            else [[unlikely]]
            {
                throw parsing_exception(curr_offset(),
                                        {"word starting with '"sv,
                                        std::string_view(&m_curr_char, 1),
                                        "' is not the expected '",
                                        out_node.as_string_view(),
                                        "'"});
            }

            return retVal;
        }

        bool parse_control_char(json_node&)
        {
            next_char();
            return true;
        }

        bool skip_one_char(json_node&)
        {
            throw parsing_exception(curr_offset(), {"json syntax error: unexpected character"sv});
           // return false;
        }

        bool skip_new_line(json_node&)
        {
            // skip_new_line should be called only if m_curr_char is '\n'
            assert('\n' == m_curr_char);

            while (next_char())
            {
                ++m_current_line;
                m_offset_of_line = m_curr_char_p;
                if ('\n' != m_curr_char)
                {
                    break;
                }
            }

            assert(!is_there_more_data() || '\n' != m_curr_char);

            return false;
        }

        bool skip_whilespace(json_node&)
        {
            // skip_whilespace should be called only if m_curr_char is whitespace (but not newline)
            assert(is_white_space_not_new_line(m_curr_char));

            while (next_char())
            {
                if (!is_white_space_not_new_line(m_curr_char))
                {
                    break;
                }
            }

            // when skip_whilespace exits m_curr_char should be non-whilespace or
            // end of data has being reached
            assert(!is_there_more_data() || !is_white_space_not_new_line(m_curr_char));

            return false;
        }

        bool parse_array(json_node& out_node)
        {
            assert('[' == m_curr_char);

            if (++m_nesting_level > m_max_nesting_level) [[unlikely]]
            {
                throw parsing_exception_maximum_nesting_level(curr_offset(), m_max_nesting_level);
            }

            out_node.m_value_type = jsonland::value_type::array_t;
            size_t array_values_stack_starting_index = m_array_values_stack.size();

            parsing_value_type expecting = _value_or_array_close;

            if (!next_char()) {
                throw parsing_exception(curr_offset(), {"unexpected end of data during array parsing"sv});
            }

            json_node next_node;
            parsing_value_type new_value_type;
            while (get_next_node(next_node, new_value_type, expecting)) [[likely]]
            {

                if (new_value_type & parsing_value_type::_value) [[likely]]
                {
                    m_array_values_stack.push_back(std::move(next_node));
                    expecting = _comma_or_array_close;
                }
                else if (new_value_type & parsing_value_type::_comma)
                {
                    expecting = parsing_value_type::_value;
                }
                else if (new_value_type & parsing_value_type::_array_close) [[unlikely]]
                {
                    if (array_values_stack_starting_index != m_array_values_stack.size()) [[likely]] // is array is not empty
                    {
                        auto move_starts_from = std::next(m_array_values_stack.begin(), array_values_stack_starting_index);

                        out_node.m_values.insert(out_node.m_values.begin(),
                                            std::make_move_iterator(move_starts_from),
                                            std::make_move_iterator(m_array_values_stack.end()));
                        m_array_values_stack.erase(move_starts_from, m_array_values_stack.end());
                    }
                    --m_nesting_level;
                    return true;
                }
                else
                {
                    throw parsing_exception(curr_offset(), {"C++ parser bug: unexpected token during array creation"sv});
                }
            }

            if (0 == ((int)next_node.m_value_type & parsing_value_type::_array_close)) [[unlikely]]
            {
                throw parsing_exception(curr_offset(), {"unexpected end of tokens during array initializtion"sv});
            }

            return false;
        }

        bool parse_obj(json_node& out_node)
        {
            assert('{' == m_curr_char);

            if (++m_nesting_level > m_max_nesting_level) [[unlikely]]
            {
                throw parsing_exception_maximum_nesting_level(curr_offset(), m_max_nesting_level);
            }

            out_node.m_value_type = jsonland::value_type::object_t;
            size_t array_values_stack_starting_index = m_array_values_stack.size();
            size_t obj_keys_stack_starting_index = m_obj_keys_stack.size();

            bool expecting_key = true;

            if (!next_char()) {
                throw parsing_exception(curr_offset(), {"unexpected end of data during array parsing"sv});
            }

            jsonland::string_and_view key;
            json_node next_node;
            parsing_value_type new_value_type;
            parsing_value_type expecting = _str_or_obj_close;
            while (get_next_node(next_node, new_value_type, expecting)) [[likely]]
            {
                if ((new_value_type & parsing_value_type::_str) && expecting_key)
                {
                    auto& last_key = m_obj_keys_stack.emplace_back(next_node.m_value);
                    last_key.unescape_json_string_internal();
                    expecting_key = false;
                    expecting = parsing_value_type::_colon;
                }
                else if (new_value_type & parsing_value_type::_colon)
                {
                    expecting = parsing_value_type::_value;
                }
                else if (new_value_type & parsing_value_type::_value)
                {
                    next_node.m_key = m_obj_keys_stack.back();
                    m_array_values_stack.emplace_back(std::move(next_node));
                    expecting = parsing_value_type::_comma_or_obj_close;
                }
                else if (new_value_type & parsing_value_type::_comma)
                {
                    expecting = parsing_value_type::_str;
                    expecting_key = true;
                }
                else if (new_value_type & parsing_value_type::_obj_close)
                {
                    if (array_values_stack_starting_index != m_array_values_stack.size()) [[likely]]  // if array is not empty
                    {
                        auto move_starts_from = std::next(m_array_values_stack.begin(), array_values_stack_starting_index);

                        out_node.m_values.reserve(m_array_values_stack.size() - array_values_stack_starting_index);
                        out_node.m_values.insert(out_node.m_values.begin(),
                                                std::make_move_iterator(move_starts_from),
                                                std::make_move_iterator(m_array_values_stack.end()));
                        m_array_values_stack.erase(move_starts_from, m_array_values_stack.end());

                        out_node.m_obj_key_to_index.reserve(m_obj_keys_stack.size() -
                                                               obj_keys_stack_starting_index);
                        int index = 0;
                        auto obj_key_stack_start = std::next(m_obj_keys_stack.begin(),
                                                             obj_keys_stack_starting_index);
                        for (auto iterKey = obj_key_stack_start;
                             iterKey != m_obj_keys_stack.end(); ++iterKey)
                        {
                            out_node.m_obj_key_to_index[*iterKey] = index++;
                        }
                        m_obj_keys_stack.erase(obj_key_stack_start, m_obj_keys_stack.end());
                    }
                    --m_nesting_level;
                    return true;
                }
                else [[unlikely]]
                {
                    throw parsing_exception(curr_offset(),
                                            {"C++ parser bug: unexpected token during json-object creation"sv});
                }
            }

            if (0 == ((int)next_node.m_value_type & parsing_value_type::_obj_close)) [[unlikely]]
            {
                throw parsing_exception(curr_offset(),
                                        {"unexpected end of tokens during object initializtion"sv});
            }

            return false;
        }

        /// Advances to the next character in the input buffer.
        ///
        /// If the current character pointer `m_curr_char_p` is less than `m_end`,
        /// this function updates `m_curr_char` with the character at the current pointer,
        /// advances the pointer, and returns `true`.
        /// Otherwise, it sets `m_curr_char` to `end_mark` and returns `false`.
        ///
        /// @return `true` if a valid character was read; `false` if the end of the buffer was reached.
       inline bool next_char()
        {
            bool retVal = m_curr_char_p < m_end;
            m_curr_char = retVal ? *m_curr_char_p++ : end_mark;
            return retVal;
        }

public:
        int parse(const size_t in_max_nesting_level)
        {
            m_max_nesting_level = in_max_nesting_level;
            m_parse_error = 0;

            try
            {
                if (next_char()) [[likely]]
                {
                    // there should be one and only one top level json node
                    // get_next_node will return false if no valid json was found
                    parsing_value_type type_dummy = _uninitialized;
                    bool found_json = get_next_node(m_top_node, type_dummy, parsing_value_type::_value);
                    if (found_json && m_top_node.is_valid())
                    {
                        // check that remaing characters are only whitespace
                        while (is_whitespace(m_curr_char)) {
                            next_char();
                        }

                        if (m_curr_char != end_mark) // found non white space chars
                        {
                            throw parsing_exception(curr_offset(), {"Invalid characters after json"sv});
                        }
                    }
                    else [[unlikely]]{
                        throw parsing_exception(curr_offset(), {"Could not find valid json"sv});
                    }
                }
                else [[unlikely]]
                {
                    throw parsing_exception(curr_offset(), {"Could not find valid json - only whitespace"sv});
                }
            }
            catch (parsing_exception& p_ex) {
                m_top_node.m_parse_error = -1;
                m_top_node.m_parse_error_message = p_ex.what();
                m_top_node.m_value_type = uninitialized_t;
            }

            return m_top_node.m_parse_error;
        }
    };
}
}

void json_doc::set_max_nesting_level(const size_t in_max_nesting_level)
{
    m_max_nesting_level = in_max_nesting_level;
}

int json_doc::parse_insitu(std::string_view in_json_str)
{
    parser_impl::Parser pa(*this, const_cast<char*>(in_json_str.data()), const_cast<char*>(in_json_str.data()) + in_json_str.size());
    int retVal = pa.parse(m_max_nesting_level);
    return retVal;
}

int json_doc::parse(const std::string_view in_json_str)
{
    m_json_text = in_json_str;
    int retVal = parse_insitu(m_json_text);
    return retVal;
}

size_t json_doc::memory_consumption()
{
    size_t retVal = sizeof(*this);

    retVal += json_node::memory_consumption();

    return retVal;
}

void json_doc::clear()
{
    json_node::clear();
    m_json_text.clear();
    m_parse_error = 0;
    m_parse_error_message.clear();
}

bool json_node::operator==(const jsonland::json_node& other) const
{
    bool retVal = false;

    if (other.m_value_type == m_value_type)
    {
        switch(other.m_value_type)
        {
            case jsonland::value_type::object_t:
                retVal = other.m_values == m_values;
            break;
            case jsonland::value_type::array_t:
                retVal = other.m_values == m_values;
            break;
            case jsonland::value_type::number_t:
                // comparing two number in text representation creates a dilema:
                // what if the text representation is different but the actual number are the same?
                // e.g. "1.000000000000000011", "1.000000000000000012"
                // Answer: if both numbers are text - compare the text, otherwise compare m_num
                if (other.is_num_in_string() && is_num_in_string())
                    retVal = other.m_value == m_value;
                else
                {
                    double my_num = other.get_float<double>();
                    double other_num = get_float<double>();
                    retVal = my_num == other_num;
                }
            break;
            case jsonland::value_type::string_t:
            case jsonland::value_type::bool_t:
                retVal = other.m_value == m_value;
            break;
            case jsonland::value_type::null_t:
            default:
                retVal = true;
            break;
        }
    }

    return retVal;
}

bool json_node::operator!=(const jsonland::json_node& other) const
{
    return !(other == *this);
}
