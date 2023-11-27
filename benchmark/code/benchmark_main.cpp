#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
//#include "rapidjson/error/en.h"
//#include "rapidjson/document.h"
//#include "rapidjson/writer.h"
//#include "rapidjson/stringbuffer.h"
//#include "nlohmann/json.hpp"
//#include "jsonland/json_node.h"

#include "JsonlandBenchMarker.h"
#include "RapidjsonBenchmarker.h"
//#include "NlohmannBenchmarker.h"

static void prepare_file_list(std::vector<std::filesystem::path>& path_vec)
{
    std::filesystem::path file_in_folder = std::filesystem::path(__FILE__);
    //file_in_folder.remove_filename(); std::cout << file_in_folder << std::endl;
    file_in_folder = file_in_folder.parent_path();
    file_in_folder = file_in_folder.parent_path();
    file_in_folder.append("files");
    file_in_folder.append("example_1.json");
    path_vec.push_back(file_in_folder);
    
    path_vec.push_back(file_in_folder.replace_filename("example_2.json"));
//    path_vec.push_back(file_in_folder.replace_filename("example_3.json"));

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
    }
    
    for (auto& res : res_vec)
    {
        res.report_results(std::cout);
    }
}
