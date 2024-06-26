#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include "JsonlandBenchMarker.h"
#include "RapidjsonBenchmarker.h"
#include "ArduinoJsonBenchmarker.h"

static void prepare_file_list(std::vector<std::filesystem::path>& path_vec)
{
    std::filesystem::path file_in_folder = std::filesystem::path(__FILE__);

    file_in_folder = file_in_folder.parent_path().parent_path();
    file_in_folder.append("files");
    file_in_folder.append("example_1.json");
    
    path_vec.push_back(file_in_folder);
    path_vec.push_back(file_in_folder.replace_filename("example_2.json"));
    path_vec.push_back(file_in_folder.replace_filename("example_3.json"));

}


int main(int argc, char* argv[])
{
    std::vector<std::filesystem::path> file_vec;
    prepare_file_list(file_vec);
    
    std::vector<Benchmark_results> res_vec;

    for (auto& path : file_vec)
    {
        {
            JsonlandBenchMarker jsl_bench;
            jsl_bench.benchmark_file(path, res_vec.emplace_back());
        }
        {
            RapidjsonBenchmarker rpj_bench;
            rpj_bench.benchmark_file(path, res_vec.emplace_back());
        }
//        {
//            ArduinoJsonBenchmarker ard_bench;
//            ard_bench.benchmark_file(path, res_vec.emplace_back());
//        }
    }
    
    for (auto& res : res_vec)
    {
        res.report_results(std::cout);
    }
    
    
    auto before = std::chrono::steady_clock::now();
    std::string jstr;
    for (int i = 0; i < 10'000'000; ++i)
    {
        jstr += 'y';
        jstr += 'z';
    }
    auto after = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> one_char_time = after - before;
    std::cout << "    " << "write two chars 10'000'000 times: " << std::fixed << one_char_time.count()  << "ms; " << std::endl;
    
    before = std::chrono::steady_clock::now();
    std::string svstr;
    constexpr std::string_view two_char_sv{"xy"};
    for (int i = 0; i < 10'000'000; ++i)
    {
        svstr += two_char_sv;
    }
    after = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> sv_time = after - before;
    std::cout << "    " << "write sv 10'000'000 times: " << std::fixed << sv_time.count()  << "ms; " << std::endl;
}
