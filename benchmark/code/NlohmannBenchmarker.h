#ifndef __NlohmannBenchmarker_h__
#define __NlohmannBenchmarker_h__

#include <nlohmann/json.hpp>

class NlohmannBenchmarker : public JsonBenchmarker
{
public:
    NlohmannBenchmarker()
    {
        parser_name = "nlohmann";
    }
    
    void parse_file(const std::filesystem::path& in_path, Benchmark_results& results) override
    {
        results.file_path = in_path.string();

        std::ifstream ifs(in_path);
        contents = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        results.file_size = contents.size();

        auto before = std::chrono::steady_clock::now();
        ndoc = nlohmann::json::parse(contents);
        auto after = std::chrono::steady_clock::now();
        results.file_parse_duration_milli = after - before;
    }

    void recursive_copy(Benchmark_results& results) override
    {
        recursive_copy_self(ndoc, ndoc_copy);
        ndoc.clear(); // make sure no allocation from ndoc were used by ndoc_copy
    }
    void recursive_copy_self(const nlohmann::json& jdoc, nlohmann::json& jNodeOut)
    {
        if (jdoc.is_string())
        {
            jNodeOut = jdoc.get<std::string_view>();
        }
        else if (jdoc.is_boolean())
        {
            jNodeOut = jdoc.get<bool>();
        }
        else if (jdoc.is_number_float())
        {
            jNodeOut = jdoc.get<double>();
        }
        else if (jdoc.is_number_integer())
        {
            jNodeOut = jdoc.get<int64_t>();
        }
        else if (jdoc.is_object())
        {
            jNodeOut = nlohmann::json::object();

            for (auto& elem : jdoc.items())
            {
                nlohmann::json objItem;
                recursive_copy_self(elem.value(), objItem);
                std::string_view the_key = elem.key();
                jNodeOut[the_key] = std::move(objItem);
            }
        }
        else if (jdoc.is_array())
        {
            jNodeOut = nlohmann::json::array();
            for (auto& val : jdoc)
            {
                nlohmann::json arrayItem;
                recursive_copy_self(val, arrayItem);
                jNodeOut.push_back(std::move(arrayItem));
            }
        }
        else if (jdoc.is_null())
        {
            jNodeOut = nullptr;
        }
    }

    void write_copy_to_string(Benchmark_results& results,
                              std::string& out_str) override
    {
        out_str = ndoc_copy.dump();
    }

    nlohmann::json ndoc;
    nlohmann::json ndoc_copy;
};

#endif // __NlohmannBenchmarker_h__
