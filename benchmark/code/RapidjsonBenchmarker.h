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
        document_copy.CopyFrom(document, document_copy.GetAllocator());
        auto after = std::chrono::steady_clock::now();
        results.resusive_copy_duration_milli = after - before;
        document.SetObject();
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
                jNodeOut.PushBack(outItem, document.GetAllocator());
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
                key.SetString(the_key.c_str(), the_key.size(), document.GetAllocator());
                jNodeOut.AddMember(key, outItem, document.GetAllocator());
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
