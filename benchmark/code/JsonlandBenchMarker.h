#ifndef __JsonlandBenchMarker_h__
#define __JsonlandBenchMarker_h__

#include <sstream>

#include "JsonBenchmarker.h"
#include "jsonland/json_node.h"

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
            std::string the_string(jdoc.as_string());
            jNodeOut = the_string;
        }
        else if (jdoc.is_bool())
        {
            ++is_bool_count;
            const bool b = jdoc.as_bool();
            jNodeOut = b;
        }
        else if (jdoc.is_float())
        {
            ++is_float_count;
            const double d = jdoc.as_double();
            jNodeOut = d;
        }
        else if (jdoc.is_int())
        {
            ++is_int_count;
            const int64_t i = jdoc.as_int<int64_t>();
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
    
    void write_copy_to_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        auto out_file = in_path;
        std::string new_extension = parser_name;
        new_extension += ".out.json";
        out_file.replace_extension(new_extension);
        {
            std::ostringstream ofs;
            ofs.setf(std::ios::fixed, std::ios::floatfield);
            auto before = std::chrono::steady_clock::now();
            jdoc_copy.dump(ofs);
            auto after = std::chrono::steady_clock::now();
            results.write_copy_to_file_duration_milli = after - before;
            
            std::ofstream ffs(out_file);
            const std::string& s = ofs.str();
            ffs.write(s.c_str(), s.size());
        }
        

    }

    
    jsonland::json_doc jdoc;
    jsonland::json_node jdoc_copy;

};
#endif // __JsonlandBenchMarker_h__
