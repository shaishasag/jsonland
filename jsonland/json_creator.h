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

using TStr = std::string;

constexpr std::string_view _TRUE{"true"};
constexpr std::string_view _FALSE{"false"};
constexpr std::string_view _NULL{"false"};
constexpr std::string_view _COMMA_SPACE{", "};
constexpr std::string_view _KEY_VAL_SEP{R"|(": )|"};

class save_restore_end;

template<const std::string_view& t_empty_structure, size_t t_max_levels=15>
class creator_base
{
    
protected:
    creator_base(TStr& the_str, size_t start_level)
    : m_json_str(the_str)
    , m_level(start_level)
    {}
    
    TStr& m_json_str;
    std::string_view  m_whitespace_seperator{"\n"};
    size_t m_num_subs{0};
    size_t m_level{0};
    static inline std::string_view m_empty_structure{t_empty_structure};  // "{}" or "[]"
    
    std::string_view save_end();
    void restore_end(std::string_view in_end);
    
    void prepare_for_additional_value();
    
    class save_restore_end
    {
    public:
        save_restore_end(creator_base& to_save);
        ~save_restore_end();

    private:
        creator_base&   m_to_save;
        std::string_view m_saved_end;
        char m_save_buf[t_max_levels+1];
    };

public:
    size_t size() { return m_json_str.size(); }
    size_t capacity() { return m_json_str.capacity(); }
    size_t max_size() { return m_json_str.max_size(); }
    const char* c_str() const { return m_json_str.c_str(); }
    operator std::string_view() { return m_json_str;}
    
    bool empty() const { return m_json_str[1] == m_empty_structure[1]; }
    
    friend class save_restore_end;
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
    
    void prepare_for_additional_value(const std::string_view in_key);
    
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
    
    object_creator append_object(std::string_view in_key)  ;
    array_creator append_array(std::string_view in_key);
    
    // add a value that is already formated as json
    void push_json_str(const std::string_view in_key, const std::string_view in_value);

    template <typename TValue>
    void append(const std::string_view in_key, const TValue&& in_value);
    
    void append(const std::string_view in_key, const char* in_value)
    {
        append<std::string_view>(in_key, std::string_view(in_value));
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
    
    array_creator append_array();
    object_creator append_object();
    
    // add a value that is already formated as json
    void push_json_str(const std::string_view in_value);
    
    void append() {}
    void append(const char* in_value);
    template<typename TValue> void append(const TValue& in_value);

    template<typename TValue, typename... Args>
    void append(const TValue& in_value, Args&&... args)
    {
        append(in_value);
        append(std::forward<Args>(args)...);
    }

    template<typename TContainer>
    void extend(const TContainer& in_values_container)
    {
        for (const auto& value : in_values_container)
        {
            append(value);
        }
    }

protected:
};



} // namespace jsonland

#endif // __json_creator_h__
