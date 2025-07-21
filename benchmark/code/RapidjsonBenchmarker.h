#ifndef __RapidjsonBenchmarker_h__
#define __RapidjsonBenchmarker_h__

#include "JsonBenchmarker.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"

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
    
    void recursive_copy(Benchmark_results&) override
    {
        recursive_copy_self(document, document_copy);
        document.SetNull(); // make sure no allocation from document were used by document_copy
    }
    
    void recursive_copy_self(const Value& from, Value& to)
    {
        if (from.IsString())
        {
            std::string the_string(from.GetString());
            to.SetString(the_string.data(), the_string.size(), document_copy.GetAllocator());
        }
        else if (from.IsBool())
        {
            const bool b = from.GetBool();
            to.SetBool(b);
        }
        else if (from.IsInt())
        {
            const int64_t i = from.GetInt64();
            to.SetInt64(i);
        }
        else if (from.IsNumber())
        {
            const double d = from.GetDouble();
            to.SetFloat(d);
        }
        else if (from.IsObject())
        {
            to.SetObject();

            for (rapidjson::Value::ConstMemberIterator itr = from.MemberBegin(); itr != from.MemberEnd(); ++itr)
            {
                Value objItem;
                recursive_copy_self(itr->value, objItem);
                std::string_view the_key{itr->name.GetString(), itr->name.GetStringLength()};
                //to[the_key.c_str()] = outItem;
                Value key;
                key.SetString(the_key.data(), the_key.size(), document_copy.GetAllocator());
                to.AddMember(key, objItem, document_copy.GetAllocator());
            }
        }
        else if (from.IsArray())
        {
            to.SetArray();
            for (SizeType i = 0; i < from.Size(); i++)
            {
                Value arrayItem;
                recursive_copy_self(from[i], arrayItem);
                to.PushBack(arrayItem, document_copy.GetAllocator());
            }
        }
        else if (from.IsNull())
        {
            to.SetNull();
        }
    }
    
    void write_copy_to_string(Benchmark_results& results,
                              std::string& out_str) override
    {
        rapidjson::StringBuffer jstr;
        rapidjson::Writer<StringBuffer> writer(out_str);
        document_copy.Accept(writer);
    }
    rapidjson::Document document;
    rapidjson::Document document_copy;

};

#endif // __RapidjsonBenchmarker_h__
