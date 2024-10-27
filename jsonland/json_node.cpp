#include "jsonland/json_node.h"

#include <array>
#include <iterator>
#include <cstdio>
#include <charconv>

using namespace jsonland;

#if JSONLAND_DEBUG==1
size_t string_or_view::num_allocations{0};
#endif

inline bool is_white_space_not_new_line(const char in_c)
{
    return ' ' == in_c  || '\r' == in_c || '\t' == in_c;
}

inline bool is_whitespace(const char in_c)
{
    return is_white_space_not_new_line(in_c) || '\n' == in_c;
}

inline bool is_digit(const char in_c)
{
    const bool retVal = ('0' <= in_c && '9' >= in_c);
    return retVal;
}

inline bool is_hex_digit(const char in_c)
{
    const bool retVal = is_digit(in_c) || ('a' <= in_c && 'f' >= in_c) || ('A' <= in_c && 'F' >= in_c);
    return retVal;
}

inline bool is_num_char(const char in_c)
{
    const bool retVal = ('0' <= in_c && '9' >= in_c) || '.' == in_c || '-' == in_c || '+' == in_c || 'e' == in_c || 'E' == in_c;
    return retVal;
}

inline bool is_illegal_string_char(const char in_c)
{
    const bool retVal = '\0' == in_c;
    return retVal;
}

inline bool is_escapable_char(const char in_c)
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

inline bool is_a_char_that_must_be_escaped(const char in_c)
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

inline const char* escapable_to_escaped(const char* in_c, size_t& out_chars_to_copy)
{
    const char* retVal = in_c;
    out_chars_to_copy = 2;

    switch(*in_c)
    {
        case '"':  retVal = R"(\\")"; break;
        case '\\':  retVal = R"(\\\\)"; break;
        case '\n': retVal = "\\n"; break;
        case '\r': retVal = "\\r"; break;
        case '\t': retVal = "\\t"; break;
        case '\b': retVal = "\\b"; break;
        case '\f': retVal = "\\f"; break;
        default:
            out_chars_to_copy = 1;
            break;
    }

    return retVal;
}

static const char* name_of_control_char(const char in_c)
{
    const char* retVal = "unknown";
    switch(in_c)
    {
        case '\b': retVal = "backspace"; break;
        case '\f': retVal = "formfeed"; break;
        case '\n': retVal = "linefeed"; break;
        case '\r': retVal = "carriage return"; break;
        case '\t': retVal = "horizontal tab"; break;
    }
    return retVal;
}

//json_node::json_node(const json_node& in_node) noexcept
//: m_value_type(in_node.m_value_type)
//, m_value(in_node.m_value)
//, m_num(in_node.m_num)
//, m_values(in_node.m_values)
//, m_obj_key_to_index(in_node.m_obj_key_to_index)
//, m_key(in_node.m_key)
//{
//}
//
//json_node::json_node(json_node&& in_node) noexcept
//: m_value_type(in_node.m_value_type)
//, m_value(std::move(in_node.m_value))
//, m_num(in_node.m_num)
//, m_values(std::move(in_node.m_values))
//, m_obj_key_to_index(std::move(in_node.m_obj_key_to_index))
//, m_key(std::move(in_node.m_key))
//{
//}

// dummy uninitialized json_node to be returned from operator[] const; where the
// object/array doe snot contain the key/index
const json_node json_node::const_uninitialized_json_node(jsonland::uninitialized_t);


json_node& json_node::operator=(const json_node& in_node) noexcept
{
    m_value_type = in_node.m_value_type;

    m_value = in_node.m_value;
    m_num = in_node.m_num;
    m_values = in_node.m_values;
    m_obj_key_to_index = in_node.m_obj_key_to_index;
    m_hints = in_node.m_hints;
    //m_key = in_node.m_key; - do not copy m_key, parent node (if obj) should take care of that

    return *this;
}

