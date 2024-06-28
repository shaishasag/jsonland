#include "jsonland/json_creator.h"


namespace jsonland
{
namespace internal
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
template int int_to_str<int>(const int value_to_convert, char buf[], int buf_size);
template int int_to_str<unsigned int>(const unsigned int value_to_convert, char buf[], int buf_size);
template int int_to_str<short>(const short value_to_convert, char buf[], int buf_size);
template int int_to_str<unsigned short>(const unsigned short value_to_convert, char buf[], int buf_size);
template int int_to_str<long>(const long value_to_convert, char buf[], int buf_size);
template int int_to_str<unsigned long>(const unsigned long value_to_convert, char buf[], int buf_size);
template int int_to_str<long long>(const long long value_to_convert, char buf[], int buf_size);
template int int_to_str<unsigned long long>(const unsigned long long value_to_convert, char buf[], int buf_size);

template<typename TNumber, typename TStr>
int int_to_str(const TNumber value_to_convert, TStr& out_str)
{
    out_str.resize(out_str.size()+40);
    int num_chars = int_to_str(value_to_convert, out_str.data()+out_str.size()-40, 40);
    out_str.resize(out_str.size() - (40-num_chars));
    return num_chars;
}
template int int_to_str<int, std::string>(const int value_to_convert, std::string& out_str);
template int int_to_str<unsigned int, std::string>(const unsigned int value_to_convert, std::string& out_str);
template int int_to_str<short, std::string>(const short value_to_convert, std::string& out_str);
template int int_to_str<unsigned short, std::string>(const unsigned short value_to_convert, std::string& out_str);
template int int_to_str<long, std::string>(const long value_to_convert, std::string& out_str);
template int int_to_str<unsigned long, std::string>(const unsigned long value_to_convert, std::string& out_str);
template int int_to_str<long long, std::string>(const long long value_to_convert, std::string& out_str);
template int int_to_str<unsigned long long, std::string>(const unsigned long long value_to_convert, std::string& out_str);

template<typename TFloat, typename TStr>
int float_to_str(const TFloat value_to_convert, TStr& out_str, int percision)
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
        
        size_t stript_from_end{0};
        for (auto i = out_str.rbegin(); i != out_str.rend(); ++i)
        {
            if ('0' == *i) {
                ++stript_from_end;
            }
            else if ('.' == *i) {
                --stript_from_end;
                break;
            }
            else
                break;
        }
        out_str.resize(out_str.size()-stript_from_end);
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
template int float_to_str<float, std::string>(const float value_to_convert, std::string& out_str, int percision);
template int float_to_str<double, std::string>(const double value_to_convert, std::string& out_str, int percision);
}

template<typename TStr>
creator_base<TStr>::save_restore_end::save_restore_end(creator_base& to_save)
: m_creator_to_save(to_save)
{
    std::string_view end_to_save = m_creator_to_save.save_end();
    std::memcpy(m_save_buf,
                end_to_save.data(),
                end_to_save.size());
    to_save.m_json_str.resize(to_save.m_json_str.size()-end_to_save.size());
    m_saved_end_chars = std::string_view(m_save_buf, end_to_save.size());
}
template creator_base<std::string>::save_restore_end::save_restore_end(creator_base& to_save);
#if JSONLAND_DEBUG==1
template creator_base<fixed::fstring_ref>::save_restore_end::save_restore_end(creator_base& to_save);
#endif

template<typename TStr>
creator_base<TStr>::save_restore_end::~save_restore_end()
{
    m_creator_to_save.m_json_str += m_saved_end_chars;
}
template creator_base<std::string>::save_restore_end::~save_restore_end();
#if JSONLAND_DEBUG==1
template creator_base<fixed::fstring_ref>::save_restore_end::~save_restore_end();
#endif

template<typename TStr>
std::string_view creator_base<TStr>::save_end()
{
    return std::string_view(m_json_str.data()+m_json_str.size()-m_level-1, m_level+1);
}

template<typename TStr>
void creator_base<TStr>::restore_end(std::string_view in_end)
{
    m_json_str += in_end;
}

template<typename TStr>
void object_creator<TStr>::prepare_for_additional_value(const std::string_view in_key)
{
    if (0 < this->m_num_subs) {  // not first, need to add ','
        this->m_json_str += internal::_COMMA_SPACE;
    }
    ++this->m_num_subs;
    
    this->m_json_str += '"';
    this->m_json_str += in_key;
    this->m_json_str += internal::_KEY_VAL_SEP;
}

template<typename TStr>
object_creator<TStr> object_creator<TStr>::append_object(std::string_view in_key)
{
    typename array_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value(in_key);
    object_creator retVal(this->m_json_str, this->m_level+1);

    return retVal;
}
template
object_creator<std::string> object_creator<std::string>::append_object(std::string_view in_key);

template<typename TStr>
array_creator<TStr> object_creator<TStr>::append_array(std::string_view in_key)
{
    typename array_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value(in_key);
    array_creator retVal(this->m_json_str, this->m_level+1);
    
    return retVal;
}
template
array_creator<std::string> object_creator<std::string>::append_array(std::string_view in_key);

// add a value that is already formated as json
template<typename TStr>
void object_creator<TStr>::push_json_str(const std::string_view in_key, const std::string_view in_value)
{
    typename array_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value(in_key);
    this->m_json_str += in_value;
}

template<typename TStr>
void array_creator<TStr>::prepare_for_additional_value()
{
    if (0 < this->m_num_subs) {  // not first, need to add ','
        this->m_json_str += internal::_COMMA_SPACE;
    }
    ++this->m_num_subs;
}

template <typename TStr>
array_creator<TStr> array_creator<TStr>::append_array()
{
    typename array_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value();
    array_creator retVal(this->m_json_str, this->m_level+1);
    
    return retVal;
}

template <typename TStr>
object_creator<TStr> array_creator<TStr>::append_object()
{
    typename array_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value();
    object_creator retVal(this->m_json_str, this->m_level+1);
    
    return retVal;
}


// add a value that is already formated as json
template <typename TStr>
void array_creator<TStr>::push_json_str(const std::string_view in_value)
{
    typename array_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value();
    this->m_json_str += in_value;
}

template <typename TStr>
void array_creator<TStr>::append_value(const char* in_value)
{
    typename array_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value();
    internal::write_value(in_value, this->m_json_str);
}
template void array_creator<std::string>::append_value(const char* in_value);

}
