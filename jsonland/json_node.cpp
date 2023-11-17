#include "jsonland/json_node.h"

#include <array>
#include <iterator>
#include <cstdio>

using namespace jsonland;

inline bool is_white_space_not_new_line(const char in_c)
{
    return ' ' == in_c  or '\r' == in_c or '\t' == in_c;
}

inline bool is_whitespace(const char in_c)
{
    return is_white_space_not_new_line(in_c) or '\n' == in_c;
}

inline bool is_digit(const char in_c)
{
    const bool retVal = ('0' <= in_c and '9' >= in_c);
    return retVal;
}

inline bool is_hex_digit(const char in_c)
{
    const bool retVal = is_digit(in_c) or ('a' <= in_c and 'f' >= in_c) or ('A' <= in_c and 'F' >= in_c);
    return retVal;
}

inline bool is_num_char(const char in_c)
{
    const bool retVal = ('0' <= in_c and '9' >= in_c) or '.' == in_c or '-' == in_c or '+' == in_c or 'e' == in_c or 'E' == in_c;
    return retVal;
}

inline bool is_illegal_string_char(const char in_c)
{
    const bool retVal = '\0' == in_c;
    return retVal;
}

inline bool is_escapable_char(const char in_c)
{
    const bool retVal = '\\' == in_c or
                        '/' == in_c or
                        '"' == in_c or
                        'b' == in_c or
                        'f' == in_c or
                        'n' == in_c or
                        'r' == in_c or
                        't' == in_c or
                        'u' == in_c;
    return retVal;
}

inline bool is_must_be_escaped_char(const char in_c)
{
    const bool retVal = '\b' == in_c or
                        '\f' == in_c or
                        '\n' == in_c or
                        '\r' == in_c or
                        '\t' == in_c;
    return retVal;
}

