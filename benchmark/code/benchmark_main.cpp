#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include "json_node_benchmarker.h"
#include "JsOn_benchmarker.h"
//#include "RapidjsonBenchmarker.h"
//#include "NlohmannBenchmarker.h"

static void prepare_file_list(std::vector<std::filesystem::path>& path_vec)
{
    std::filesystem::path this_file = std::filesystem::path(__FILE__);

    std::filesystem::path input_files = this_file.parent_path().parent_path();
    input_files.append("files");
    
    std::filesystem::path results = this_file.parent_path().parent_path();
    results.append("results");
    std::filesystem::create_directory(results);

    input_files.append("dummy.json");
    path_vec.push_back(input_files.replace_filename("valid_strings.json"));
    path_vec.push_back(input_files.replace_filename("valid_numbers.json"));
    path_vec.push_back(input_files.replace_filename("example_1.json"));
    path_vec.push_back(input_files.replace_filename("example_2.json"));
    path_vec.push_back(input_files.replace_filename("example_3.json"));
}

int main(int argc, char* argv[])
{

    std::cout << "sizeof(json_node): " << sizeof(json_node) << std::endl;
    std::cout << "sizeof(JsOn): " << sizeof(JsOn) << std::endl;

    std::vector<std::filesystem::path> file_vec;
    prepare_file_list(file_vec);

    std::vector<Benchmark_results> res_vec;

    for (auto& path : file_vec)
    {
        
        std::ifstream ifs(path);
        std::string contents = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        {
            //std::cout << "    with json_node" << std::endl;
            json_node_benchmarker jsl_bench;
            auto& results = res_vec.emplace_back();
            results.file_path = path.string();
            results.file_size = contents.size();
            results.contents = contents;
            jsl_bench.benchmark_file(results);
            res_vec.back().report_results(std::cout);
        }
        {
            //std::cout << "    with JsOn" << std::endl;
            JsOn_benchmarker JsOn_bench;
            auto& results = res_vec.emplace_back();
            results.file_path = path.string();
            results.file_size = contents.size();
            results.contents = contents;
            JsOn_bench.benchmark_file(results);
            res_vec.back().report_results(std::cout);
        }
        {
//            RapidjsonBenchmarker rpj_bench;
//            rpj_bench.benchmark_file(path, res_vec.emplace_back());
        }
        {
//            NlohmannBenchmarker nloh_bench;
//            nloh_bench.benchmark_file(path, res_vec.emplace_back());
        }
    }

//    for (auto& res : res_vec)
//    {
//        res.report_results(std::cout);
//    }
}
