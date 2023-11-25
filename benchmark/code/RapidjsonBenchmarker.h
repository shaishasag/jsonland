#ifndef __RapidjsonBenchmarker_h__
#define __RapidjsonBenchmarker_h__

#include "JsonBenchmarker.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

class RapidjsonBenchmarker : public JsonBenchmarker
{
public:
    RapidjsonBenchmarker()
    {
        parser_name = "Rapidjson";
    }
    
    void parse_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        results.file_path = in_path.string();
        
        std::ifstream ifs(in_path);
        contents = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        results.file_size = contents.size();
        
        auto before = std::chrono::steady_clock::now();
        ParseResult pr = document.ParseInsitu(contents.data());
        auto after = std::chrono::steady_clock::now();
        results.file_parse_duration_milli = after - before;
        
        if (pr)
        {
        }
        else
        {
            results.error = pr.Code();
        }
    }
    
    void recursive_copy(Benchmark_results& results) override
    {
        auto before = std::chrono::steady_clock::now();
        Value copy = recursive_copy_self(document);
        auto after = std::chrono::steady_clock::now();
        results.resusive_copy_duration_milli = after - before;
    }
    
    Value recursive_copy_self(const Value& jdoc)
    {
        Value jNodeOut;
        if (jdoc.IsNull())
        {
            jNodeOut.SetNull();
        }
        else if (jdoc.IsBool())
        {
            jNodeOut.SetBool(jdoc.GetBool());
        }
        else if (jdoc.IsNumber())
        {
            jNodeOut.SetFloat(jdoc.GetDouble());
        }
        else if (jdoc.IsString())
        {
            std::string_view the_string = jdoc.GetString();
            jNodeOut.SetString(the_string.data(), the_string.size());
        }
        else if (jdoc.IsArray())
        {
            jNodeOut.SetArray();
            for (SizeType i = 0; i < jdoc.Size(); i++)
            {
                Value outItem = recursive_copy_self(jdoc[i]);
                jNodeOut.PushBack(outItem, document_copy.GetAllocator());
            }
        }
        else if (jdoc.IsObject())
        {
            jNodeOut.SetObject();

            for (rapidjson::Value::ConstMemberIterator itr = jdoc.MemberBegin(); itr != jdoc.MemberEnd(); ++itr)
            {
                Value outItem = recursive_copy_self(itr->value);
                std::string the_key = itr->name.GetString();
                //jNodeOut[the_key.c_str()] = outItem;
                Value key;
                key.SetString(the_key.c_str(), the_key.size(), document_copy.GetAllocator());
                jNodeOut.AddMember(key, outItem, document_copy.GetAllocator());
            }
        }
        
        return jNodeOut;
    }
    
    void write_copy_to_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        
    }

    rapidjson::Document document;
    rapidjson::Document document_copy;

};

#endif // __RapidjsonBenchmarker_h__
