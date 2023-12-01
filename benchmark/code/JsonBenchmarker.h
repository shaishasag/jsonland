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
    size_t file_size;
    std::chrono::duration<double, std::milli> file_parse_duration_milli;
    std::chrono::duration<double, std::milli> resusive_copy_duration_milli;
    std::chrono::duration<double, std::milli> write_to_string_duration_milli;
    
    void report_results(std::ostream& out_stream)
    {

        std::cout << "Test: " << test_name << std::endl;
        std::cout << "    " << "file: " << file_path << std::endl;
        std::cout << "    " << "file size: " << file_size << " bytes " << std::endl;
        if (0 == error)
        {
            uint64_t parse_bytes_per_milli = double(file_size) / double(file_parse_duration_milli.count());
            std::cout << "    " << "parsing time: " << std::fixed << file_parse_duration_milli.count()  << "ms; ";
            std::cout << parse_bytes_per_milli << " bytes per millisecond" << std::endl;
            
            uint64_t copy_bytes_per_milli = double(file_size) / double(resusive_copy_duration_milli.count());
            std::cout << "    " << "recursive copy time: " << std::fixed << resusive_copy_duration_milli.count()  << "ms; ";
            std::cout << copy_bytes_per_milli << " bytes per millisecond" << std::endl;
            
            uint64_t write_copy_to_file_bytes_per_milli = double(file_size) / double(write_to_string_duration_milli.count());
            std::cout << "    " << "write to string time: " << std::fixed << write_to_string_duration_milli.count()  << "ms; ";
            std::cout << write_copy_to_file_bytes_per_milli << " bytes per millisecond" << std::endl;
        }
        else
        {
            std::cout << "    " << "error: " << error << std::endl;
        }
    }
};

class JsonBenchmarker
{
public:
    JsonBenchmarker() = default;
    
    std::string parser_name;
    std::string contents;
    virtual void parse_file(const std::filesystem::path& in_path, Benchmark_results& results) = 0;
    virtual void recursive_copy(Benchmark_results& results) = 0;
    virtual void write_copy_to_file(const std::filesystem::path& in_path, Benchmark_results& results) = 0;

    void benchmark_file(std::filesystem::path& in_path, Benchmark_results& results)
    {
        results.test_name = parser_name;
        results.test_name += " ";
        results.test_name += in_path.filename();

        parse_file(in_path, results);
        recursive_copy(results);
        write_copy_to_file(in_path, results);
    }

};

#endif // __JsonBenchmarker_h__
