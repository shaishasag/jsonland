#ifndef __JsOnBenchMarker_h__
#define __JsOnBenchMarker_h__

#include <sstream>

#include "JsonBenchmarker.h"
#include "jsonland/JsOn.h"

using namespace jsonland;

class JsOn_benchmarker : public JsonBenchmarker
{
public:
    JsOn_benchmarker()
    {
        parser_name = "JsOn";
    }
    
    void parse_text(Benchmark_results& results) override
    {
        results.error = jdoc.parse_inplace(results.contents);
    }
    
    void recursive_copy(Benchmark_results&) override
    {
        recursive_copy_self(jdoc, jdoc_copy);
        jdoc.clear(); // make sure no allocation from jdoc were used by jdoc_copy
    }
    
    size_t is_null_count{0};
    size_t is_bool_count{0};
    size_t is_float_count{0};
    size_t is_int_count{0};
    size_t is_string_count{0};
    size_t is_array_count{0};
    size_t is_object_count{0};
    void recursive_copy_self(const JsOn& jdocIn,
                             JsOn& jNodeOut)
    {
        if (jdocIn.is_type(string_t))
        {
            ++is_string_count;
            jNodeOut = jdocIn.get_string();
        }
        else if (jdocIn.is_type(bool_t))
        {
            ++is_bool_count;
            const bool b = jdocIn.get_bool();
            jNodeOut = b;
        }
        else if (jdocIn.is_float())
        {
            ++is_float_count;
            const double d = jdocIn.get_double();
            jNodeOut = d;
        }
        else if (jdocIn.is_int())
        {
            ++is_int_count;
            const int64_t i = jdocIn.get_int();
            jNodeOut = i;
        }

        else if (jdocIn.is_object())
        {
            ++is_object_count;
            jNodeOut = JsOn(object_t);

            for (auto& [key, inItem] : jdocIn.object_range())
            {
                JsOn& objItem = jNodeOut[key];
                recursive_copy_self(inItem, objItem);
            }
        }
        else if (jdocIn.is_array())
        {
            ++is_array_count;
            jNodeOut = array_t;
            for (auto& inItem : jdocIn.array_range())
            {
                JsOn& arrayItem = jNodeOut.emplace_back();
                recursive_copy_self(inItem, arrayItem);
            }
        }

        else if (jdocIn.is_type(null_t))
        {
            ++is_null_count;
            jNodeOut = null_t;
        }

    }
    
    
    void acumulate_object(const jsonland::JsOn& in_obj_node,
                          jl_fixed::sub_object_json_creator_t& out_acum) const noexcept
    {
        for (const auto& [key, val] : in_obj_node.object_range())
        {
            if (val.is_object())
            {
                auto sub_obj = out_acum.append_object(key);
                acumulate_object(val, sub_obj);
            }
            else if (val.is_array())
            {
                auto sub_array = out_acum.append_array(key);
                acumulate_array(val, sub_array);
            }
            else if (val.is_int())
            {
                out_acum.append_value(key, val.get_int());
            }
            else if (val.is_float())
            {
                out_acum.append_value(key, val.get_float());
            }
            else if (val.is_string())
            {
                out_acum.append_value(key, val.get_string());
            }
            else if (val.is_bool())
            {
                out_acum.append_value(key, val.get_bool());
            }
            else if (val.is_null())
            {
                out_acum.append_value(key, val.get_null());
            }
        }
    }
    
    void acumulate_array(const jsonland::JsOn& in_array_node,
                         jl_fixed::sub_array_json_creator_t& out_acum) const noexcept
    {

        for (auto& val : in_array_node.array_range())
        {
            if (val.is_object())
            {
                auto sub_obj = out_acum.append_object();
                acumulate_object(val, sub_obj);
            }
            else if (val.is_array())
            {
                auto sub_array = out_acum.append_array();
                acumulate_array(val, sub_array);
            }
            else if (val.is_int())
            {
                out_acum.append_value(val.get_int());
            }
            else if (val.is_float()) // distiguish float/int ??
            {
                out_acum.append_value(val.get_float());
            }
            else if (val.is_string())
            {
                out_acum.append_value(val.get_string());
            }
            else if (val.is_bool())
            {
                out_acum.append_value(val.get_bool());
            }
            else if (val.is_null())
            {
                out_acum.append_value(val.get_null());
            }
        }
    }
    
    void acumulate_doc(const jsonland::JsOn& in_doc,
                       std::string& out_str) const noexcept
    {

        if (in_doc.is_object())
        {
            jl_fixed::object_json_creator<100'000> obj_acum;
            acumulate_object(in_doc, obj_acum);
            out_str = obj_acum;
        }
        else if (in_doc.is_array())
        {
            jl_fixed::array_json_creator<100'000> array_acum;
            acumulate_array(in_doc, array_acum);
            out_str = array_acum;
        }

    }

    void write_copy_to_string(Benchmark_results&,
                            std::string& out_str) override
    {
        jdoc_copy.dump(out_str);
    }

    jsonland::JsOn jdoc;
    jsonland::JsOn jdoc_copy;

};
#endif // __JsOnBenchMarker_h__
