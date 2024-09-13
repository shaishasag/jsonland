#include "jsonland/json_creator.h"


namespace jsonland
{
namespace internal
{
template<typename TStr>
base_json_creator<TStr>::save_restore_end::save_restore_end(base_json_creator<TStr>& to_save) noexcept
: m_creator_to_save(to_save)
{
    std::string_view end_to_save = m_creator_to_save.save_end();
    std::memcpy(m_save_buf,
                end_to_save.data(),
                end_to_save.size());
    to_save.m_json_str.resize(to_save.m_json_str.size()-end_to_save.size());
    m_saved_end_chars = std::string_view(m_save_buf, end_to_save.size());
}

template<typename TStr>
base_json_creator<TStr>::save_restore_end::~save_restore_end()
{
    m_creator_to_save.m_json_str += m_saved_end_chars;
}

template<typename TStr>
std::string_view base_json_creator<TStr>::save_end()
{
    return std::string_view(this->m_json_str.data()+this->m_json_str.size()-m_level-1, m_level+1);
}

template<typename TStr>
void base_json_creator<TStr>::restore_end(std::string_view in_end)
{
    this->m_json_str += in_end;
}

template<typename TStr>
void sub_object_json_creator<TStr>::prepare_for_additional_value(const std::string_view in_key)
{
    if (0 < this->m_num_subs) {  // not first, need to add ','
        this->m_json_str += internal::_COMMA;
        this->m_json_str += ___object_whitespace_after_comma;
    }
    ++this->m_num_subs;

    this->m_json_str += '"';
    internal::copy_and_escape(in_key, this->m_json_str);
    this->m_json_str += internal::_KEY_VAL_SEP;
}

template<typename TStr>
sub_object_json_creator<TStr> sub_object_json_creator<TStr>::append_object(std::string_view in_key)
{
    typename base_json_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value(in_key);
    sub_object_json_creator<TStr> retVal(this->m_json_str,
                                         (this->m_level)+1);

    return retVal;
}

template<typename TStr>
sub_array_json_creator<TStr> sub_object_json_creator<TStr>::append_array(std::string_view in_key)
{
    typename base_json_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value(in_key);
    sub_array_json_creator<TStr> retVal(this->m_json_str,
                                        (this->m_level)+1);

    return retVal;
}

// add a value that is already formated as json
template<typename TStr>
void sub_object_json_creator<TStr>::append_json_str(const std::string_view in_key, const std::string_view in_value)
{
    typename base_json_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value(in_key);
    this->m_json_str += in_value;
}

// add a value that is already formated as json to the begening of the object
// Warning: less efficient than append_json_str!
template<typename TStr>
void sub_object_json_creator<TStr>::prepend_json_str(const std::string_view in_key, const std::string_view in_value)
{
    size_t num_additional_chars{0};
    num_additional_chars += 1;  // '"'
    num_additional_chars += in_key.size();
    num_additional_chars += internal::_KEY_VAL_SEP.size();
    num_additional_chars += in_value.size();
    if (0 < this->m_num_subs) {  // not first, need to add ','
        num_additional_chars += internal::_COMMA.size();
        num_additional_chars += ___object_whitespace_after_comma.size();
    }
    this->m_json_str.insert(1, num_additional_chars, '*');

    size_t replacement_location{1};
    this->m_json_str[replacement_location] = '"';
    ++replacement_location;
    this->m_json_str.replace(replacement_location,
                             in_key.size(),
                             in_key);
    replacement_location += in_key.size();

    this->m_json_str.replace(replacement_location,
                             internal::_KEY_VAL_SEP.size(),
                             internal::_KEY_VAL_SEP);
    replacement_location += internal::_KEY_VAL_SEP.size();

    this->m_json_str.replace(replacement_location,
                             in_value.size(),
                             in_value);
    replacement_location += in_value.size();

    if (0 < this->m_num_subs) {  // not first, need to add ','
        this->m_json_str.replace(replacement_location,
                                 internal::_COMMA.size(),
                                 internal::_COMMA);
        replacement_location += internal::_COMMA.size();
        this->m_json_str.replace(replacement_location,
                                 ___object_whitespace_after_comma.size(),
                                 ___object_whitespace_after_comma);
    }
    ++this->m_num_subs;
}

template<typename TStr>
void sub_object_json_creator<TStr>::append_values_from(const sub_object_json_creator& in_to_merge_from)
{
    std::string_view values_to_merge(in_to_merge_from.m_json_str.data()+1,
                                     in_to_merge_from.m_json_str.size()-2);
    if (! values_to_merge.empty())
    {
        typename base_json_creator<TStr>::save_restore_end sv(*this);
        if (0 < this->m_num_subs) {  // not first, need to add ','
            this->m_json_str += internal::_COMMA;
            this->m_json_str += ___object_whitespace_after_comma;
        }
        this->m_json_str.insert(this->m_json_str.size(), values_to_merge);
        ++this->m_num_subs;
    }
}

template<typename TStr>
void sub_object_json_creator<TStr>::prepend_values_from(const sub_object_json_creator& in_to_merge_from)
{
    std::string_view values_to_merge(in_to_merge_from.m_json_str.data()+1,
                                     in_to_merge_from.m_json_str.size()-2);
    if (! values_to_merge.empty())
    {
        size_t num_additional_chars{values_to_merge.size()};
        if (0 < this->m_num_subs) {  // not first, need to add ','
            num_additional_chars += internal::_COMMA.size();
            num_additional_chars += ___object_whitespace_after_comma.size();
        }
        this->m_json_str.insert(1, num_additional_chars, '*');

        size_t replacement_location{1};
        this->m_json_str.replace(replacement_location,
                                 values_to_merge.size(),
                                 values_to_merge);
        replacement_location += values_to_merge.size();
        if (0 < this->m_num_subs) {
            this->m_json_str.replace(replacement_location,
                                     internal::_COMMA.size(),
                                     internal::_COMMA);
            replacement_location += internal::_COMMA.size();
            this->m_json_str.replace(replacement_location,
                                     ___object_whitespace_after_comma.size(),
                                     ___object_whitespace_after_comma);
        }
        ++this->m_num_subs;
    }
}

template<typename TStr>
void sub_array_json_creator<TStr>::prepare_for_additional_value()
{
    if (0 < this->m_num_subs) {  // not first, need to add ','
        this->m_json_str += internal::_COMMA;
        this->m_json_str += ___array_whitespace_after_comma;
    }
    ++this->m_num_subs;
}

template<typename TStr>
sub_array_json_creator<TStr> sub_array_json_creator<TStr>::append_array()
{
    typename base_json_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value();
    sub_array_json_creator retVal(this->m_json_str,
                                  base_json_creator<TStr>::m_level+1);

    return retVal;
}

template<typename TStr>
sub_object_json_creator<TStr> sub_array_json_creator<TStr>::append_object()
{
    typename base_json_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value();
    sub_object_json_creator<TStr> retVal(this->m_json_str,
                                         (this->m_level)+1);

    return retVal;
}

// add a value that is already formated as json to the end of the array
template<typename TStr>
void sub_array_json_creator<TStr>::append_json_str(const std::string_view in_value)
{
    typename sub_array_json_creator<TStr>::save_restore_end sv(*this);
    prepare_for_additional_value();
    this->m_json_str += in_value;
}

// add a value that is already formated as json to the beginning of the array
template<typename TStr>
void sub_array_json_creator<TStr>::prepend_json_str(const std::string_view in_value)
{
    size_t num_additional_chars{in_value.size()};
    if (0 < this->m_num_subs) {  // not first, need to add ','
        num_additional_chars += internal::_COMMA.size();
        num_additional_chars += ___array_whitespace_after_comma.size();
    }
    this->m_json_str.insert(1, num_additional_chars, '*');

    size_t replacement_location{1};

    this->m_json_str.replace(replacement_location,
                             in_value.size(),
                             in_value);
    replacement_location += in_value.size();

    if (0 < this->m_num_subs) {  // not first, need to add ','
        this->m_json_str.replace(replacement_location,
                                 internal::_COMMA.size(),
                                 internal::_COMMA);
        replacement_location += internal::_COMMA.size();
        this->m_json_str.replace(replacement_location,
                                 ___array_whitespace_after_comma.size(),
                                 ___array_whitespace_after_comma);
    }
    ++this->m_num_subs;
}

template<typename TStr>
void sub_array_json_creator<TStr>::append_value(const char* in_value)
{
    typename sub_array_json_creator::save_restore_end sv(*this);
    prepare_for_additional_value();
    internal::write_value(in_value, this->m_json_str);
}

template<typename TStr>
void sub_array_json_creator<TStr>::append_values_from(const sub_array_json_creator& in_to_merge_from)
{
    std::string_view values_to_merge(in_to_merge_from.m_json_str.data()+1,
                                     in_to_merge_from.m_json_str.size()-2);
    if (! values_to_merge.empty())
    {
        typename base_json_creator<TStr>::save_restore_end sv(*this);
        if (0 < this->m_num_subs) {  // not first, need to add ','
            this->m_json_str += internal::_COMMA;
            this->m_json_str += ___array_whitespace_after_comma;
        }
        this->m_json_str.insert(this->m_json_str.size(), values_to_merge);
        ++this->m_num_subs;
    }
}

template<typename TStr>
void sub_array_json_creator<TStr>::prepend_values_from(const sub_array_json_creator& in_to_merge_from)
{
    std::string_view values_to_merge(in_to_merge_from.m_json_str.data()+1,
                                     in_to_merge_from.m_json_str.size()-2);
    if (! values_to_merge.empty())
    {
        size_t num_additional_chars{values_to_merge.size()};
        if (0 < this->m_num_subs) {  // not first, need to add ','
            num_additional_chars += internal::_COMMA.size();
            num_additional_chars += ___array_whitespace_after_comma.size();
        }
        this->m_json_str.insert(1, num_additional_chars, '*');

        size_t replacement_location{1};
        this->m_json_str.replace(replacement_location,
                                 values_to_merge.size(),
                                 values_to_merge);
        replacement_location += values_to_merge.size();
        if (0 < this->m_num_subs) {
            this->m_json_str.replace(replacement_location,
                                     internal::_COMMA.size(),
                                     internal::_COMMA);
            replacement_location += internal::_COMMA.size();
            this->m_json_str.replace(replacement_location,
                                     ___array_whitespace_after_comma.size(),
                                     ___array_whitespace_after_comma);
        }
        ++this->m_num_subs;
    }
}
} //namespace internal

}


template class DllExport jsonland::internal::base_json_creator<fstr::fstr_ref>;
template class DllExport jsonland::internal::sub_object_json_creator<fstr::fstr_ref>;
template class DllExport jsonland::internal::sub_array_json_creator<fstr::fstr_ref>;

template class DllExport jsonland::internal::base_json_creator<std::string&>;
template class DllExport jsonland::internal::sub_object_json_creator<std::string&>;
template class DllExport jsonland::internal::sub_array_json_creator<std::string&>;
