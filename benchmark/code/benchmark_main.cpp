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
    std::filesystem::path file_in_folder = std::filesystem::path(__FILE__);

    file_in_folder = file_in_folder.parent_path().parent_path();
    file_in_folder.append("files");

    file_in_folder.append("valid_strings.json");
    
    path_vec.push_back(file_in_folder);
    
    path_vec.push_back(file_in_folder.replace_filename("example_2.json"));
    path_vec.push_back(file_in_folder.replace_filename("example_3.json"));

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