json_node& json_node::operator=(json_node&& in_node) noexcept
{
    m_value_type = in_node.m_value_type;

    m_value = std::move(in_node.m_value);
    m_num = in_node.m_num;
    m_values = std::move(in_node.m_values);
    m_obj_key_to_index = std::move(in_node.m_obj_key_to_index);
    m_hints = in_node.m_hints;
    //m_key = in_node.m_key; - do not copy m_key, parent node (if obj) should take care of that

    return *this;
}

void string_or_view::store_value_deal_with_escapes(std::string_view in_str) noexcept
{
    m_num_escapes = 0;
    std::string temp_str;
    temp_str.reserve(static_cast<size_t>(static_cast<double>(in_str.size())*1.1));

    for (char curr : in_str)
    {
        size_t num_chars_to_copy = 0;
        const char* p = escapable_to_escaped(&curr, num_chars_to_copy);
        temp_str.append(p, num_chars_to_copy);
        if ('\\' == *p) // \\ will be counted as two escapes, but count need not be accurate
            ++m_num_escapes;
    }
    m_value = std::move(temp_str);
#if JSONLAND_DEBUG==1
    ++num_allocations;
#endif
}

std::ostream& json_node::dump(std::ostream& os) const noexcept
{
    std::string str;
    dump(str);
    os.write(str.c_str(), str.size());
    return os;
}

