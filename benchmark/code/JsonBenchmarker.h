#ifndef __JsonBenchmarker_h__
#define __JsonBenchmarker_h__

#include <filesystem>
#include <chrono>
#include <fstream>
#include <iostream>

struct Benchmark_results
{
    int error{0};
    std::string test_name;
    std::string file_path;
    std::string_view contents;
    size_t file_size;
    std::chrono::duration<double, std::milli> file_parse_duration_milli;
    std::chrono::duration<double, std::milli> resusive_copy_duration_milli;
    std::chrono::duration<double, std::milli> write_to_string_duration_milli;
    std::chrono::duration<double, std::milli> write_to_string_duration_milli_2;

    void report_results(std::ostream& out_stream)
    {

        out_stream << "Test: " << test_name << std::endl;
        out_stream << "    " << "file: " << file_path << std::endl;
        out_stream << "    " << "file size: " << file_size << " bytes " << std::endl;
        if (0 == error)
        {
            uint64_t parse_bytes_per_milli = static_cast<uint64_t>(double(file_size) / double(file_parse_duration_milli.count()));
            out_stream << "    " << "parsing time: " << std::fixed << file_parse_duration_milli.count()  << "ms; ";
            out_stream << parse_bytes_per_milli << " bytes per millisecond" << std::endl;
            
            uint64_t copy_bytes_per_milli = static_cast<uint64_t>(double(file_size) / double(resusive_copy_duration_milli.count()));
            out_stream << "    " << "recursive copy time: " << std::fixed << resusive_copy_duration_milli.count()  << "ms; ";
            out_stream << copy_bytes_per_milli << " bytes per millisecond" << std::endl;
            
            uint64_t write_copy_to_file_bytes_per_milli = static_cast<uint64_t>(double(file_size) / double(write_to_string_duration_milli.count()));
            
            out_stream << "    " << "write to string time: " << std::fixed << write_to_string_duration_milli.count()  << "ms; ";
            out_stream << write_copy_to_file_bytes_per_milli << " bytes per millisecond" << std::endl;
            
            if (0.0 != write_to_string_duration_milli_2.count())
            {
                out_stream << "    " << "fixed::json_creator write time: " << std::fixed << write_to_string_duration_milli_2.count()  << "ms; " << std::endl;
            }
        }
        else
        {
            out_stream << "    " << "error: " << error << std::endl;
        }
    }
};

class JsonBenchmarker
{
public:
    JsonBenchmarker() = default;
    virtual ~JsonBenchmarker() = default;

    std::string parser_name;
    std::string_view contents;
    virtual void parse_text(Benchmark_results& results) = 0;
    virtual void recursive_copy(Benchmark_results& results) = 0;
    virtual void write_copy_to_string(Benchmark_results& results,
                                      std::string& out_str) = 0;

    void benchmark_file(Benchmark_results& results)
    {
        results.test_name = parser_name;
        results.test_name += " ";
        //results.test_name += results.file_path.filename();

        {
            auto before = std::chrono::steady_clock::now();
            parse_text(results);
            auto after = std::chrono::steady_clock::now();
            results.file_parse_duration_milli = after - before;
        }

        {
            auto before = std::chrono::steady_clock::now();
            recursive_copy(results);
            auto after = std::chrono::steady_clock::now();
            results.resusive_copy_duration_milli = after - before;
        }

        {
            std::string out_str;
            auto before = std::chrono::steady_clock::now();
            write_copy_to_string(results, out_str);
            auto after = std::chrono::steady_clock::now();
            results.write_to_string_duration_milli = after - before;

            std::filesystem::path results_dir = results.file_path;
            std::string results_file_name = results_dir.stem();
            
            std::filesystem::path results_file_path = results_dir.parent_path().parent_path();
            results_file_path.append("results");
            results_file_path.append(results_file_name);
            std::string new_extension = parser_name;
            new_extension += ".out.json";
            results_file_path.replace_extension(new_extension);

            std::ofstream fs(results_file_path);
            fs.write(out_str.data(), out_str.size());
        }

    }

};

#endif // __JsonBenchmarker_h__
