#include "jsonland/json_creator.h"

using namespace jsonland;

namespace
{

// 18,446,744,073,709,551,615 max uint64_t, 20 digits
// 340,282,366,920,938,463,463,374,607,431,768,211,456  max uint128_t, 39 digits
template<typename TNumber>
int int_to_str(const TNumber value_to_convert, char buf[], int buf_size)
{
    TNumber val{value_to_convert};
    
    bool negative{false};
    if constexpr(std::is_signed_v<TNumber>)
    {
        if (val < 0)
        {
            val = -val;
            negative = true;
        }
    }
    
    char* it = buf + buf_size - 1;
    TNumber quot = val / 10;
    TNumber rem = val % 10;
    while(quot)
    {
        *it = '0' + rem;
        val = quot;
        quot = val / 10;
        rem = val % 10;
        --it;
    }

    *it = '0' + rem;
    
    long num_chars = (buf + buf_size) - it;
    if (negative)
    {
        *buf = '-';
        memcpy(buf+1, it, num_chars);
        ++num_chars;
    }
    else
    {
        memcpy(buf, it, num_chars);
    }

    return static_cast<int>(num_chars);
}

template<typename TNumber>
int int_to_str(const TNumber value_to_convert, std::string& out_str)
{
    out_str.resize(out_str.size()+40);
    int num_chars = int_to_str(value_to_convert, out_str.data()+out_str.size()-40, 40);
    out_str.resize(out_str.size() - (40-num_chars));
    return num_chars;
}

template<typename TFloat>
int float_to_str(const TFloat value_to_convert, char buffer_to_fill[], int buf_size, int percision=6)
{
    int num_chars = 0;
    if (std::isnormal(value_to_convert))
    {
        TFloat val{value_to_convert};
        if (val < 0)
        {
            val = -val;
            buffer_to_fill[num_chars++] = '-';
        }
        
        int64_t int_part = static_cast<uint64_t>(val);
        num_chars += int_to_str(int_part, buffer_to_fill+num_chars, buf_size-num_chars);
        
        buffer_to_fill[num_chars++] = '.';

        percision = std::max(1, percision);
        percision = std::min(buf_size-num_chars, percision);
        
        // farc_part_as_int is raised to percision+1, 5 is added, for rounding, /= 10
        int64_t farc_part_as_int = static_cast<int64_t>(std::abs(val - int_part) * std::pow(10, percision+1));
        farc_part_as_int +=  5;
        
        //for (int i=num_chars; i<buf_size; ++i) {buffer_to_fill[i] = 'X';}

        for (int i = percision - 1; i >= 0; --i) {
            farc_part_as_int /= 10;
            buffer_to_fill[num_chars+i] = '0' + farc_part_as_int % 10;
        }
        
        num_chars += percision;
        while (buffer_to_fill[num_chars-1] == '0') {
            --num_chars;
        }
        if (buffer_to_fill[num_chars-1] == '.') {
            ++num_chars;
        }
        
    }
    else if (TFloat(0.0) == value_to_convert) {  // std::isnormal(0.0)==false
        memcpy(buffer_to_fill, "0.0", 3);
        num_chars = 3;
    }
    else if (std::isnan(value_to_convert)) {
        memcpy(buffer_to_fill, "nan", 3);
        num_chars = 3;
    }
    else if (std::isinf(value_to_convert)) {
        memcpy(buffer_to_fill, "inf", 3);
        num_chars = 3;
    }
    
    return num_chars;
}

template<typename TFloat>
int float_to_str(const TFloat value_to_convert, std::string& out_str, int percision=6)
{
    size_t num_chars_before = out_str.size();
    if (std::isnormal(value_to_convert))
    {
        TFloat val{value_to_convert};
        if (val < 0)
        {
            val = -val;
            out_str += '-';
        }
        
        int64_t int_part = static_cast<uint64_t>(val);
        int_to_str(int_part, out_str);
        
        out_str += '.';

        percision = std::max(1, percision);
        
        // farc_part_as_int is raised to percision+1, 5 is added, for rounding, /= 10
        int64_t farc_part_as_int = static_cast<int64_t>(std::abs(val - int_part) * std::pow(10, percision+1));
        farc_part_as_int +=  5;
        
        out_str.resize(out_str.size()+percision);
        for (int i = 1; i <= percision; ++i) {
            farc_part_as_int /= 10;
            out_str[out_str.size()-i] = '0' + farc_part_as_int % 10;
        }
        
        while (out_str.back() == '0') {
            out_str.resize(out_str.size()-1);
        }
        if (out_str.back() == '.') {
            out_str += '0';
        }
        
    }
    else if (TFloat(0.0) == value_to_convert) {  // std::isnormal(0.0)==false
        out_str += "0.0";
    }
    else if (std::isnan(value_to_convert)) {
        out_str += "nan";
    }
    else if (std::isinf(value_to_convert)) {
        out_str += "inf";
    }
    
    return static_cast<int>(out_str.size() - num_chars_before);
}

template <typename TValue>
void write_value(TStr& in_json_str, const TValue in_value)
{
    char buff[256];
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
    else if constexpr (std::is_same_v<std::byte, TValue>)
    {
        [[maybe_unused]] int num_chars = int_to_str(in_value, in_json_str);
    }
    else if constexpr (std::is_integral_v<TValue>)
    {
        [[maybe_unused]] int num_chars = int_to_str(in_value, in_json_str);
    }
    else if constexpr (std::is_floating_point_v<TValue>)
    {
        int num_chars = float_to_str<TValue>(in_value, buff, 256);
        in_json_str.append(buff, buff+num_chars);
    }
    else if constexpr (std::is_null_pointer_v<TValue>)
    {
        in_json_str += _NULL;
    }
    else if constexpr (std::is_convertible_v<TValue, std::string_view>)
    {
        in_json_str += '"';
        in_json_str += std::string_view(in_value);
        in_json_str += '"';
    }
    else
    {
        in_json_str += '"';
        in_json_str += in_value;
        in_json_str += '"';
    }
}
}

