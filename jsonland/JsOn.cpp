#define JSONLAND_INTERNAL_PARSING_FUNCTIONS
#include "jsonland/JsOn.h"

#include <string_view>
#include <source_location>
#include <charconv>

using namespace std::literals;
using namespace jsonland;

// rule: skip_ws before each function except parse_inplace ???
// or
// each function skips it's own ws ???


// todo:
// line numbers and offsets of json text in error
// line numbers and offsets of code in error
// elaborate exception description
// handle escapes
// parse_number: implement all types of numbers (currently only ints)
// parse_array handle leading or trailing ',' [, "a", "b", ,]
// fast_float::from_chars/std::from_chars

namespace jsonland
{
namespace parser_helper
{

enum ParseErrors : int32_t
{
    no_err = 0,
    unexpected_separator = -1,
    unexpected_char = -2,
    array_not_terminated = -3,
    object_not_terminated = -4,
    string_not_terminated = -5,
    invalid_constant = -6,
    invalid_number = -7,
    invalid_string = -8,
};

class JsOn_parser_exception : public std::exception
{
public:
    JsOn_parser_exception(int32_t err_num, std::string&& message)
    : m_err_num(err_num)
    , m_msg(std::move(message))
    {
    }
    
    int error_num()  const noexcept  { return m_err_num; }
    
    const char* what() const noexcept override
    {
        return m_msg.c_str();
    }
    
private:
    int32_t m_err_num{0};
    std::string m_msg;
};


class JsOn_parser
{
private:
    JsOn& m_top_level_json;
    std::string_view m_original_text;
    std::string_view m_current_text;
    size_t m_line = 1;
    const char* m_curr_line_ptr = nullptr;
    size_t m_curr_line_offset = 0;
    
    
public:
    
    JsOn_parser(const std::string_view in_text, JsOn& out_j)
    : m_top_level_json(out_j)
    , m_original_text(in_text)
    , m_current_text(in_text)
    {
        if (!m_original_text.empty())
        {
            m_curr_line_ptr = m_original_text.begin();
        }
    }
    
    template<typename... Args>
    JsOn_parser_exception create_exception(std::source_location loc,
                                           int32_t err_num,
                                           Args&&... args)
    {
        std::ostringstream oss;
        ((oss << std::forward<Args>(args) << ' '), ...);
        oss << "\nat line " << m_line << " column " << m_current_text.begin() - m_curr_line_ptr;
        oss << "\nC++ source file: " << loc.file_name() << " line " << loc.line();
        
        return JsOn_parser_exception(err_num, oss.str());
    }
    
    
    void parse()
    {
        parse(m_top_level_json);
    }
    
    void inline skip_one()
    {
        m_current_text = m_current_text.substr(1);
    }
    
    void parse(JsOn& out_j)
    {
        skip_whitespace();
        if (m_current_text.empty())
        {
            return;
        }
        
        char curr_char = m_current_text.front();
        switch (curr_char)
        {
            case '{':
            {
                parse_object(out_j);
                
            }
                break;
            case '[':
            {
                parse_array(out_j);
            }
                break;
            case '"':
            {
                parse_string(out_j);
            }
                break;
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
            case '-':
            {
                parse_number(out_j);
            }
                break;
            case 't':
            {
                parse_constant(the_true_string_view, out_j);
                out_j.m_value_type = bool_t;
            }
                break;
            case 'f':
            {
                parse_constant(the_false_string_view, out_j);
                out_j.m_value_type = bool_t;
            }
                break;
            case 'n':
            {
                parse_constant(the_null_string_view, out_j);
                out_j.m_value_type = null_t;
            }
                break;
        }
    }
    
