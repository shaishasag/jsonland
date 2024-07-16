#ifndef __JsonlandBenchMarker_h__
#define __JsonlandBenchMarker_h__

#include <sstream>

#include "JsonBenchmarker.h"
#include "jsonland/json_node.h"
#include "json_creator.h"

using namespace jsonland;

class JsonlandBenchMarker : public JsonBenchmarker
{
public:
    JsonlandBenchMarker()
    {
        parser_name = "jsonland";
    }
    
    void parse_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        results.file_path = in_path.string();
        
        std::ifstream ifs(in_path);
        contents = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        results.file_size = contents.size();
        
        auto before = std::chrono::steady_clock::now();
        results.error = jdoc.parse_insitu(contents);
        auto after = std::chrono::steady_clock::now();
        results.file_parse_duration_milli = after - before;
    }
    
    void recursive_copy(Benchmark_results& results) override
    {
        auto before = std::chrono::steady_clock::now();
        recursive_copy_self(jdoc, jdoc_copy);
        auto after = std::chrono::steady_clock::now();
        results.resusive_copy_duration_milli = after - before;
        for (char& c : contents) { // and no references to contents remain
            c = '|';
        }
        contents.clear();
        jdoc.clear(); // make sure no allocation from jdoc were used by jdoc_copy
//        std::cout << "is_null_count: " << is_null_count << "\n";
//        std::cout << "is_bool_count: " << is_bool_count << "\n";
//        std::cout << "is_float_count: " << is_float_count << "\n";
//        std::cout << "is_int_count: " << is_int_count << "\n";
//        std::cout << "is_string_count: " << is_string_count << "\n";
//        std::cout << "is_array_count: " << is_array_count << "\n";
//        std::cout << "is_object_count: " << is_object_count << "\n";
//        std::cout << "total_count: " << is_object_count+is_array_count+is_string_count+is_int_count+is_float_count+is_bool_count+is_null_count << "\n";
//        std::cout << "---" << "\n";
    }
    
    size_t is_null_count{0};
    size_t is_bool_count{0};
    size_t is_float_count{0};
    size_t is_int_count{0};
    size_t is_string_count{0};
    size_t is_array_count{0};
    size_t is_object_count{0};
    void recursive_copy_self(const json_node& jdoc, json_node& jNodeOut)
    {
        if (jdoc.is_string())
        {
            ++is_string_count;
            std::string the_string(jdoc.get_string());
            jNodeOut = the_string;
        }
        else if (jdoc.is_bool())
        {
            ++is_bool_count;
            const bool b = jdoc.get_bool();
            jNodeOut = b;
        }
        else if (jdoc.is_float())
        {
            ++is_float_count;
            const double d = jdoc.get_float<double>();
            jNodeOut = d;
        }
        else if (jdoc.is_int())
        {
            ++is_int_count;
            const int64_t i = jdoc.get_int<int64_t>();
            jNodeOut = i;
        }
        else if (jdoc.is_object())
        {
            ++is_object_count;
            jNodeOut = json_node(object_t);

            for (auto& inItem : jdoc)
            {
                json_node objItem;
                recursive_copy_self(inItem, objItem);
                std::string_view the_key = inItem.key();
                jNodeOut[the_key] = std::move(objItem);
            }
        }
        else if (jdoc.is_array())
        {
            ++is_array_count;
            jNodeOut = array_t;
            for (auto& inItem : jdoc)
            {
                json_node arrayItem;
                recursive_copy_self(inItem, arrayItem);
                jNodeOut.push_back(std::move(arrayItem));
            }
        }
        else if (jdoc.is_null())
        {
            ++is_null_count;
            jNodeOut = null_t;
        }
    }
    
    
    void acumulate_object(const jsonland::json_node& in_obj_node, fixed::sub_object_json_creator& out_acum) const noexcept
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
    
    void acumulate_array(const jsonland::json_node& in_array_node, fixed::sub_array_json_creator& out_acum) const noexcept
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
    
    void acumulate_doc(const jsonland::json_node& in_doc, std::string& out_str) const noexcept
    {
        if (in_doc.is_object())
        {
            fixed::object_json_creator<100'000> obj_acum;
            acumulate_object(in_doc, obj_acum);
            out_str = obj_acum;
        }
        else if (in_doc.is_array())
        {
            fixed::array_json_creator<100'000> array_acum;
            acumulate_array(in_doc, array_acum);
            out_str = array_acum;
        }
    }

    void write_copy_to_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        auto out_file = in_path;
        auto out_file2 = in_path;
        std::string new_extension = parser_name;
        new_extension += ".out.json";
        out_file.replace_extension(new_extension);
        out_file2.replace_extension("acum.json");
        {
            auto before = std::chrono::steady_clock::now();
            std::string jstr;
            jdoc_copy.dump(jstr);
            auto after = std::chrono::steady_clock::now();
            results.write_to_string_duration_milli = after - before;
            
            std::ofstream ffs(out_file);
            ffs.write(jstr.c_str(), jstr.size());

            before = std::chrono::steady_clock::now();
            std::string acum_str;
            acum_str.reserve(results.file_size*2);
            acumulate_doc(jdoc_copy, acum_str);
            after = std::chrono::steady_clock::now();
            results.write_to_string_duration_milli_2 = after - before;
            
            std::ofstream ffs2(out_file2);
            ffs2.write(acum_str.c_str(), acum_str.size());
        }
    }

    jsonland::json_doc jdoc;
    jsonland::json_node jdoc_copy;

};
#endif // __JsonlandBenchMarker_h__