inline const char* escapable_to_escaped(const char* in_c, size_t& out_chars_to_copy)
{
    const char* retVal = in_c;
    out_chars_to_copy = 2;
    
    switch(*in_c)
    {
        case '\b': retVal = "\\b"; break;
        case '\f': retVal = "\\f"; break;
        case '\n': retVal = "\\n"; break;
        case '\r': retVal = "\\r"; break;
        case '\t': retVal = "\\t"; break;
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

json_node::json_node(const json_node& in_node)
: m_node_type(in_node.m_node_type)
, m_value(in_node.m_value)
, m_num(in_node.m_num)
, m_values(in_node.m_values)
, m_obj_key_to_index(in_node.m_obj_key_to_index)
, m_key(in_node.m_key)
{
}

json_node::json_node(json_node&& in_node)
: m_node_type(in_node.m_node_type)
, m_value(std::move(in_node.m_value))
, m_num(in_node.m_num)
, m_values(std::move(in_node.m_values))
, m_obj_key_to_index(std::move(in_node.m_obj_key_to_index))
, m_key(std::move(in_node.m_key))
{
}

json_node& json_node::operator=(const json_node& in_node)
{
    m_node_type = in_node.m_node_type;

    m_value = in_node.m_value;
    m_num = in_node.m_num;
    m_values = in_node.m_values;
    m_obj_key_to_index = in_node.m_obj_key_to_index;
    //m_key = in_node.m_key; - do not copy m_key, parent node (if obj) should take care of that

    return *this;
}

json_node& json_node::operator=(json_node&& in_node)
{
    m_node_type = in_node.m_node_type;

    m_value = in_node.m_value;
    m_num = in_node.m_num;
    m_values = std::move(in_node.m_values);
    m_obj_key_to_index = std::move(in_node.m_obj_key_to_index);
    //m_key = in_node.m_key; - do not copy m_key, parent node (if obj) should take care of that

    return *this;
}

void jsonland::string_or_view::store_value_deal_with_escapes(std::string_view in_str)
{
    m_num_escapes = 0;
    m_internal_store.reserve(m_value.size()*1.1);
    
    for (char curr : in_str)
    {
        size_t num_chars_to_copy = 0;
        const char* p = escapable_to_escaped(&curr, num_chars_to_copy);
        m_internal_store.append(p, num_chars_to_copy);
        if ('\\' == *p) // \\ will be counted as two escapes, but count need not be accurate
            ++m_num_escapes;
    }
    m_value = m_internal_store;
}

class solve_escapes_iter
{
private:
    const std::string_view m_str;
    int m_str_i = -1;
    char m_curr_char = '\0';
    char m_waiting_char = '\0';

    size_t m_num_escapes_found = 0;
    
public:
    solve_escapes_iter(std::string_view in_str)
    : m_str(in_str)
    {
        if (!m_str.empty())
            operator++();
    }
    
    operator bool()
    {
        return m_str_i < static_cast<int>(m_str.size()) && '\0' != m_curr_char;
    }
    
    size_t num_escapes_found()
    {
        return m_num_escapes_found;
    }

    char hex2char(const char hi, const char lo)
    {
        uint8_t hi_t = (hi <= 57)? hi - 48 : (hi <= 70)? (hi - 65) + 0x0a : (hi - 97) + 0x0a;
        uint8_t lo_t = (lo <= 57)? lo - 48 : (lo <= 70)? (lo - 65) + 0x0a : (lo - 97) + 0x0a;
        return hi_t*16+lo_t;
    }

    
    solve_escapes_iter& operator++()
    {
        if (m_str_i >= static_cast<int>(m_str.size()) && m_str_i != -1)
            return *this;

        if ('\0' != m_waiting_char)
        {
            m_curr_char = m_waiting_char;
            m_waiting_char = '\0';
        }
        else
        {
            ++m_str_i;
            m_curr_char = m_str[m_str_i];
        
            if ('\\' == m_curr_char)
            {
                ++m_num_escapes_found;
                m_curr_char = m_str[++m_str_i];
                switch (m_curr_char)
                {
                    case '\\':
                    case '/':
                    case '"':
                        break;
                    case 'n': m_curr_char = '\n'; break;
                    case 'r': m_curr_char = '\r'; break;
                    case 't': m_curr_char = '\t'; break;
                    case 'b': m_curr_char = '\b'; break;
                    case 'f': m_curr_char = '\f'; break;
                    case 'u':
                    {
                        if (m_str_i+4 < static_cast<int>(m_str.size()))
                        {
                            char c1st = m_curr_char;
                            char c2nd = m_str[++m_str_i];
                            m_curr_char = hex2char(c1st, c2nd);
                            c1st = m_str[++m_str_i];
                            c2nd = m_str[++m_str_i];
                            m_waiting_char = hex2char(c1st, c2nd);
                        }
                        else
                        {
                            m_curr_char = '\0';
                            m_str_i = (int)m_str.size();
                        }
                    }
                }
            }
        }
        
        return *this;
    }
    
    char operator*()
    {
        return m_curr_char;
    }
    
};

void jsonland::string_or_view::unescape(std::string& out_unescaped) const
{
    out_unescaped.reserve(m_value.size());

    solve_escapes_iter iter(m_value);
    
    while (iter)
    {
        out_unescaped += *iter;
        ++iter;
    }
}

void jsonland::string_or_view::unescape_internal()
{
    unescape(m_internal_store);
    m_num_escapes = 0;
    m_value = m_internal_store;
}

const std::string_view json_node::as_resolved_string_view() const
{
    return as_string_view();
}

std::string json_node::dump() const
{
    std::ostringstream oss;
    dump(oss);
    return oss.str();
}

std::ostream& json_node::dump(std::ostream& os) const
{
    if (is_object())
    {
        os << '{';
        if (!m_values.empty())
        {
            auto io = m_values.begin();
            os << '"' << io->m_key.as_string_view() <<  R"(":)";
            io->dump(os);
            
            for (++io; io != m_values.end(); ++io)
            {
                os << R"(,")" << io->m_key.as_string_view() <<  R"(":)";
                io->dump(os);
            }
        }
        os << '}';
    }
    else if (is_array())
    {
        os << '[';
        if (!m_values.empty())
        {
            auto ai = m_values.begin();
            ai->dump(os);

            for (++ai; ai != m_values.end(); ++ai) {
                os << ",";
                ai->dump(os);
            }
        }
        os << ']';
    }
    else if (is_num() && is_number_assigned())
    {
        if (m_hints & _num_is_int)
            os << as_int<int64_t>();
        else
            os << as_double();
    }
    else if (is_string())
    {
        os << '"';
        os << m_value.as_string_view();
        os << '"';
    }
    else
    {
        os << m_value.as_string_view();
    }
    return os;
}

namespace jsonland
{

size_t json_node::memory_consumption() const
{
    size_t retVal{0};
    
    retVal += m_value.allocation_size();
    retVal += m_key.allocation_size();
    for (const json_node& val : m_values)
    {
        retVal += sizeof(ArrayVec::value_type);
        retVal += val.memory_consumption();
    }
    // if m_values over allocated
    retVal += (m_values.capacity() - m_values.size()) * sizeof(ArrayVec::value_type);
    
    for (const KeyToIndex::value_type& val : m_obj_key_to_index)
    {
        retVal += sizeof(KeyToIndex::value_type);
        retVal += val.first.allocation_size();
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
            location_in_string() = default;
            size_t m_line = 0;
            size_t m_offset = 0;
        };

        enum parsing_node_type : uint32_t  // for parsing only
        {
            _uninitialized = jsonland::node_type::uninitialized_t,
            _null = jsonland::node_type::null_t,
            _bool = jsonland::node_type::bool_t,
            _num = jsonland::node_type::number_t,
            _str = jsonland::node_type::string_t,
            _array = jsonland::node_type::array_t,
            _obj = jsonland::node_type::object_t,

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
            m_array_values_stack.reserve(512);
            m_obj_keys_stack.reserve(512);
        }

    private:
        json_doc& m_top_node;

        const size_t m_str_size;
        const char* m_start = nullptr;
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
        
        inline char next_chars(const ssize_t in_num_chars_to_skip)
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

        inline ssize_t num_remaining_chars()
        {
           return m_end - m_curr_char;
        }

        struct func_type_pair
        {
            bool (Parser::*m_func)(parsing_node_type, json_node&);
            parsing_node_type m_type;
        };
        func_type_pair m_char_to_token_table[256];

        void prepare_char_to_token_table()
        {
            for (int i = 0; i < 256; ++i)
                m_char_to_token_table[i] = {&Parser::skip_one_char, parsing_node_type::_uninitialized};

            m_char_to_token_table['['] = {&Parser::parse_array, parsing_node_type::_array};
            m_char_to_token_table[']'] = {&Parser::parse_control_char, parsing_node_type::_array_close};
            m_char_to_token_table[','] = {&Parser::parse_control_char, parsing_node_type::_comma};
            m_char_to_token_table['{'] = {&Parser::parse_obj, parsing_node_type::_obj};
            m_char_to_token_table['}'] = {&Parser::parse_control_char, parsing_node_type::_obj_close};
            m_char_to_token_table[':'] = {&Parser::parse_control_char, parsing_node_type::_colon};
            m_char_to_token_table['"'] = {&Parser::parse_string, parsing_node_type::_str};
            m_char_to_token_table['0'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['1'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['2'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['3'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['4'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['5'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['6'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['7'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['8'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['9'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['-'] = {&Parser::parse_number, parsing_node_type::_num};
            m_char_to_token_table['f'] = {&Parser::parse_constant, parsing_node_type::_bool};
            m_char_to_token_table['n'] = {&Parser::parse_constant, parsing_node_type::_null};
            m_char_to_token_table['t'] = {&Parser::parse_constant, parsing_node_type::_bool};
            m_char_to_token_table['\n'] = {&Parser::skip_new_line, parsing_node_type::_uninitialized};
            m_char_to_token_table['\r'] = {&Parser::skip_whilespace, parsing_node_type::_uninitialized};
            m_char_to_token_table['\t'] = {&Parser::skip_whilespace, parsing_node_type::_uninitialized};
            m_char_to_token_table[' '] = {&Parser::skip_whilespace, parsing_node_type::_uninitialized};
#if 0 // case insensitive constants
            //m_char_to_token_table['F'] = m_char_to_token_table['f'];
            //m_char_to_token_table['N'] = m_char_to_token_table['n'];
            //m_char_to_token_table['T'] = m_char_to_token_table['t'];
#endif
        }

        inline bool get_next_node(json_node& out_node)
        {
            bool retVal = false;
            while (JSONLAND_LIKELY(!retVal && is_there_more_data()))
            {
                char curr_char = *m_curr_char;
                if (256 > (unsigned int)curr_char)
                {
                    func_type_pair& call_pair = m_char_to_token_table[curr_char];
                    retVal = (this->*call_pair.m_func)(call_pair.m_type, out_node);
                }
                else
                {
                    func_type_pair& call_pair = m_char_to_token_table[0];
                    retVal = (this->*call_pair.m_func)(call_pair.m_type, out_node);
                }
            }
            return retVal;
        }

        bool parse_string(parsing_node_type, json_node& out_node)
        {
            // if parse_string was called then *curr == '"'
            bool retVal = false;

            char curr_char = next_char();
            char* str_start = m_curr_char;

            int num_escapes = 0;
            while (JSONLAND_LIKELY(is_there_more_data()))
            {
                if (JSONLAND_UNLIKELY(curr_char == '\\'))
                {
                    ++num_escapes;
                    curr_char = next_char();
                    if (JSONLAND_UNLIKELY(! is_there_more_data())) // the '\' was  the last char
                    {
                        std::string message = "escape char '\\' is the last char";
                        throw parsing_exception(message.c_str(), curr_offset());
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
                else if (JSONLAND_UNLIKELY(is_must_be_escaped_char(curr_char)))
                {
                    std::string message = "char '";
                    message +=  name_of_control_char(curr_char);
                    message += "' should be escaped";
                    throw parsing_exception(message.c_str(), curr_offset());
                }
                else if (JSONLAND_UNLIKELY(is_illegal_string_char(curr_char)))
                {
                    std::string message = "illegal char in string";
                    throw parsing_exception(message.c_str(), curr_offset());
                }
                else if (JSONLAND_UNLIKELY(curr_char == '"'))
                    break;

                curr_char = next_char();
            }

            if (JSONLAND_LIKELY(curr_char == '"'))
            {
                *const_cast<char*>(m_curr_char) = '\0';
                
                jsonland::string_or_view sov(m_curr_char-str_start,
                                              str_start,
                                              num_escapes);
                out_node.parser_direct_set(std::move(sov), jsonland::node_type::string_t);
                next_char();
                retVal = true;
            }
            else
            {
                std::string message = "string was not terminated with '\"'";
                throw parsing_exception(message.c_str(), curr_offset());
            }

            return retVal;
        }

        bool parse_number(parsing_node_type, json_node& out_node)
        {
            char* str_start = m_curr_char;
            char curr_char = *m_curr_char;

            // json number should start with '-' or a digit
            if ('-' == curr_char)
                curr_char = next_char();

            if ('0' == curr_char) {
                // number started with zero
                // so only '.' or 'e' or 'E' are expected, not more digits
                curr_char = next_char();
                if ('.' == curr_char)
                    goto after_decimal_point;
                else if ('e' == curr_char or 'E' == curr_char)
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
                else if ('e' == curr_char or 'E' == curr_char)
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
            // after a '.' only a digit is expected
            curr_char = next_char();
            if (!isdigit(curr_char)) {
                std::string message = "expected a digit after '.' got '";
                message += curr_char;
                message += "' instead";
                throw parsing_exception(message.c_str(), curr_offset());
            }
            while (isdigit(curr_char = next_char())) ;
            if ('e' == curr_char or 'E' == curr_char)
                goto after_exponent;
            else
                goto scan_number_done;            

after_exponent:
            // after 'e' or 'E'
            curr_char = next_char();
            if ('-' == curr_char or '+' == curr_char)
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
            jsonland::string_or_view sov(m_curr_char-str_start,
                                          str_start,
                                          0);
#if JSONLAND_DEBUG==1
// in release build the number is translated from text only when to_double/to_int is called
            out_node.m_num = std::atof(sov.data());
#endif
            out_node.parser_direct_set(std::move(sov), jsonland::node_type::number_t);
            return true;
        }
        
        bool parse_constant(parsing_node_type, json_node& out_node)
        {
            bool retVal = false;

            char first_char = *m_curr_char;
            switch (first_char) {
                case 'f':
                    out_node = false;
                break;
                case 't':
                    out_node = true;
                break;
                case 'n':
                    out_node = nullptr;
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

            if (JSONLAND_UNLIKELY(num_remaining_chars() < static_cast<ssize_t>(out_node.as_string_view().size())))
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

        bool parse_control_char(parsing_node_type in_node_type, json_node& out_node)
        {
            out_node.parser_direct_set(m_curr_char, 1, static_cast<jsonland::node_type>(in_node_type));
            next_char();
            return true;
        }

        bool skip_one_char(parsing_node_type, json_node&)
        {
            throw parsing_exception("json syntax error: unexpected character", curr_offset());
            return false;
        }

        bool skip_new_line(parsing_node_type, json_node&)
        {
            ++m_current_line;
            next_char();
            m_offset_of_line = m_curr_char;
            return false;
        }

        bool skip_whilespace(parsing_node_type, json_node&)
        {
            while(is_white_space_not_new_line(next_char())) ;
            return false;
        }

        bool parse_array(parsing_node_type, json_node& out_node)
        {
            if (JSONLAND_UNLIKELY(++m_nesting_level > m_max_nesting_level))
            {
                std::array<char, 64> temp_str;
                std::string message = "Reached maximum nesting level of ";
                std::snprintf(&temp_str[0], 64, "%zu", m_max_nesting_level);
                message += &temp_str[0];
                throw parsing_exception(message.c_str(), curr_offset());
            }

            out_node = jsonland::node_type::array_t;
            size_t array_values_stack_starting_index = m_array_values_stack.size();

            uint32_t expecting = parsing_node_type::_value | parsing_node_type::_array_close;
            next_char();
            json_node next_node;
            while (JSONLAND_LIKELY(get_next_node(next_node)))
            {
                const uint32_t new_node_type = next_node.m_node_type;
                if (JSONLAND_UNLIKELY(!(new_node_type & expecting)))
                {
                    throw parsing_exception("json syntax error: unexpected token during array creation", curr_offset());
                }

                if (JSONLAND_LIKELY(new_node_type & parsing_node_type::_value))
                {
                    m_array_values_stack.push_back(std::move(next_node));
                    expecting = parsing_node_type::_comma | parsing_node_type::_array_close;
                }
                else if (new_node_type & parsing_node_type::_comma)
                {
                    expecting = parsing_node_type::_value;
                }
                else if (JSONLAND_UNLIKELY(new_node_type & parsing_node_type::_array_close))
                {
                    if (JSONLAND_LIKELY(array_values_stack_starting_index != m_array_values_stack.size()))  // is array is not empty
                    {
                        json_node::ArrayVec& array_values = out_node.as_array();
                        auto move_starts_from = std::next(m_array_values_stack.begin(), array_values_stack_starting_index);

                        array_values.insert(array_values.begin(),
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

            if (JSONLAND_UNLIKELY(0 == (next_node.m_node_type & parsing_node_type::_array_close)))
            {
                throw parsing_exception("unexpected end of tokens during array initializtion", curr_offset());
            }
            
            return false;
        }

        bool parse_obj(parsing_node_type, json_node& out_node)
        {
            if (JSONLAND_UNLIKELY(++m_nesting_level > m_max_nesting_level))
            {
                std::array<char, 64> temp_str;
                std::string message = "Reached maximum nesting level of ";
                std::snprintf(&temp_str[0], 64, "%zu", m_max_nesting_level);
                throw parsing_exception(message.c_str(), curr_offset());
            }

            out_node = jsonland::node_type::object_t;
            size_t array_values_stack_starting_index = m_array_values_stack.size();
            size_t obj_keys_stack_starting_index = m_obj_keys_stack.size();

            bool expecting_key = true;
            jsonland::node_type expecting = static_cast<jsonland::node_type>(parsing_node_type::_str | parsing_node_type::_obj_close);
            next_char();
            json_node next_node;
            jsonland::string_or_view key;
            while (JSONLAND_LIKELY(get_next_node(next_node)))
            {
                const uint32_t new_node_type = next_node.m_node_type;
                if (JSONLAND_UNLIKELY(!(new_node_type & expecting)))
                {
                    throw parsing_exception("json syntax error: unexpected token during object initializtion", curr_offset());
                }

                if ((new_node_type & parsing_node_type::_str) && expecting_key)
                {
                    m_obj_keys_stack.emplace_back(next_node.m_value);
                    expecting_key = false;
                    expecting = static_cast<jsonland::node_type>(parsing_node_type::_colon);
                }
                else if (new_node_type & parsing_node_type::_colon)
                {
                    expecting = static_cast<jsonland::node_type>(parsing_node_type::_value);
                }
                else if (new_node_type & parsing_node_type::_value)
                {
                    next_node.m_key = m_obj_keys_stack.back();
                    m_array_values_stack.push_back(std::move(next_node));
                    expecting = static_cast<jsonland::node_type>(parsing_node_type::_comma | parsing_node_type::_obj_close);
                }
                else if (new_node_type & parsing_node_type::_comma)
                {
                    expecting = static_cast<jsonland::node_type>(parsing_node_type::_str);
                    expecting_key = true;
                }
                else if (JSONLAND_UNLIKELY(new_node_type & parsing_node_type::_obj_close))
                {
                    if (JSONLAND_LIKELY(array_values_stack_starting_index != m_array_values_stack.size()))  // if array is not empty
                    {
                        json_node::ArrayVec& array_values = out_node.as_array();
                        auto move_starts_from = std::next(m_array_values_stack.begin(), array_values_stack_starting_index);

                        array_values.reserve(m_array_values_stack.size() - array_values_stack_starting_index);
                        array_values.insert(array_values.begin(),
                                                std::make_move_iterator(move_starts_from),
                                                std::make_move_iterator(m_array_values_stack.end()));
                        m_array_values_stack.erase(move_starts_from, m_array_values_stack.end());

                        json_node::KeyToIndex& key_to_index_map = out_node.get_key_to_index_map();
                        key_to_index_map.reserve(m_obj_keys_stack.size() - obj_keys_stack_starting_index);
                        int index = 0;
                        auto obj_key_stack_start = std::next(m_obj_keys_stack.begin(), obj_keys_stack_starting_index);
                        for (auto iterKey = obj_key_stack_start;
                             iterKey != m_obj_keys_stack.end(); ++iterKey)
                        {
                            key_to_index_map[*iterKey] = index++;
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

            if (JSONLAND_UNLIKELY(0 == (next_node.m_node_type & parsing_node_type::_obj_close)))
            {
                throw parsing_exception("unexpected end of tokens during object initializtion", curr_offset());
            }
            
            return false;
        }

        // skip possibly whitespace at the beginning or end, while still keeping count of the number of new lines
        int skip_whitespace()
        {
            parsing_node_type type_dummy = _uninitialized;
            json_node node_dummy;

            while (is_there_more_data())
            {
                if ('\n' == *m_curr_char) {
                    skip_new_line(type_dummy, node_dummy);
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
                    bool found_json = get_next_node(m_top_node);
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


bool jsonland::operator==(const jsonland::json_node& lhs, const jsonland::json_node& rhs)
{
    bool retVal = false;

    if (lhs.m_node_type == rhs.m_node_type)
    {
        switch(lhs.m_node_type)
        {
            case jsonland::node_type::object_t:
                retVal = lhs.m_values == rhs.m_values;
            break;
            case jsonland::node_type::array_t:
                retVal = lhs.m_values == rhs.m_values;
            break;
            case jsonland::node_type::number_t:
                // comparing two number in text representation creates a dilema:
                // what if the text representation is different but the actual number are the same?
                // e.g. "1.000000000000000011", "1.000000000000000012"
                // Answer: if both numbers are text - compare the text, otherwise compare m_num
                if (!lhs.is_number_assigned() && !rhs.is_number_assigned())
                    retVal = lhs.m_value == rhs.m_value;
                else
                {
                    double my_num = lhs.as_double();
                    double other_num = rhs.as_double();
                    retVal = my_num == other_num;
                }
            break;
            case jsonland::node_type::string_t:
            case jsonland::node_type::bool_t:
                retVal = lhs.m_value == rhs.m_value;
            break;
            case jsonland::node_type::null_t:
            default:
                retVal = true;
            break;
        }
    }

    return retVal;
}

bool jsonland::operator!=(const jsonland::json_node& lhs, const jsonland::json_node& rhs)
{
    return !(lhs == rhs);
}

std::ostream& jsonland::operator<<(std::ostream& os, const jsonland::json_node& jn)
{
    return jn.dump(os);
}
