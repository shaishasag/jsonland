#ifndef __json_creator_h__
#define __json_creator_h__

#include <string.h>
#include <string>
#include <string_view>
#include <charconv>

namespace jsonland
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


using TStr = std::string;

constexpr std::string_view _TRUE{"true"};
constexpr std::string_view _FALSE{"false"};
constexpr std::string_view _NULL{"false"};
constexpr std::string_view _COMMA_SPACE{", "};
constexpr std::string_view _KEY_VAL_SEP{R"|(": )|"};

template <typename TValue>
inline static void write_value(TStr& in_json_str, const TValue& in_value)
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
        int num_chars = float_to_str(in_value, buff, 256);
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

template<const std::string_view& t_empty_structure, size_t t_max_levels=15>
class creator_base
{
    
protected:
    creator_base(TStr& the_str, size_t start_level)
    : m_json_str(the_str)
    , m_level(start_level)
    {
    }
    
    TStr& m_json_str;
    std::string_view  m_whitespace_seperator{"\n"};
    size_t m_num_subs{0};
    size_t m_level{0};
    static inline std::string_view m_empty_structure{t_empty_structure};  // "{}" or "[]"
    
    std::string_view save_end()
    {
        return std::string_view(m_json_str.data()+m_json_str.size()-m_level-1, m_level+1);
    }
    void restore_end(std::string_view in_end)
    {
        m_json_str += in_end;
    }
    
    class save_restore_end
    {
    public:
        save_restore_end(creator_base& to_save)
        : m_to_save(to_save)
        {
            std::string_view end_to_save = m_to_save.save_end();
            std::memcpy(m_save_buf,
                        end_to_save.data(),
                        end_to_save.size());
            to_save.m_json_str.resize(to_save.m_json_str.size()-end_to_save.size());
            m_saved_end = std::string_view(m_save_buf, end_to_save.size());
        }
        ~save_restore_end()
        {
            m_to_save.m_json_str += m_saved_end;
        }
    private:
        creator_base&   m_to_save;
        std::string_view m_saved_end;
        char    m_save_buf[t_max_levels+1];
    };
    
    inline void prepare_for_additional_value()
    {
        if (0 < m_num_subs) {  // not first, need to add ','
            m_json_str += _COMMA_SPACE;
        }
        ++m_num_subs;
    }
    
public:
    size_t size() { return m_json_str.size(); }
    size_t capacity() { return m_json_str.capacity(); }
    size_t max_size() { return m_json_str.max_size(); }
    const char* c_str() const { return m_json_str.c_str(); }
    operator std::string_view() { return m_json_str;}
    
    bool empty() const { return m_json_str[1] == m_empty_structure[1]; }
    
    friend class object_creator;
    friend class array_creator;
};

class object_creator;
class array_creator;

constexpr std::string_view empty_json_object{"{}"};

class object_creator : public creator_base<empty_json_object>
{
    friend class array_creator;
    
private:
    
    inline void prepare_for_additional_value(const std::string_view in_key)
    {
        creator_base::prepare_for_additional_value();
        
        m_json_str += '"';
        m_json_str += in_key;
        m_json_str += _KEY_VAL_SEP;
    }
    
    // this constructor can only be called from append_object
    object_creator(TStr& the_str, size_t start_level)
    : creator_base(the_str, start_level)
    {
        m_json_str += m_empty_structure;
    }
    
public:
    
    object_creator(TStr& the_str)
    : creator_base(the_str, 0)
    {
        m_json_str += m_empty_structure;
    }
    
    object_creator append_object(std::string_view in_key)
    {
        save_restore_end save_end(*this);
        prepare_for_additional_value(in_key);
        object_creator retVal(m_json_str, m_level+1);
        
        return retVal;
    }
    
    array_creator append_array(std::string_view in_key);
    
    // add a value that is already formated as json
    inline void push_json_str(const std::string_view in_key, const std::string_view in_value)
    {
        save_restore_end save_end(*this);
        prepare_for_additional_value(in_key);
        m_json_str += in_value;
    }
    
    template <typename TValue>
    inline void append(const std::string_view in_key, const TValue& in_value)
    {
        save_restore_end save_end(*this);
        prepare_for_additional_value(in_key);
        write_value(m_json_str, in_value);
    }
};

const std::string_view empty_json_array{"[]"};

class array_creator : private creator_base<empty_json_array>
{
    friend class object_creator;
    
private:
    
protected:
    array_creator(TStr& the_str, size_t start_level)
    : creator_base(the_str, start_level)
    {
        m_json_str += m_empty_structure;
    }
    
public:
    
    array_creator(TStr& the_str)
    : creator_base(the_str, 0)
    {
        m_json_str += m_empty_structure;
    }
    
    array_creator append_array()
    {
        save_restore_end save_end(*this);
        prepare_for_additional_value();
        array_creator retVal(m_json_str, m_level+1);
        
        return retVal;
    }
    
    object_creator append_object()
    {
        save_restore_end save_end(*this);
        prepare_for_additional_value();
        object_creator retVal(m_json_str, m_level+1);
        
        return retVal;
    }
    
    
    // add a value that is already formated as json
    inline void push_json_str(const std::string_view in_value)
    {
        save_restore_end save_end(*this);
        prepare_for_additional_value();
        m_json_str += in_value;
    }
    
    template <typename TValue>
    inline void append(const TValue& in_value)
    {
        save_restore_end save_end(*this);
        prepare_for_additional_value();
        write_value(m_json_str, in_value);
    }
    template<typename TValue, typename... Args>
    inline void append(const TValue& in_value, Args&&... args)
    {
        append(in_value);
        append(args...);
    }
    
    template<typename TContainer>
    inline void extend(const TContainer& in_values_container)
    {
        for (const auto& value : in_values_container)
        {
            append(value);
        }
    }
    
protected:
};


array_creator object_creator::append_array(std::string_view in_key)
{
    save_restore_end save_end(*this);
    prepare_for_additional_value(in_key);
    array_creator retVal(m_json_str, m_level+1);
    
    return retVal;
}

} // namespace jsonland

#endif // __json_creator_h__
