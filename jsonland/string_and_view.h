#ifndef __string_and_view_h__
#define __string_and_view_h__

#include <string>
#include <string_view>
using namespace std::string_view_literals;

#include "escape.h"
///
/// @class string_and_view
/// @brief A utility class that manages a `std::string` and its associated `std::string_view`.
///
///
/// This class provides mechanisms to manage a string and its view efficiently. It ensures that
/// the `std::string_view` is always synchronized with the `std::string`. Additionally, it supports
/// move and copy semantics while maintaining ownership information.
///
/// Ownership in string_and_view
///
/// By default, `string_and_view` "owns" the string. This means a copy of the string is stored in
/// an internal `std::string` member (called `m_string`). Consequently, constructing or assigning
/// a `string_and_view` typically involves memory allocation and a copy of the string data.
///
/// In cases where a string is already allocated, and its lifetime exceeds that of the `string_and_view`
/// instance, the allocation and copying can be avoided by calling `set_without_ownership`. This sets the
/// `string_and_view` instance to a non-owning state. In this state, the string is referenced via an
/// internal `std::string_view` (`m_view`), and the `std::string` member (`m_string`) remains unused,
/// thereby eliminating unnecessary memory allocation and copying.
///
/// Regardless of whether `string_and_view` is owning or non-owning, access is always read-only
/// and performed through `operator std::string_view()`. This ensures a consistent interface
/// and behavior, making it indistinguishable to the user whether the instance owns the string
/// or merely references it.
///
namespace jsonland
{
class string_and_view
{
public:

    /// @brief Default constructor.
    string_and_view() = default;

    /// @brief Constructor initializing with a `std::string_view`.
    /// @param in_str_view The string view to initialize the object with.
    /// the int parameter is there just to signal to not own the string
    /// Ownership: non-owning, no allocation or copy.
    explicit string_and_view(std::string_view in_str_view, int) noexcept
    : m_view(in_str_view)
    {}

    /// @brief Constructor initializing with a `std::string_view`.
    /// @param in_str_view The string view to initialize the object with.
    /// Ownership: owning, resulting in allocation and copy.
    explicit string_and_view(std::string_view in_str_view) noexcept
    : m_string(in_str_view)
    , m_view(m_string)
    {}

    explicit string_and_view(const char* in_str) noexcept
    : string_and_view(std::string_view(in_str))
    {}

    explicit string_and_view(const std::string& in_str) noexcept
    : string_and_view(std::string_view(in_str))
    {}

    explicit string_and_view(std::string&& in_str) noexcept
    : m_string(std::move(in_str))
    , m_view(m_string)
    {}

    /// @brief Assignment operator from a `std::string_view`.
    /// @param in_str_view The string view to assign.
    /// @return Reference to the updated object.
    /// Ownership: owning, resulting in allocation and memory copy.
    string_and_view& operator=(std::string_view in_str_view) noexcept
    {
        m_string = in_str_view;
        m_view = m_string;
        return *this;
    }
    string_and_view& operator=(const char* in_str) noexcept
    {
        operator=(std::string_view(in_str));
        return *this;
    }
    string_and_view& operator=(const std::string& in_str) noexcept
    {
        operator=(std::string_view(in_str));
        return *this;
    }
    string_and_view& operator=(std::string&& in_str) noexcept
    {
        m_string = std::move(in_str);
        m_view = m_string;
        return *this;
    }

    /// @brief Copy constructor.
    /// @param in_sav The source `string_and_view` to copy.
    /// Ownership: owning, resulting in allocation and memory copy.
    string_and_view(const string_and_view& in_sav) noexcept
    : string_and_view(in_sav.sv())
    {}


    /// @brief Move constructor.
    /// @param in_sav The source `string_and_view` to move from.
    /// Ownership: same as the copied string_and_view.
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

    /// @brief Copy assignment operator.
    /// @param in_sav The source `string_and_view` to copy.
    /// @return Reference to the updated object.
    /// Ownership: owning, resulting in allocation and memory copy.
    string_and_view& operator=(const string_and_view& in_sav) noexcept
    {
        if (this != &in_sav)
        {
            operator=(in_sav.sv());
        }
        return *this;
    }

    /// @brief Move assignment operator.
    /// @param in_sav The source `string_and_view` to move from.
    /// @return Reference to the updated object.
    /// Ownership: same as the copied string_and_view.
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

    /// @brief Implicit conversion to `std::string_view`.
    /// @return The current string view.
    operator std::string_view() const noexcept {return m_view;}

    /// @brief Accessor for the current string view.
    /// @return The current string view.
    std::string_view sv() const  noexcept {return m_view;}

