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
    
    void recursive_copy(Benchmark_results& results) override
    {
        auto before = std::chrono::steady_clock::now();
        recursive_copy_self(document, document_copy);
        auto after = std::chrono::steady_clock::now();
        results.resusive_copy_duration_milli = after - before;
        document.SetObject();
    }
    
    void recursive_copy_self(const Value& from, Value& to)
    {
        if (from.IsNull())
        {
            to.SetNull();
        }
        else if (from.IsBool())
        {
            to.SetBool(from.GetBool());
        }
        else if (from.IsNumber())
        {
            to.SetFloat(from.GetDouble());
        }
        else if (from.IsString())
        {
            std::string_view the_string = from.GetString();
            to.SetString(the_string.data(), the_string.size());
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
        else if (from.IsObject())
        {
            to.SetObject();

            for (rapidjson::Value::ConstMemberIterator itr = from.MemberBegin(); itr != from.MemberEnd(); ++itr)
            {
                Value objItem;
                recursive_copy_self(itr->value, objItem);
                std::string the_key = itr->name.GetString();
                //to[the_key.c_str()] = outItem;
                Value key;
                key.SetString(the_key.c_str(), the_key.size(), document_copy.GetAllocator());
                to.AddMember(key, objItem, document_copy.GetAllocator());
            }
        }
    }
    
    void write_copy_to_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        auto out_file = in_path;
        std::string new_extension = parser_name;
        new_extension += ".out.json";
        out_file.replace_extension(new_extension);

        std::ostringstream ofs;
        auto before = std::chrono::steady_clock::now();
        rapidjson::OStreamWrapper osw(ofs);
        rapidjson::Writer<OStreamWrapper> writer(osw);
        document_copy.Accept(writer);
        auto after = std::chrono::steady_clock::now();
        results.write_copy_to_file_duration_milli = after - before;

        std::ofstream ffs(out_file);
        const std::string& s = ofs.str();
        ffs.write(s.c_str(), s.size());
    }
    rapidjson::Document document;
    rapidjson::Document document_copy;

};

#endif // __RapidjsonBenchmarker_h__
