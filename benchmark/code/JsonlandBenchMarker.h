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
        jdoc_copy = recursive_copy_self(jdoc);
        auto after = std::chrono::steady_clock::now();
        results.resusive_copy_duration_milli = after - before;
    }
    
    json_node recursive_copy_self(const json_node& jdoc)
    {
        json_node jNodeOut;
        if (jdoc.is_null())
        {
            jNodeOut = json_node(null_t);
        }
        else if (jdoc.is_bool())
        {
            jNodeOut = json_node(jdoc.as_bool());
        }
        else if (jdoc.is_num())
        {
            jNodeOut = json_node(jdoc.as_double());
        }
        else if (jdoc.is_string())
        {
            std::string_view the_string = jdoc.as_string();
            jNodeOut = json_node(the_string);
        }
        else if (jdoc.is_array())
        {
            jNodeOut = json_node(array_t);
            for (auto& inItem : jdoc)
            {
                json_node outItem = recursive_copy_self(inItem);
                jNodeOut.push_back(outItem);
            }
        }
        else if (jdoc.is_object())
        {
            jNodeOut = json_node(object_t);

            for (auto& inItem : jdoc)
            {
                json_node outItem = recursive_copy_self(inItem);
                std::string_view the_key = inItem.key();
                jNodeOut[the_key] = outItem;
            }
        }
        
        return jNodeOut;
    }
    
    void write_copy_to_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        auto out_file = in_path;
        std::string new_extension = parser_name;
        new_extension += ".out.json";
        out_file.replace_extension(new_extension);
        {
            std::ostringstream ofs;
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