    /// @brief Checks if the object owns the string data.
    /// @return `true` if the object owns the string, `false` otherwise.
    bool is_owner() const  noexcept
    {
        bool retVal = (m_string.empty() && m_view.empty())
                        || m_string.data() == m_view.data();
        return retVal;
    }

    void take_ownership() noexcept
    {
        if (!is_owner())
        {
            m_string = m_view;
            m_view = m_string;
        }
    }

    /// @brief Sets the object to only manage a string view.
    /// @param in_str_view The string view to manage.
    /// Ownership: non-owning, lifetime of the original string must acceed that this instance.
    void set_without_ownership(std::string_view in_str_view) noexcept // know what you are doing...!
    {
        m_string.clear();
        m_view = in_str_view;
    }

    size_t size() const noexcept
    {
        return m_view.size();
    }

    bool empty() const noexcept
    {
        return m_view.empty();
    }

    /// @brief Clears the managed string and string view.
    void clear() noexcept
    {
        m_string.clear();
        m_view = m_string;
    }

    size_t allocation_size() const noexcept
    {
        return m_string.size();
    }

    /// unescape escaped characters and update internal m_string if needed.
    /// if there were not escaped character internal members are not changed,
    /// saving allocation and copying
    void unescape_json_string_internal() noexcept
    {
        std::string unescaped;
        escapism::unescape_result uer = escapism::unescape_json_string(m_view, unescaped);
        if (uer == escapism::something_was_unescaped)
        {
            m_string = std::move(unescaped);
            m_view = m_string;
        }
        else if (escapism::nothing_to_unescape)
        {
            // m_view remained unchanged
        }
    }
    /// unescape escaped characters copy to an internal string.
    void unescape_json_string_external(std::string& out_str) const noexcept
    {
        escapism::unescape_result uer = escapism::unescape_json_string(m_view, out_str);
        if (escapism::nothing_to_unescape == uer)
        {
            out_str += m_view;
        }
        else if (escapism::something_was_unescaped)
        {
            // out_str was already updated
        }
    }
//...

    /// if there were not escaped character internal members are not changed,
    /// saving allocation and copying
    void escape_json_string_internal() noexcept
    {
        std::string escaped;
        escapism::escape_result uer = escapism::escape_json_string(m_view, escaped);
        if (uer == escapism::something_was_escaped)
        {
            m_string = std::move(escaped);
            m_view = m_string;
        }
        else if (escapism::something_was_escaped)
        {
            // m_view remained unchanged
        }
    }
    /// escape escapable characters copy to an internal string.
    void escape_json_string_external(std::string& out_str) const noexcept
    {
        escapism::escape_result uer = escapism::escape_json_string(m_view, out_str);
        if (escapism::nothing_to_escape == uer)
        {
            out_str += m_view;
        }
        else if (escapism::something_was_escaped)
        {
            // out_str was already updated
        }
    }

    bool operator==(std::string_view other) const noexcept
    {
        return sv() == other;
    }

private:
    std::string m_string;       ///< The managed string.
    std::string_view m_view;    ///< The associated string view.
};


///
/// @brief Three-way comparison operator for `string_and_view`.
/// @param lhs The left-hand side operand.
/// @param rhs The right-hand side operand.
/// @return Strong ordering result.
////
inline std::strong_ordering operator<=>(const jsonland::string_and_view& lhs, const jsonland::string_and_view& rhs) noexcept
{
    // Apple clang does not have operator<=> for std::string_view, so implementing with  std::string_view::compare
    int result = lhs.sv().compare(rhs.sv());
    return (result < 0) ? std::strong_ordering::less
         : (result > 0) ? std::strong_ordering::greater
                        : std::strong_ordering::equal;
}


///
/// @brief Equality comparison operator for `string_and_view`.
/// @param lhs The left-hand side operand.
/// @param rhs The right-hand side operand.
/// @return `true` if the two objects are equal, `false` otherwise.
////
inline bool operator==(const jsonland::string_and_view& lhs, const jsonland::string_and_view& rhs) noexcept
{
    return lhs.sv() == rhs.sv();
}

} // namespace jsonland

namespace std
{
    /// @struct hash
    /// @brief Specialization of `std::hash` for `string_and_view`.
    /// @example:     std::unordered_map<string_and_view, string_and_view> uom;
    template <>
    struct hash<jsonland::string_and_view>
    {
        /// @brief Hash function for `string_and_view`.
        /// @param sav The `string_and_view` instance to hash.
        /// @return The hash value.
        std::size_t operator()(const jsonland::string_and_view& sav) const noexcept
        {
            return std::hash<std::string_view>{}(sav.sv());
        }
    };
}

#endif // __string_and_view_h__