template<const std::string_view& t_empty_structure, size_t t_max_levels>
creator_base<t_empty_structure, t_max_levels>::save_restore_end::save_restore_end(creator_base& to_save)
: m_to_save(to_save)
{
    std::string_view end_to_save = m_to_save.save_end();
    std::memcpy(m_save_buf,
                end_to_save.data(),
                end_to_save.size());
    to_save.m_json_str.resize(to_save.m_json_str.size()-end_to_save.size());
    m_saved_end = std::string_view(m_save_buf, end_to_save.size());
}

template<const std::string_view& t_empty_structure, size_t t_max_levels>
creator_base<t_empty_structure, t_max_levels>::save_restore_end::~save_restore_end()
{
    m_to_save.m_json_str += m_saved_end;
}

template<const std::string_view& t_empty_structure, size_t t_max_levels>
std::string_view creator_base<t_empty_structure, t_max_levels>::save_end()
{
    return std::string_view(m_json_str.data()+m_json_str.size()-m_level-1, m_level+1);
}

template<const std::string_view& t_empty_structure, size_t t_max_levels>
void creator_base<t_empty_structure, t_max_levels>::restore_end(std::string_view in_end)
{
    m_json_str += in_end;
}

template<const std::string_view& t_empty_structure, size_t t_max_levels>
void creator_base<t_empty_structure, t_max_levels>::prepare_for_additional_value()
{
    if (0 < m_num_subs) {  // not first, need to add ','
        m_json_str += _COMMA_SPACE;
    }
    ++m_num_subs;
}

void object_creator::prepare_for_additional_value(const std::string_view in_key)
{
    creator_base::prepare_for_additional_value();
    
    m_json_str += '"';
    m_json_str += in_key;
    m_json_str += _KEY_VAL_SEP;
}

object_creator object_creator::append_object(std::string_view in_key)
{
    save_restore_end save_end(*this);
    prepare_for_additional_value(in_key);
    object_creator retVal(m_json_str, m_level+1);
    
    return retVal;
}

array_creator object_creator::append_array(std::string_view in_key)
{
    save_restore_end save_end(*this);
    prepare_for_additional_value(in_key);
    array_creator retVal(m_json_str, m_level+1);
    
    return retVal;
}

// add a value that is already formated as json
void object_creator::push_json_str(const std::string_view in_key, const std::string_view in_value)
{
    save_restore_end save_end(*this);
    prepare_for_additional_value(in_key);
    m_json_str += in_value;
}

template <typename TValue>
void object_creator::append(const std::string_view in_key, const TValue&& in_value)
{
    save_restore_end save_end(*this);
    prepare_for_additional_value(in_key);
    write_value(m_json_str, in_value);
}
template void object_creator::append(const std::string_view in_key, const float&& in_value);
template void object_creator::append(const std::string_view in_key, const double&& in_value);
template void object_creator::append(const std::string_view in_key, const std::string_view&& in_value);
template void object_creator::append(const std::string_view in_key, const short&& in_value);
template void object_creator::append(const std::string_view in_key, const unsigned short&& in_value);
template void object_creator::append(const std::string_view in_key, const int&& in_value);
template void object_creator::append(const std::string_view in_key, const unsigned int&& in_value);
template void object_creator::append(const std::string_view in_key, const long&& in_value);
template void object_creator::append(const std::string_view in_key, const unsigned long&& in_value);


array_creator array_creator::append_array()
{
    save_restore_end save_end(*this);
    prepare_for_additional_value();
    array_creator retVal(m_json_str, m_level+1);
    
    return retVal;
}

object_creator array_creator::append_object()
{
    save_restore_end save_end(*this);
    prepare_for_additional_value();
    object_creator retVal(m_json_str, m_level+1);
    
    return retVal;
}


// add a value that is already formated as json
void array_creator::push_json_str(const std::string_view in_value)
{
    save_restore_end save_end(*this);
    prepare_for_additional_value();
    m_json_str += in_value;
}

template <typename TValue>
void array_creator::append(const TValue& in_value)
{
    save_restore_end save_end(*this);
    prepare_for_additional_value();
    write_value(m_json_str, in_value);
}
template void array_creator::append(const float& in_value);
template void array_creator::append(const double& in_value);
template void array_creator::append(const std::string_view& in_value);

void array_creator::append(const char* in_value)
{
    save_restore_end save_end(*this);
    prepare_for_additional_value();
    write_value(m_json_str, in_value);
}


