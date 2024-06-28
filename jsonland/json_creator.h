#ifndef __json_creator_h__
#define __json_creator_h__

#include <string.h>
#include <string>
#include <string_view>
#include <charconv>

/* Copy to include
#include "jsonland/json_creator.h"
*/


namespace jsonland
{
namespace internal
{
    constexpr std::string_view _TRUE{"true"};
    constexpr std::string_view _FALSE{"false"};
    constexpr std::string_view _NULL{"false"};
    constexpr std::string_view _COMMA_SPACE{", "};
    constexpr std::string_view _KEY_VAL_SEP{R"|(": )|"};

    template<typename TNumber>
    int int_to_str(const TNumber value_to_convert, char buf[], int buf_size);
    template<typename TNumber, typename TStr>
    int int_to_str(const TNumber value_to_convert, TStr& out_str);
    template<typename TFloat, typename TStr>
    int float_to_str(const TFloat value_to_convert, TStr& out_str, int percision);

    template <typename TStr, typename TValue>
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
        else if constexpr (std::is_same_v<std::byte, TValue>)
        {
            [[maybe_unused]] int num_chars = internal::int_to_str(in_value, in_json_str);
        }
        else if constexpr (std::is_integral_v<TValue>)
        {
            [[maybe_unused]] int num_chars = internal::int_to_str(in_value, in_json_str);
        }
        else if constexpr (std::is_floating_point_v<TValue>)
        {
            [[maybe_unused]] int num_chars = internal::float_to_str<TValue>(in_value, in_json_str, 6);
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


template<typename TStr>
class creator_base
{
private:
    static const size_t t_max_levels{15};
    
protected:
    creator_base(TStr& the_str, size_t start_level)
    : m_json_str(the_str)
    , m_level(start_level)
    {}
    
    TStr& m_json_str;

    size_t m_num_subs{0};
    size_t m_level{0};
    
    std::string_view save_end();
    void restore_end(std::string_view in_end);
        
    class save_restore_end
    {
    public:
        save_restore_end(creator_base& to_save);
        ~save_restore_end();

    private:
        creator_base&    m_creator_to_save;
        std::string_view m_saved_end_chars;
        char m_save_buf[t_max_levels+1];
    };

public:
    size_t size() { return m_json_str.size(); }
    size_t capacity() { return m_json_str.capacity(); }
    size_t max_size() { return m_json_str.max_size(); }
    const char* c_str() const { return m_json_str.c_str(); }
    operator std::string_view() { return m_json_str;}
    
    
//    friend class save_restore_end;
};

template<typename TStr> class object_creator;
template<typename TStr> class array_creator;


template<typename TStr>
class object_creator : public creator_base<TStr>
{
    friend class array_creator<TStr>;
    
private:
    constexpr static inline std::string_view empty_json_object{"{}"};

    void prepare_for_additional_value(const std::string_view in_key);
    
    // this constructor can only be called from append_object
    object_creator(TStr& the_str, size_t start_level)
    : creator_base<TStr>(the_str, start_level)
    {
        this->m_json_str += empty_json_object;
    }
    
public:
    
    object_creator(TStr& the_str)
    : creator_base<TStr>(the_str, 0)
    {
        this->m_json_str += empty_json_object;
    }
    
    object_creator append_object(std::string_view in_key)  ;
    array_creator<TStr> append_array(std::string_view in_key);
    
    // add a value that is already formated as json
    void push_json_str(const std::string_view in_key, const std::string_view in_value);

    template <typename TValue>
    void append_value(const std::string_view in_key, const TValue& in_value)
    {
        typename array_creator<TStr>::save_restore_end sv(*this);
        prepare_for_additional_value(in_key);
        internal::write_value(in_value, this->m_json_str);
    }

    void append_value(const std::string_view in_key, const char* in_value)
    {
        std::string_view as_sv(in_value);
        append_value<std::string_view>(in_key, as_sv);
    }
   
    bool empty() const { return this->m_json_str[1] == empty_json_object[1]; }
    void clear() const { this->m_json_str = empty_json_object; }
};


template<typename TStr>
class array_creator : private creator_base<TStr>
{
    friend class object_creator<TStr>;
    
private:
    constexpr static inline std::string_view empty_json_array{"[]"};
    void prepare_for_additional_value();

protected:
    array_creator(TStr& the_str, size_t start_level)
    : creator_base<TStr>(the_str, start_level)
    {
        this->m_json_str += empty_json_array;
    }
    
public:

    array_creator(TStr& the_str)
    : creator_base<TStr>(the_str, 0)
    {
        this->m_json_str += empty_json_array;
    }
    
    array_creator append_array();
    object_creator<TStr> append_object();
    
    // add a value that is already formated as json
    void push_json_str(const std::string_view in_value);
    
    void append_value() {}
    void append_value(const char* in_value);

    template <typename TValue>
    void append_value(const TValue& in_value)
    {
        typename array_creator<TStr>::save_restore_end sv(*this);
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

    bool empty() const { return this->m_json_str[1] == empty_json_array[1]; }
    void clear() const { this->m_json_str = empty_json_array; }
};
} // namespace jsonland

#endif // __json_creator_h__
