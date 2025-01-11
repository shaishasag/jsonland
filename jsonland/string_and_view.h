#ifndef __string_and_view_h__
#define __string_and_view_h__

#include <string>
#include <string_view>

class string_and_view
{
public:
    string_and_view() = default;

    string_and_view(std::string_view in_str_view) noexcept
    : m_string(in_str_view)
    , m_view(m_string)
    {}

    string_and_view& operator=(std::string_view in_str_view) noexcept
    {
        m_string = in_str_view;
        m_view = m_string;
        return *this;
    }

    string_and_view(const string_and_view& in_sav) noexcept
    : string_and_view(in_sav.sv())
    {}

    string_and_view(string_and_view&& in_sav) noexcept
    {
        if (in_sav.is_owner())
        {
            m_string = std::move(in_sav.m_string);
            m_view = m_string;
        }
        else
        {
            m_view = in_sav.m_view;
        }
        in_sav.clear(); // not strictly needed, but useful for tests
    }

    string_and_view& operator=(const string_and_view& in_sav) noexcept
    {
        if (this != &in_sav)
        {
            operator=(in_sav.sv());
        }
        return *this;
    }

    string_and_view& operator=(string_and_view&& in_sav) noexcept
    {
        if (this != &in_sav)
        {
            if (in_sav.is_owner())
            {
                m_string = std::move(in_sav.m_string);
                m_view = m_string;
            }
            else
            {
                m_view = in_sav.m_view;
            }
            in_sav.clear(); // not strictly needed, but useful for tests
       }
        return *this;
    }


    operator std::string_view() const  noexcept {return m_view;}
    std::string_view sv() const  noexcept {return m_view;}

    bool is_owner() const  noexcept
    {
        bool retVal = (m_string.empty() && m_view.empty())
                        || m_string.data() == m_view.data();
        return retVal;
    }

    void set_string_view_only(std::string_view in_str_view) noexcept // know what you are doing...!
    {
        m_string.clear();
        m_view = in_str_view;
    }

    void clear() noexcept
    {
        m_string.clear();
        m_view = m_string;
    }

private:
    std::string m_string;
    std::string_view m_view;
};

std::strong_ordering operator<=>(const string_and_view& lhs, const string_and_view& rhs) noexcept
{
    // Apple clang does not have operator<=> for std::string_view, so implementing with  std::string_view::compare
    int result = lhs.sv().compare(rhs.sv());
    return (result < 0) ? std::strong_ordering::less
         : (result > 0) ? std::strong_ordering::greater
                        : std::strong_ordering::equal;
}

bool operator==(const string_and_view& lhs, const string_and_view& rhs) noexcept
{
    return lhs.sv() == rhs.sv();
}

// for using string_and_view as a key in unordered_map, unordered_set
namespace std
{
    template <>
    struct hash<string_and_view>
    {
        std::size_t operator()(const string_and_view& sav) const noexcept
        {
            return std::hash<std::string_view>{}(sav.sv());
        }
    };
}


#endif // __string_and_view_h__