    inline void parse_object(JsOn& out_j)
    {
        assert(!m_current_text.empty());
        assert(m_current_text.front() == '{');
        
        out_j.m_value_type = object_t;
        
        skip_one(); // skip the '{' and whitespace
        enum obj_parse_states {before_key, after_key, before_value, after_value};
        obj_parse_states _state{before_key};
        JsOn key; // JsOn object just for the m_value that will be used as the key
        while (!m_current_text.empty())
        {
            skip_whitespace();
            char sep = m_current_text.front(); // '}'? ','
            switch (_state)
            {
                case before_key:
                    if ('}' == sep)
                    {
                        skip_one();
                        return;
                    }
                    else if ('"' == sep)
                    {
                        parse_string(key);
                        _state = after_key;
                    }
                    else
                    {
                        throw create_exception(std::source_location::current(),
                                               unexpected_char,
                                               "'}' or '\"'", "were expeced not", sep);
                    }
                    break;
                case after_key:
                    if (':' == sep)
                    {
                        skip_one();
                        _state = before_value;
                    }
                    else
                    {
                        throw create_exception(std::source_location::current(),
                                               unexpected_separator,
                                               "':'", "was expeced not", sep);
                    }
                    break;
                case before_value:
                {
                    JsOn& value_j = out_j.m_obj_values[key.m_value];
                    parse(value_j);
                    _state = after_value;
                }
                    break;
                case after_value:
                    if (',' == sep)
                    {
                        skip_one();
                        _state = before_key;
                    }
                    else if ('}' == sep)
                    {
                        skip_one();
                        return; // this is where the function should return if all is OK
                    }
                    else
                    {
                        throw create_exception(std::source_location::current(),
                                               unexpected_char,
                                               "'}' or ','", "were expeced not", sep);
                    }
            }
        }
        
        throw create_exception(std::source_location::current(),
                               object_not_terminated,
                               "object was not terminated");
    }
    
    inline void parse_array(JsOn& out_j)
    {
        assert(!m_current_text.empty());
        assert(m_current_text.front() == '[');
        
        out_j.m_value_type = array_t;
        
        skip_one(); // skip the '['
        while (!m_current_text.empty())
        {
            skip_whitespace();
            char sep = m_current_text.front(); // ']'? ','
            if (']' == sep)
            {
                skip_one();
                return; // this is where the function should return if all is OK
            }
            else if (',' == sep) // todo: detect leading or trailing ','
            {
                skip_one();
            }
            else
            {
                parse(out_j.m_array_values.emplace_back());
            }
        }
        assert(m_current_text.front() == ']');
        
        throw create_exception(std::source_location::current(),
                               array_not_terminated,
                               "']'", "was expeced");
        
    }
    
    inline void skip_whitespace()
    {
        const char* pc{nullptr};
        for (pc = m_current_text.begin(); pc != m_current_text.end(); ++pc)
        {
            if (! is_whitespace(*pc))
            {
                m_curr_line_offset = pc - m_curr_line_ptr;
                m_current_text = m_current_text.substr(std::distance(m_current_text.begin(), pc));
                return;
            }
            else if (*pc == '\n')
            {
                ++m_line;
                m_curr_line_ptr = pc;
                m_curr_line_offset = 0;
            }
        }
    }
       
    inline void parse_string(JsOn& out_j)
    {
        assert(!m_current_text.empty());
        out_j.m_value_type = string_t;
        
        struct error_info
        {
            std::source_location loc;
            std::string_view error_msg;
        };
        error_info last_error_info{std::source_location::current(),
            "unexpected error during string parsing"sv};
        
        std::string_view starting_text = m_current_text;
        
        enum string_parse_states {
            got_a_char,
            started_escape,
            expected_hex_char,
            scan_string_done,
            scan_string_failed
        };
        string_parse_states _state{got_a_char};
        char current_char = m_current_text.front();
        
        int hex_char_counter{0}; // after \u 4 hex chars are expected
        
        skip_one();
        
        while (!m_current_text.empty())
        {
            current_char = m_current_text.front();
            switch(_state)
            {
                case got_a_char: [[likely]]
                    if ('"' == current_char) {
                        _state = scan_string_done;
                        skip_one();
                    }
                    else if ('\\' == current_char) {
                        _state = started_escape;
                        skip_one();
                    }
                    else if (is_a_char_that_must_be_escaped(current_char)) {
                        last_error_info = {std::source_location::current(),
                            "unexcaped char"sv};
                        _state = scan_string_failed;
                    }
                    else { [[likely]] // another char
                        skip_one();
                    }
                    break;
                case started_escape:
                    if (! is_escapable_char(current_char))
                    {
                        last_error_info = {std::source_location::current(),
                            "char should not be escpaed"sv};
                        _state = scan_string_failed;
                    }
                    else if ('u' == current_char)  // unicode escape
                    {
                        skip_one();
                        hex_char_counter = 4;
                        _state = expected_hex_char;
                    }
                    else { [[likely]] // another char
                        skip_one();
                        _state = got_a_char;
                    }
                    break;
                case expected_hex_char:
                    if (is_hex_digit(current_char)) {
                        _state = --hex_char_counter == 0 ? got_a_char : expected_hex_char;
                        skip_one();
                    }
                    else {
                        last_error_info = {std::source_location::current(),
                                            "expected 4 hex char after \\u"sv};
                        _state = scan_string_failed;

                    }
                    break;
                case scan_string_done:
                {
                    auto _str_size = m_current_text.begin() - starting_text.begin();
                    out_j.m_value = std::string_view(starting_text.begin(), _str_size);
                    //std::cout << "String: " << out_j.m_value << std::endl;
                }
                    return;
                case scan_string_failed:
                {
                    throw create_exception(last_error_info.loc,
                                           invalid_string,
                                           last_error_info.error_msg,
                                           "; string parsed:>"sv,
                                           starting_text.substr(0, 10),
                                           "<"sv);
                    
                }
                    break;
            }
        }
        
        // If we got here it's unexpected error not handled in the code.
        // The expected flow to end parse_string is either to get to:
        // - scan_string_done and return
        // or
        // - scan_string_failed and throw.
        throw create_exception(last_error_info.loc,
                               invalid_number,
                               last_error_info.error_msg);
        
    }
        
