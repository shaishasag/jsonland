#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#if 0
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
//    path_vec.push_back(file_in_folder.replace_filename("example_3.json"));

}
#endif
#include <nlohmann/json.hpp>

int main(int argc, char* argv[])
{
    nlohmann::json bk{nlohmann::json::object()};
    bk["0"] = nlohmann::json("the original");

    bk["1"].push_back(bk["0"]); // <<<

    std::cout << "bk: " << bk.dump() << std::endl;
    std::cout << "bk[0]: " << bk[0].dump() << std::endl;
    std::cout << "bk[1]: " << bk[1].dump() << std::endl;

//    std::vector<std::filesystem::path> file_vec;
//    prepare_file_list(file_vec);
//
//    std::vector<Benchmark_results> res_vec;
//
//    for (auto& path : file_vec)
//    {
//        {
//            JsonlandBenchMarker jsl_bench;
//            jsl_bench.benchmark_file(path, res_vec.emplace_back());
//        }
//        {
//            RapidjsonBenchmarker rpj_bench;
//            rpj_bench.benchmark_file(path, res_vec.emplace_back());
//        }
////        {
////            ArduinoJsonBenchmarker ard_bench;
////            ard_bench.benchmark_file(path, res_vec.emplace_back());
////        }
//    }
//
//    for (auto& res : res_vec)
//    {
//        res.report_results(std::cout);
//    }
}
