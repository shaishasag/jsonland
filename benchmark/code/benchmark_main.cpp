#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "nlohmann/json.hpp"
#include "jsonland/json_node.h"

class JsonBenchmarker
{
public:
    JsonBenchmarker(const char* in_name) : name(in_name)
    {
//        if (dur_.count() > 0)
//            std::cout << "dur_.count() > 0: " << dur_.count() << std::endl;
    }
    std::string name;
    std::chrono::duration<double> dur_{0};
    size_t total_num_elements = 0;
    size_t total_num_input_bytes = 0;
    size_t total_num_output_bytes = 0;
    size_t total_num_runs = 0;
    
    void benchmark_one(const std::string& in_json_string, const std::string& in_file_path)
    {
        std::string data_copy(in_json_string);
        total_num_input_bytes += data_copy.size();
        
        std::string out_data;
        auto before = std::chrono::steady_clock::now();
        total_num_elements += parse_and_dump(data_copy, out_data);
        auto after = std::chrono::steady_clock::now();
        auto diff = after - before;
        dur_ += diff;

        std::string file_path = in_file_path;
        file_path += ".";
        file_path += name;
        file_path += ".";
        file_path += "out.json";
        std::ofstream ifs(file_path);
        ifs.write(out_data.c_str(), out_data.size());

        total_num_output_bytes += out_data.size();
        ++total_num_runs;
    }
    
    void report(std::ostream& os)
    {
        os << name << " benchmark:\n";
        os << "    " << "total_num_runs: " << total_num_runs << '\n';
        os << "    " << "total_num_input_bytes: " << total_num_input_bytes << '\n';
        os << "    " << "total_num_output_bytes: " << total_num_output_bytes << '\n';
        os << "    " << "total_num_elements: " << total_num_elements << '\n';

        os << "    " << "time: " << dur_.count()*1000 << "ms\n";
    }
    
    virtual size_t parse_and_dump(std::string& in_json_string, std::string& out_json_string) = 0;
};

class JsonlandBenchmarker : public JsonBenchmarker
{
public:
    JsonlandBenchmarker() : JsonBenchmarker("JsonLand") {}
    size_t parse_and_dump(std::string& in_json_string, std::string& out_json_string) override
    {
        jsonland::json_doc jdoc;
        int parse_error = jdoc.parse_insitu(in_json_string);
        
        if (0 != parse_error)
        {
            std::cerr << ">>> Jsonland parse failed: " << jdoc.parse_error() << ", " << jdoc.parse_error_message() << "\n";
        }
        
        out_json_string = jdoc.dump();
        return jdoc.size();
    }
};

 
class RapidjsonBenchmarker : public JsonBenchmarker
{
public:
    RapidjsonBenchmarker() : JsonBenchmarker("Rapidjson") {}
    size_t parse_and_dump(std::string& in_json_string, std::string& out_json_string) override
    {
        int retVal = 0;
        using namespace rapidjson;

        Document document;
        ParseResult pr = document.Parse(in_json_string.c_str());
//        for (Value::ConstMemberIterator iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter){
//            std::cout << "Rapidjson: " << iter->name.GetString() << " => " <<  iter->value.GetString() << "\n";
//        }

        if (pr)
        {
            StringBuffer buffer;
            buffer.Clear();
            Writer<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);
            out_json_string = buffer.GetString();
            
            if (document.IsObject())
                retVal = document.MemberCount();
            else if (document.IsArray())
                retVal = document.Size();
            else if (!document.IsNull())
                retVal = 1;
        }
        else
        {
            std::cerr << ">>> Rapidjson parse failed: " << pr.Code() << ", " << GetParseError_En(pr.Code()) << " at offset " << pr.Offset() << "\n";
        }
        return retVal;
    }
};

class NlohmannBenchmarker : public JsonBenchmarker
{
public:
    NlohmannBenchmarker() : JsonBenchmarker("Nlohmann") {}
    size_t parse_and_dump(std::string& in_json_string, std::string& out_json_string) override
    {
        using namespace nlohmann;
        
        try
        {
            json nj = json::parse(in_json_string.c_str());
//            for (json::iterator it = nj.begin(); it != nj.end(); ++it)
//            {
//                std::cout << "Nlohmann: " << it.key() << " => " <<  it.value() << "\n";
//            }
            out_json_string = nj.dump();
            return nj.size();
        }
        catch (json::parse_error& e)
        {
            std::cerr << ">>> Nlohmann parse failed: " << e.id << ", " <<  e.what() << " at offset " << e.byte << "\n";
        }
        return 0;
    }
};

int main(int argc, char* argv[])
{
    std::vector<std::pair<std::string, std::string>> files_and_datas;
    for (int i = 1; i < argc; ++i)
    {
        std::pair<std::string, std::string> file_and_data;
        file_and_data.first = argv[i];
        std::ifstream ifs(file_and_data.first);
        ifs.seekg( 0, std::ios::end );
        file_and_data.second.reserve(ifs.tellg());
        ifs.seekg( 0, std::ios::beg);

        file_and_data.second.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        
        
        files_and_datas.emplace_back(file_and_data);
    }
    
    std::vector<JsonBenchmarker*> benchmarks;
//    benchmarks.push_back(new RapidjsonBenchmarker);
//    benchmarks.push_back(new NlohmannBenchmarker);
    benchmarks.push_back(new JsonlandBenchmarker);

    int num_runs = 8;
    for (int i = 0; i < num_runs; ++i)
    {
        for (auto benchmarker : benchmarks)
        {
            for (auto& f_and_d : files_and_datas)
            {
                benchmarker->benchmark_one(f_and_d.second, f_and_d.first);
            }
        }
//        std::cout << "---\n";
    }
    for (auto benchmarker : benchmarks)
    {
        benchmarker->report(std::cout);
    }
}