    // Note: JSON numbers must have at least one digit before and after the decimal point when a fraction is used, and must have digits before the exponent as well.
    void parse_number(JsOn& out_node)
    {
        assert(!m_current_text.empty());
        out_node.m_value_type = number_t;
        out_node.m_hints.set_hint(_num_is_int);
        out_node.m_hints.set_hint(_num_in_string);
                
        struct error_info
        {
            std::source_location loc;
            std::string_view error_msg;
        };
        error_info last_error_info{std::source_location::current(),
                                    "unexpected error during number parsing"sv};
        
        std::string_view starting_text = m_current_text;
        
        enum number_parse_states {
            got_a_digit,
            started_minus,
            started_zero,
            first_decimal_digit,
            next_decimal_digit,
            exponent_started,
            first_exponent_digit,
            next_exponent_digit,
            scan_number_done,
            scan_numer_failed
        };
        number_parse_states _state{got_a_digit};
        char current_char = m_current_text.front();
        if ('0' == current_char) _state = started_zero;
        else if ('-' == current_char) _state = started_minus;
        
        skip_one();

        while (!m_current_text.empty())
        {
            current_char = m_current_text.front();
            switch(_state)
            {
                case got_a_digit: [[likely]]
                    if ('.' == current_char) {
                        _state = first_decimal_digit;
                        skip_one();
                    }
                    else if ('e' == current_char || 'E' == current_char) {
                        _state = exponent_started;
                        skip_one();
                    }
                    else if (!isdigit(current_char)) {
                        _state = scan_number_done;
                    }
                    else { [[likely]] // another digit
                        skip_one();
                    }
                    break;
                case started_minus:
                    if (isdigit(current_char)) [[likely]] {
                        _state = got_a_digit;
                        skip_one();
                    }
                    else {
                        last_error_info = {std::source_location::current(),
                                            "digits expected after '-'"sv};
                        _state = scan_numer_failed;
                    }
                    break;
                case started_zero:
                    if (isdigit(current_char)) {
                        last_error_info = {std::source_location::current(),
                                            "digits are not expected after '0'"sv};
                        _state = scan_numer_failed;
                    }
                    else if ('.' == current_char) [[likely]] {
                        _state = first_decimal_digit;
                        skip_one();
                    }
                    else if ('e' == current_char || 'E' == current_char) {
                        _state = exponent_started;
                        skip_one();
                    }
                    else {
                        _state = scan_number_done;
                    }
                    break;
               case first_decimal_digit:
                    out_node.m_hints.unset_hint(_num_is_int);
                    if (!isdigit(current_char)) {
                        last_error_info = {std::source_location::current(),
                                            "digit are expected after '.'"sv};
                        _state = scan_numer_failed;
                    }
                    else [[likely]] {
                        _state = next_decimal_digit;
                        skip_one();
                    }
                    break;
                case next_decimal_digit:
                    if (isdigit(current_char)) [[likely]] {
                        skip_one();
                    }
                    else if ('e' == current_char || 'E' == current_char) {
                        _state = exponent_started;
                        skip_one();
                    }
                    else {
                        _state = scan_number_done;
                    }
                    break;
                case exponent_started:
                    out_node.m_hints.unset_hint(_num_is_int);
                    if ('+' == current_char || '-' == current_char) {
                        _state = first_exponent_digit;
                        skip_one();
                    }
                    else if (isdigit(current_char)) [[likely]] {
                        _state = next_exponent_digit;
                        skip_one();
                    }
                    else {
                        last_error_info = {std::source_location::current(),
                                            "digit or '-', '+' are expected after 'E';"sv};
                        _state = scan_numer_failed;
                    }
                    break;
                case first_exponent_digit:
                    if (isdigit(current_char)) [[likely]] {
                        skip_one();
                        _state = next_exponent_digit;
                    }
                    else {
                        last_error_info = {std::source_location::current(),
                                            "digit are expected after 'E';"sv};
                        _state = scan_numer_failed;
                    }
                    break;
                case next_exponent_digit:
                    if (isdigit(current_char)) [[likely]] {
                        skip_one();
                    }
                    else {
                        _state = scan_number_done;
                    }
                    break;
                case scan_number_done:
                    {
                        auto number_str_size = m_current_text.begin() - starting_text.begin();
                        out_node.m_value = std::string_view(starting_text.begin(), number_str_size);
                        //std::cout << "Number: " << out_node.m_value << std::endl;
                    }
                    return;
                case scan_numer_failed:
                    {
                        throw create_exception(last_error_info.loc,
                                               invalid_number,
                                               last_error_info.error_msg,
                                               "; number parsed:>"sv,
                                               starting_text.substr(0, 10),
                                               "<"sv);
                        
                    }
                    break;
            }
        }

        // If we got here it's unexpected error not handled in the code.
        // The expected flow to end parse_number is either to get to:
        // - scan_number_done and return
        // or
        // - scan_numer_failed and throw.
        throw create_exception(last_error_info.loc,
                               invalid_number,
                               last_error_info.error_msg);

    }

    
    inline void parse_constant(std::string_view the_const, JsOn& out_j)
    {
        assert(!m_current_text.empty());
        assert(m_current_text.front() == the_const.front());
        
        out_j.m_value = m_current_text.substr(0, the_const.size());
        m_current_text = m_current_text.substr(the_const.size());
        if (out_j.m_value != the_const)
        {
            throw create_exception(std::source_location::current(),
                                   invalid_constant,
                                   "'true' was expeced not ", out_j.m_value);
        }
    }
};
}

JsOn::JsOn(value_type in_type, size_t in_reserve) noexcept
: m_value_type(in_type)
{
    switch (m_value_type)
    {
        case null_t:
            m_value = the_null_string_view;
            break;
        case number_t:
            m_hints.set_hint(_num_is_int);
            m_int = 0;
            break;
        case bool_t:
            m_value = the_false_string_view;
            break;
        case object_t:
            m_obj_values.reserve(in_reserve);
            break;
        case array_t:
            m_array_values.reserve(in_reserve);
            break;
        default:
            break;
    }
}
JsOn& JsOn::operator=(jsonland::value_type in_type) noexcept
{
    if (in_type != m_value_type) {
        clear(in_type);
    }
    return *this;
}

JsOn& JsOn::operator=(const std::string_view in_str_value) noexcept
{
    clear(string_t);
    m_value = in_str_value;
    return *this;
}


std::string_view JsOn::get_string(std::string_view in_default_str) const noexcept
{
    if (is_type(string_t))
    {
        return m_value;
    }
    return in_default_str;
}

double JsOn::get_double(const double in_default_fp) const noexcept
{
    double retVal = in_default_fp;
    if (is_type(number_t))  [[likely]]
    {
        if (m_hints.get_hint(_num_in_string))
        {
            retVal = static_cast<double>(std::atof(m_value.data()));
        }
        else
        {
            if (m_hints.get_hint(_num_is_int)) {
                retVal = static_cast<double>(m_int);
            }
            else {
                retVal = m_float;
            }
        }
    }
    
    return retVal;

}

float JsOn::get_float(const float in_default_fp) const noexcept
{
    return static_cast<float>(get_double(in_default_fp));
}

int64_t JsOn::get_int(const int64_t in_default_int) const noexcept
{
    int64_t retVal = in_default_int;
    if (is_type(number_t))  [[likely]]
    {
        if (m_hints.get_hint(_num_in_string))
        {
            retVal = static_cast<int64_t>(std::atoll(m_value.data()));
        }
        else
        {
            if (m_hints.get_hint(_num_is_int)) {
                retVal = m_int;
            }
            else {
                retVal = static_cast<int64_t>(m_float);
            }
        }
    }
    
    return retVal;
}

bool JsOn::get_bool(const bool in_default_bool) const noexcept
{
    if (is_type(bool_t)) [[likely]]
    {
        return m_value.data()[0] == 't';
    }
    else
    {
        return in_default_bool;
    }
}

size_t JsOn::size_as(const value_type in_expected_type) const noexcept
{
    if (in_expected_type != m_value_type)
    {
        return 0;
    }
    
    switch (m_value_type)
    {
        case object_t:
            return m_obj_values.size();
        case array_t:
            return m_array_values.size();
        case string_t:
            return m_value.size();
        case number_t:
        case bool_t:
            return 1;
        case null_t:
        default:
            return 0;
    }
}

bool JsOn::empty_as(const value_type in_expected_type) const noexcept
{
    bool retVal = 0 == size_as(in_expected_type);
    return retVal;
}

void JsOn::clear(const value_type in_new_type) noexcept
{
    m_value_type = in_new_type;
    m_value = std::string_view();
    m_obj_values.clear();
    m_array_values.clear();
    m_hints.clear();
}

JsOn& JsOn::operator[](std::string_view key)
{
    assert(is_object() && "operator[](string_view) requires object_t");
    return m_obj_values[key];
}

const JsOn& JsOn::operator[](std::string_view key) const
{
    assert(is_object() && "operator[](string_view) requires object_t");
    auto it = m_obj_values.find(key);
    if (it != m_obj_values.end()) return it->second;
    static const JsOn null_value(null_t);
    return null_value;
}

JsOn& JsOn::operator[](size_t index)
{
    assert(is_array() && "operator[](size_t) requires array_t");
    assert(index < m_array_values.size() && "index out of bounds");
    return m_array_values[index];
}

const JsOn& JsOn::operator[](size_t index) const
{
    assert(is_array() && "operator[](size_t) requires array_t");
    if (index < m_array_values.size()) return m_array_values[index];
    static const JsOn null_value(null_t);
    return null_value;
}

// push_back
void JsOn::push_back(const JsOn& value)
{
    assert(is_array() && "push_back() called on non-array JsOn");
    m_array_values.push_back(value);
}

void JsOn::push_back(JsOn&& value)
{
    assert(is_array() && "push_back() called on non-array JsOn");
    m_array_values.push_back(std::move(value));
}

using namespace parser_helper;

int JsOn::parse_inplace(const std::string_view in_text) noexcept
{
    int retVal = 0;
    
    try
    {
        JsOn_parser parser(in_text, *this);
        parser.parse();
    }
    catch (const JsOn_parser_exception& err)
    {
        std::cout << "Error: " << err.what() << std::endl;
        retVal = err.error_num();
    }
    
    return retVal;
}

void JsOn::dump(std::string& out_str,  size_t level) const
{
    if (is_type(object_t))
    {
        out_str += '{';
        
        if (!m_obj_values.empty())
        {
            ++level;
            nl_indent(out_str, level);
            
            auto io = m_obj_values.begin();
            out_str += '"';
            out_str += io->first; // io->m_key.escape_json_string_external(out_str);
            out_str += '"';
            
            out_str += ':';
            if (!io->second.empty_as(object_t) || !io->second.empty_as(array_t))
            {
                out_str += '\n';
                indent(out_str, level);
            }
            else
            {
                out_str += ' ';
            }
            io->second.dump(out_str, level);
            
            for (++io; io != m_obj_values.end(); ++io)
            {
                out_str += ',';
                nl_indent(out_str, level);
                
                out_str += '"';
                out_str += io->first; // io->m_key.escape_json_string_external(out_str);
                out_str += '"';
                
                out_str += ':';
                if (!io->second.empty_as(object_t) || !io->second.empty_as(array_t))
                {
                    out_str += '\n';
                    indent(out_str, level);
                }
                else
                {
                    out_str += ' ';
                }
                io->second.dump(out_str, level);
            }
            --level;
            nl_indent(out_str, level);
        }
        out_str += '}';
    }
    else if (is_type(array_t))
    {
        out_str += '[';
        if (!m_array_values.empty())
        {
            ++level;
            nl_indent(out_str, level);
            
            auto ai = m_array_values.begin();
            ai->dump(out_str, level);
            
            for (++ai; ai != m_array_values.end(); ++ai)
            {
                out_str += ',';
                nl_indent(out_str, level);
                ai->dump(out_str, level);
            }
            --level;
            out_str += '\n';
            indent(out_str, level);
        }
        out_str += ']';
    }
    else if (is_type(number_t))
    {
        out_str += m_value;
    }
    else if (is_type(string_t))
    {
        out_str += '"';
        out_str += m_value; // escapes?
        out_str += '"';
    }
    else
    {
        out_str += m_value;
    }
}
}
