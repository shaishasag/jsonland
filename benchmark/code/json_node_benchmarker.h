#ifndef __json_node_benchmarker_h__
#define __json_node_benchmarker_h__

#include <sstream>

#include "JsonBenchmarker.h"
#include "jsonland/json_node.h"
#include "json_creator.h"

using namespace jsonland;

class json_node_benchmarker : public JsonBenchmarker
{
public:
    json_node_benchmarker()
    {
        parser_name = "jsonland";
    }
    
    void parse_text(Benchmark_results& results) override
    {
        ParseResult parsimony = jdoc.parse_insitu(results.contents);
        results.error = parsimony.error_code();
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
    void recursive_copy_self(const json_node& jNodeIn,
                             json_node& jNodeOut)
    {
        if (jNodeIn.is_string())
        {
            ++is_string_count;
            std::string the_string(jNodeIn.get_string());
            jNodeOut = the_string;
        }
        else if (jNodeIn.is_bool())
        {
            ++is_bool_count;
            const bool b = jNodeIn.get_bool();
            jNodeOut = b;
        }
        else if (jNodeIn.is_float())
        {
            ++is_float_count;
            const double d = jNodeIn.get_float<double>();
            jNodeOut = d;
        }
        else if (jNodeIn.is_int())
        {
            ++is_int_count;
            const int64_t i = jNodeIn.get_int<int64_t>();
            jNodeOut = i;
        }
        else if (jNodeIn.is_object())
        {
            ++is_object_count;
            jNodeOut = object_t;

            for (auto& inItem : jNodeIn)
            {
                std::string_view the_key = inItem.key();
                if (the_key == "an object")
                {
                    std::cout << "an object? " << the_key << std::endl;
                }
                json_node& objItem = jNodeOut[the_key];
                recursive_copy_self(inItem, objItem);
                if (objItem.key().empty())
                {
                    std::cout << "why empty? " << inItem.key() << std::endl;
                }
            }
        }
        else if (jNodeIn.is_array())
        {
            ++is_array_count;
            jNodeOut = array_t;
            for (auto& inItem : jNodeIn)
            {
                json_node arrayItem;
                recursive_copy_self(inItem, arrayItem);
                jNodeOut.push_back(std::move(arrayItem));
            }
        }
        else if (jNodeIn.is_null())
        {
            ++is_null_count;
            jNodeOut = null_t;
        }
    }
    
    
    void acumulate_object(const jsonland::json_node& in_obj_node,
                          jl_fixed::sub_object_json_creator_t& out_acum) const noexcept
    {
        for (const auto& key_val : in_obj_node)
        {
            if (key_val.is_object())
            {
                auto sub_obj = out_acum.append_object(key_val.key());
                acumulate_object(key_val, sub_obj);
            }
            else if (key_val.is_array())
            {
                auto sub_array = out_acum.append_array(key_val.key());
                acumulate_array(key_val, sub_array);
            }
            else if (key_val.is_int())
            {
                out_acum.append_value(key_val.key(), key_val.get_int<int64_t>());
            }
            else if (key_val.is_float())
            {
                out_acum.append_value(key_val.key(), key_val.get_float<double>());
            }
            else if (key_val.is_string())
            {
                out_acum.append_value(key_val.key(), key_val.get_string());
            }
            else if (key_val.is_bool())
            {
                out_acum.append_value(key_val.key(), key_val.get_bool());
            }
            else if (key_val.is_null())
            {
                out_acum.append_value(key_val.key(), key_val.get_null());
            }
        }
    }
    
    void acumulate_array(const jsonland::json_node& in_array_node,
                         jl_fixed::sub_array_json_creator_t& out_acum) const noexcept
    {
        for (auto& val : in_array_node)
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
                out_acum.append_value(val.get_int<int64_t>());
            }
            else if (val.is_float())
            {
                out_acum.append_value(val.get_float<double>());
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
    
    void acumulate_doc(const jsonland::json_node& in_doc,
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
        jdoc_copy.dump(out_str, jsonland::dump_style::pretty);
    }

    jsonland::json_doc jdoc;
    jsonland::json_node jdoc_copy;

};
#endif // __json_node_benchmarker_h__
