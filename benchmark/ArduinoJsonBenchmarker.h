#ifndef __ArduinoJsonBenchmarker_h__
#define __ArduinoJsonBenchmarker_h__

#include "JsonBenchmarker.h"
#include <ArduinoJson.h>

using namespace ArduinoJson;

class ArduinoJsonBenchmarker : public JsonBenchmarker
{
public:
    ArduinoJsonBenchmarker()
    : document{1000}
    , document_copy{1000}
    {
        parser_name = "AdruinoJson";
    }
    
    void parse_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        results.file_path = in_path.string();
        
        std::ifstream ifs(in_path);
        contents = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        results.file_size = contents.size();
        
        DynamicJsonDocument doc(results.file_size*3);
       
        auto before = std::chrono::steady_clock::now();
        DeserializationError error = deserializeJson(doc, contents);
        auto after = std::chrono::steady_clock::now();
        results.file_parse_duration_milli = after - before;
        
        if (error)
        {
            results.error = error.code();
        }
        else
        {
            document = std::move(doc);
        }
    }
    
    void recursive_copy(Benchmark_results& results) override
    {
        document_copy = document;
        document.clear(); // make sure no allocation from document were used by document_copy
    }
    
    void write_copy_to_string(Benchmark_results& results,
                              std::string& out_str) override
    {
        serializeJson(document_copy, out_str);
    }

    DynamicJsonDocument document;
    DynamicJsonDocument document_copy;

};

#endif // __ArduinoJsonBenchmarker_h__