template<typename TToPrintf>
static int __printf(char* in_buff, const size_t in_buff_size, const TToPrintf in_to_print, const char* printf_format=nullptr) noexcept
{
    bool remove_zeros{false};
    if (nullptr == printf_format)
    {
        if constexpr (std::is_floating_point_v<TToPrintf>) {
            printf_format = "%llf";
            remove_zeros = true;
        }
        else if constexpr (std::is_integral_v<TToPrintf> && std::is_signed_v<TToPrintf>) {
            printf_format = "%lli";
        }
        else if constexpr (std::is_integral_v<TToPrintf> && std::is_unsigned_v<TToPrintf>) {
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

    if (remove_zeros && num_chars > 1)// when ouput_size==1 there are no extra '0's
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

void json_node::dump(std::string& out_str) const noexcept
{
    if (is_object())
    {
        out_str += '{';
        if (!m_values.empty())
        {
            auto io = m_values.begin();
            io->m_key.dump_with_quotes(out_str);
            out_str += ':';
            io->dump(out_str);

            for (++io; io != m_values.end(); ++io)
            {
                out_str += ',';
                io->m_key.dump_with_quotes(out_str);
                out_str += ':';
                io->dump(out_str);
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
            ai->dump(out_str);

            for (++ai; ai != m_values.end(); ++ai) {
                out_str += ',';
                ai->dump(out_str);
            }
        }
        out_str += ']';
    }
    else if (is_number())
    {  // where art thou Grisu2 ?
        if (is_num_in_string())
        {
            m_value.dump_no_quotes(out_str);
        }
        else
        {
            char buff[128]{'\0'};
            if (m_hints & _num_is_int) {
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
        m_value.dump_with_quotes(out_str);
    }
    else
    {
        m_value.dump_no_quotes(out_str);
    }
}

std::string json_node::dump() const noexcept
{
    std::string retVal;
    dump(retVal);
    return retVal;
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
            retVal = (m_value.empty() || m_value.is_value_stored()) && (m_key.empty() || m_key.is_value_stored());
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
    m_key.convert_referenced_value_to_stored();
    m_value.convert_referenced_value_to_stored();
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
            for (auto& val : m_values)
            {
                val.take_ownership();
            }
        break;
    }
}

namespace jsonland
{

const char* value_type_name(jsonland::value_type in_type)
{
    const char* retVal = "Unknown";
    switch (in_type)
    {
        case jsonland::value_type::uninitialized_t: retVal = "uninitialized_t"; break;
        case jsonland::value_type::null_t: retVal = "null_t"; break;
        case jsonland::value_type::bool_t: retVal = "bool_t"; break;
        case jsonland::value_type::number_t: retVal = "number_t"; break;
        case jsonland::value_type::string_t: retVal = "string_t"; break;
        case jsonland::value_type::array_t: retVal = "array_t"; break;
        case jsonland::value_type::object_t: retVal = "object_t"; break;
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
        while (JSONLAND_LIKELY(chars_left > 0))
        {
            if (JSONLAND_UNLIKELY(*str_leftp != *str_rightp))
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
        while (JSONLAND_LIKELY(chars_left > 0))
        {
            if (JSONLAND_UNLIKELY(std::tolower(*str_leftp) != *str_rightp))
                break;
            ++str_leftp;
            ++str_rightp;
            --chars_left;
        }

        bool retVal = 0 == chars_left;
        return retVal;
    }

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
        };

        class parsing_exception : public std::exception
        {
        public:
            parsing_exception(const char* in_message, const location_in_string& in_location)
            {
                std::array<char, 64> temp_str;
                m_message += in_message;
                m_message += "' line ";
                std::snprintf(&temp_str[0], 64, "%zu", in_location.m_line);
                m_message += &temp_str[0];

                m_message += " at offset ";
                std::snprintf(&temp_str[0], 64, "%zu", in_location.m_offset);
                m_message += &temp_str[0];

                m_message += ";";
            }

            const char* what() const noexcept override
            {
                return m_message.c_str();
            }

            std::string m_message;
        };

        Parser(json_doc& in_top_node, char* in_start, char* in_end)
        : m_top_node(in_top_node)
        , m_str_size(in_end-in_start)
        , m_start(in_start)
        , m_end(in_end)
        , m_curr_char(in_start)
        , m_offset_of_line(in_start)
        {
            prepare_char_to_token_table();
            m_array_values_stack.reserve(1024);
            m_obj_keys_stack.reserve(1024);
        }

    private:
        json_doc& m_top_node;

        [[maybe_unused]] const size_t m_str_size;
        [[maybe_unused]] const char* m_start = nullptr;
        const char* m_end = nullptr;

        char* m_curr_char = nullptr;
        size_t m_max_nesting_level = 64;
        size_t m_nesting_level = 0;

        std::string m_json_text;

        const char* m_offset_of_line = nullptr;
        size_t m_current_line = 1;
        int m_parse_error = 0;
        std::string m_parse_error_message;

        json_node::ArrayVec m_array_values_stack;
        std::vector<jsonland::string_or_view> m_obj_keys_stack;

        inline char next_char()
        {
            return *++m_curr_char;
        }

        inline char next_chars(const size_t in_num_chars_to_skip)
        {
            m_curr_char += in_num_chars_to_skip;
            return *m_curr_char;
        }

        inline location_in_string curr_offset()
        {
            return location_in_string{m_current_line, (size_t)(m_curr_char - m_offset_of_line)};
        }

        inline bool is_there_more_data()
        {
            return m_curr_char < m_end;
        }

        inline size_t num_remaining_chars()
        {
           return m_end - m_curr_char;
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

        inline bool get_next_node(json_node& out_node, parsing_value_type& out_type)
        {
            bool retVal = false;
            while (JSONLAND_LIKELY(!retVal && is_there_more_data()))
            {
                char curr_char = *m_curr_char;
                if (256 > (unsigned int)curr_char)
                {
                    func_type_pair& call_pair = m_char_to_token_table[(int)curr_char];
                    out_type = call_pair.m_type;
                    retVal = (this->*call_pair.m_func)(out_node);
                }
                else
                {
                    func_type_pair& call_pair = m_char_to_token_table[0];
                    out_type = call_pair.m_type;
                    retVal = (this->*call_pair.m_func)(out_node);
                }
            }
            return retVal;
        }

        bool parse_string(json_node& out_node)
        {
            // if parse_string was called then *curr == '"'
            bool retVal = false;

            char curr_char = next_char();
            char* str_start = m_curr_char;

            out_node.m_value.m_num_escapes = 0;
            out_node.m_value_type = jsonland::value_type::string_t;
            while (JSONLAND_LIKELY(is_there_more_data()))
            {
                if (curr_char == '"') {
                    break;
                }
                if (curr_char == '\\')
                {
                    ++out_node.m_value.m_num_escapes;
                    curr_char = next_char();
                    if (JSONLAND_UNLIKELY(! is_there_more_data())) // the '\' was  the last char
                    {
                        throw parsing_exception("escape char '\\' is the last char", curr_offset());
                    }
                    else if (! is_escapable_char(curr_char))
                    {
                        std::string message = "char '";
                        message += curr_char;
                        message += "' should not be escaped";
                        throw parsing_exception(message.c_str(), curr_offset());
                    }
                    else if ('u' == curr_char)  // unicode escape
                    {
                        if (4 > num_remaining_chars())
                        {
                            throw parsing_exception("Unicode escape should be followed by 4 characters", curr_offset());
                        }
                        for (int i = 0; i < 4; ++i)
                        {
                            if (! is_hex_digit(next_char()))
                            {
                                std::string message = "char '";
                                message += curr_char;
                                message += "' is no a hex digit";
                                throw parsing_exception(message.c_str(), curr_offset());
                            }
                        }
                    }
                }
                else if (is_a_char_that_must_be_escaped(curr_char))
                {
                    std::string message = "char '";
                    message +=  name_of_control_char(curr_char);
                    message += "' should be escaped";
                    throw parsing_exception(message.c_str(), curr_offset());
                }
                else if (is_illegal_string_char(curr_char))
                {
                    throw parsing_exception("illegal char in string", curr_offset());
                }

                curr_char = next_char();
            }

            if (JSONLAND_LIKELY(curr_char == '"'))
            {
                //*const_cast<char*>(m_curr_char) = '\0';

                size_t str_size = m_curr_char-str_start;
                out_node.parser_direct_set(std::string_view(str_start, str_size), jsonland::value_type::string_t);
                next_char();
                retVal = true;
            }
            else
            {
                throw parsing_exception("string was not terminated with '\"'", curr_offset());
            }

            return retVal;
        }

        bool parse_number(json_node& out_node)
        {
            char* str_start = m_curr_char;
            char curr_char = *m_curr_char;
            bool num_is_int{true};

            // json number should start with '-' or a digit
            if ('-' == curr_char)
                curr_char = next_char();

            if ('0' == curr_char) {
                // number started with zero
                // so only '.' or 'e' or 'E' are expected, not more digits
                curr_char = next_char();
                if ('.' == curr_char)
                    goto after_decimal_point;
                else if ('e' == curr_char || 'E' == curr_char)
                    goto after_exponent;
                else if (isdigit(curr_char))
                {
                    std::string message = "number starting with 0 cannot have more digits after the 0";
                    throw parsing_exception(message.c_str(), curr_offset());
                }
                else
                    goto scan_number_done;
            }
            else if (isdigit(curr_char)) { // number started with 1-9
                while (isdigit(curr_char = next_char())) ;

                if ('.' == curr_char)
                    goto after_decimal_point;
                else if ('e' == curr_char || 'E' == curr_char)
                    goto after_exponent;
                else
                    goto scan_number_done;
            }
            else {
                std::string message = "expected a digit or '-' to start a number go '";
                message += curr_char;
                message += "' instead";
                throw parsing_exception(message.c_str(), curr_offset());
            }

after_decimal_point:
            num_is_int = false;
            // after a '.' only a digit is expected
            curr_char = next_char();
            if (!isdigit(curr_char)) {
                std::string message = "expected a digit after '.' got '";
                message += curr_char;
                message += "' instead";
                throw parsing_exception(message.c_str(), curr_offset());
            }
            while (isdigit(curr_char = next_char())) ;
            if ('e' == curr_char || 'E' == curr_char)
                goto after_exponent;
            else
                goto scan_number_done;

after_exponent:
            num_is_int = false;
            // after 'e' or 'E'
            curr_char = next_char();
            if ('-' == curr_char || '+' == curr_char)
                curr_char = next_char();

            if (isdigit(curr_char)) {
                while (isdigit(next_char())) ;
                goto scan_number_done;
            }
            else {
                std::string message = "expected a digit after '";
                message += *(m_curr_char-1);
                message += "' got '";
                message += curr_char;
                message += "' instead";
                throw parsing_exception(message.c_str(), curr_offset());
            }

scan_number_done:
            std::string_view sov(str_start, m_curr_char-str_start);
#if JSONLAND_DEBUG==1
// in release build the number is translated from text only when to_double/to_int is called
            out_node.m_num = std::atof((const char*)sov.data());
#endif
            out_node.parser_direct_set(sov, jsonland::value_type::number_t);
            out_node.m_hints = static_cast<json_node::hints>(json_node::_num_in_string);
            if (num_is_int) {
                out_node.m_hints = static_cast<json_node::hints>(out_node.m_hints | json_node::_num_is_int);
            }
            return true;
        }

        bool parse_constant(json_node& out_node)
        {
            bool retVal = false;

            char first_char = *m_curr_char;
            switch (first_char) {
                case 'f':
                    out_node.parser_direct_set(json_node::the_false_string_view, jsonland::bool_t);
                break;
                case 't':
                    out_node.parser_direct_set(json_node::the_true_string_view, jsonland::bool_t);
                break;
                case 'n':
                    out_node.parser_direct_set(json_node::the_null_string_view, jsonland::null_t);
                break;
                default:
                {
                    std::string message = "word starting with '";
                    message += first_char;
                    message += "' is not the expected 'false', 'true', 'null'";
                    throw parsing_exception(message.c_str() ,curr_offset());
                }
                break;
            }

            if (JSONLAND_UNLIKELY(num_remaining_chars() < static_cast<size_t>(out_node.as_string_view().size())))
            {
                std::string message = "not enough characters to form '";
                message += out_node.as_string_view();
                message += "'";
                throw parsing_exception(message.c_str() ,curr_offset());
            }

            else if (JSONLAND_LIKELY(str_compare_case_sensitive_n(m_curr_char, out_node.as_string_view())))
            {
                next_chars(out_node.as_string_view().size());
                retVal = true;
            }
            else
            {
                std::string message = "word starting with '";
                message += first_char;
                message += "' is not the expected '";
                message += out_node.as_string_view().data();
                message += "'";
                throw parsing_exception(message.c_str() ,curr_offset());
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
            throw parsing_exception("json syntax error: unexpected character", curr_offset());
           // return false;
        }

        bool skip_new_line(json_node&)
        {
            ++m_current_line;
            next_char();
            m_offset_of_line = m_curr_char;
            return false;
        }

        bool skip_whilespace(json_node&)
        {
            while(is_white_space_not_new_line(next_char())) ;
            return false;
        }

        bool parse_array(json_node& out_node)
        {
            if (JSONLAND_UNLIKELY(++m_nesting_level > m_max_nesting_level))
            {
                std::array<char, 64> temp_str;
                std::string message = "Reached maximum nesting level of ";
                std::snprintf(&temp_str[0], 64, "%zu", m_max_nesting_level);
                message += &temp_str[0];
                throw parsing_exception(message.c_str(), curr_offset());
            }

            out_node.m_value_type = jsonland::value_type::array_t;
            size_t array_values_stack_starting_index = m_array_values_stack.size();

            uint32_t expecting = parsing_value_type::_value | parsing_value_type::_array_close;
            next_char();
            json_node next_node;
            parsing_value_type new_value_type;
            while (JSONLAND_LIKELY(get_next_node(next_node, new_value_type)))
            {
                if (JSONLAND_UNLIKELY(!(new_value_type & expecting)))
                {
                    throw parsing_exception("json syntax error: unexpected token during array creation", curr_offset());
                }

                if (JSONLAND_LIKELY(new_value_type & parsing_value_type::_value))
                {
                    m_array_values_stack.push_back(std::move(next_node));
                    expecting = parsing_value_type::_comma | parsing_value_type::_array_close;
                }
                else if (new_value_type & parsing_value_type::_comma)
                {
                    expecting = parsing_value_type::_value;
                }
                else if (JSONLAND_UNLIKELY(new_value_type & parsing_value_type::_array_close))
                {
                    if (JSONLAND_LIKELY(array_values_stack_starting_index != m_array_values_stack.size()))  // is array is not empty
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
                    throw parsing_exception("C++ parser bug: unexpected token during array creation", curr_offset());
                }
            }

            if (JSONLAND_UNLIKELY(0 == (next_node.m_value_type & parsing_value_type::_array_close)))
            {
                throw parsing_exception("unexpected end of tokens during array initializtion", curr_offset());
            }

            return false;
        }

        bool parse_obj(json_node& out_node)
        {
            if (JSONLAND_UNLIKELY(++m_nesting_level > m_max_nesting_level))
            {
                std::array<char, 64> temp_str;
                std::string message = "Reached maximum nesting level of ";
                std::snprintf(&temp_str[0], 64, "%zu", m_max_nesting_level);
                throw parsing_exception(message.c_str(), curr_offset());
            }

            out_node.m_value_type = jsonland::value_type::object_t;
            size_t array_values_stack_starting_index = m_array_values_stack.size();
            size_t obj_keys_stack_starting_index = m_obj_keys_stack.size();

            bool expecting_key = true;
            jsonland::value_type expecting = static_cast<jsonland::value_type>(parsing_value_type::_str | parsing_value_type::_obj_close);
            next_char();
            jsonland::string_or_view key;
            json_node next_node;
            parsing_value_type new_value_type;
            while (JSONLAND_LIKELY(get_next_node(next_node, new_value_type)))
            {
                if (JSONLAND_UNLIKELY(!(new_value_type & expecting)))
                {
                    throw parsing_exception("json syntax error: unexpected token during object creation", curr_offset());
                }
                if ((new_value_type & parsing_value_type::_str) && expecting_key)
                {
                    m_obj_keys_stack.emplace_back(next_node.m_value);
                    expecting_key = false;
                    expecting = static_cast<jsonland::value_type>(parsing_value_type::_colon);
                }
                else if (new_value_type & parsing_value_type::_colon)
                {
                    expecting = static_cast<jsonland::value_type>(parsing_value_type::_value);
                }
                else if (new_value_type & parsing_value_type::_value)
                {
                    next_node.m_key = m_obj_keys_stack.back();
                    m_array_values_stack.emplace_back(std::move(next_node));
                    expecting = static_cast<jsonland::value_type>(parsing_value_type::_comma | parsing_value_type::_obj_close);
                }
                else if (new_value_type & parsing_value_type::_comma)
                {
                    expecting = static_cast<jsonland::value_type>(parsing_value_type::_str);
                    expecting_key = true;
                }
                else if (JSONLAND_UNLIKELY(new_value_type & parsing_value_type::_obj_close))
                {
                    if (JSONLAND_LIKELY(array_values_stack_starting_index != m_array_values_stack.size()))  // if array is not empty
                    {
                        auto move_starts_from = std::next(m_array_values_stack.begin(), array_values_stack_starting_index);

                        out_node.m_values.reserve(m_array_values_stack.size() - array_values_stack_starting_index);
                        out_node.m_values.insert(out_node.m_values.begin(),
                                                std::make_move_iterator(move_starts_from),
                                                std::make_move_iterator(m_array_values_stack.end()));
                        m_array_values_stack.erase(move_starts_from, m_array_values_stack.end());

                        out_node.m_obj_key_to_index.reserve(m_obj_keys_stack.size() - obj_keys_stack_starting_index);
                        int index = 0;
                        auto obj_key_stack_start = std::next(m_obj_keys_stack.begin(), obj_keys_stack_starting_index);
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
                else
                {
                    throw parsing_exception("C++ parser bug: unexpected token during json-object creation", curr_offset());
                }
            }

            if (JSONLAND_UNLIKELY(0 == (next_node.m_value_type & parsing_value_type::_obj_close)))
            {
                throw parsing_exception("unexpected end of tokens during object initializtion", curr_offset());
            }

            return false;
        }

        // skip possibly whitespace at the beginning or end, while still keeping count of the number of new lines
        int skip_whitespace()
        {
            json_node node_dummy;

            while (is_there_more_data())
            {
                if ('\n' == *m_curr_char) {
                    skip_new_line(node_dummy);
                }
                else if (is_white_space_not_new_line(*m_curr_char)) {
                    next_char();
                }
                else
                    break;
            }
            return static_cast<int>(num_remaining_chars());
        }

public:
        int parse(const size_t in_max_nesting_level)
        {
            m_max_nesting_level = in_max_nesting_level;
            m_parse_error = 0;

            try
            {
                skip_whitespace();

                if (is_there_more_data())
                {
                    // there should be one and only one top level json node
                    // get_next_node will return false if no valid json was found
                    parsing_value_type type_dummy = _uninitialized;
                    bool found_json = get_next_node(m_top_node, type_dummy);
                    if (found_json && m_top_node.is_valid())
                    {
                        // check that remaing characters are only whitespace
                        int num_remaining_non_whitespace_chars = skip_whitespace();
                        if (0 < num_remaining_non_whitespace_chars)
                        {
                            throw parsing_exception("Invalid characters after json", curr_offset());
                        }
                    }
                    else {
                        throw parsing_exception("Could not find valid json", curr_offset());
                    }
                }
                else
                {
                    throw parsing_exception("Could not find valid json - only whitespace", curr_offset());
                }
            }
            catch (parsing_exception& p_ex) {
                m_top_node.m_parse_error = -1;
                m_top_node.m_parse_error_message = p_ex.what();
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

int json_doc::parse_insitu(char* in_json_str, char* in_json_str_end)
{
    parser_impl::Parser pa(*this, in_json_str, in_json_str_end);
    int retVal = pa.parse(m_max_nesting_level);
    return retVal;
}

int json_doc::parse_insitu(char* in_json_str, const size_t in_json_str_size)
{
    int retVal = parse_insitu(in_json_str, in_json_str+in_json_str_size);
    return retVal;
}

int json_doc::parse_insitu(std::string& in_json_str)
{
    int retVal = parse_insitu(in_json_str.data(), in_json_str.data() + in_json_str.size());
    return retVal;
}

int json_doc::parse_insitu(std::string_view in_json_str)
{
    int retVal = parse_insitu(const_cast<char*>(in_json_str.data()), const_cast<char*>(in_json_str.data()) + in_json_str.size());
    return retVal;
}

int json_doc::parse(const char* in_json_str, const char* in_json_str_end)
{
    if (nullptr == in_json_str_end)
        m_json_text = std::string(in_json_str);
    else
        m_json_text = std::string(in_json_str, in_json_str_end);

    int retVal = parse_insitu(m_json_text);

    return retVal;
}

int json_doc::parse(const char* in_json_str, const size_t in_json_str_size)
{
    int retVal = parse(in_json_str, in_json_str+in_json_str_size);
    return retVal;
}

int json_doc::parse(const std::string& in_json_str)
{
    m_json_text = in_json_str;
    int retVal = parse_insitu(m_json_text);
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
