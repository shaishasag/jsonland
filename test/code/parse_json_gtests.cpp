#include "jsonland/json_node.h"
#include "gtest/gtest.h"
//#include "nlohmann/json.hpp"
#include <string>
#include <fstream>
#include <streambuf>

using namespace jsonland;


//TEST(Parse, one)
//{
//    auto js1 = R"(   [ {   "abi"  : "cadi"}, TRUE , "manyana" , 123.90 ,[1,2,3,{},{},{"one": 1, "two": 2}]  ])";
//    std::cout << js1 << ":\n";
//    pars_string_to_json(js1, std::strlen(js1));
//    
//    auto js2 = R"("banana")";
//    std::cout << js2 << ":\n";
//    pars_string_to_json(js2, std::strlen(js2));
//    
//    auto js3 = R"([trUe])";
//    std::cout << js3 << ":\n";
//    pars_string_to_json(js3, std::strlen(js3));
//}

const char* long_file_path = "/Users/shai/Downloads/sf-city-lots-json-master/citylots.json";
const char* short_file_path = "/Users/shai/Downloads/sf-city-lots-json-master/citylots-short.json";
const char* tiny_file_path = "/Users/shai/Downloads/sf-city-lots-json-master/citylots-tiny.json";

TEST(Parse, big_file)
{
    std::ifstream ifs(tiny_file_path);
    std::string jstr((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>());

    json_doc jd;
    jd.parse_insitu(jstr);
}

//TEST(Parse, nlohmann)
//{
//    std::ifstream ifs(tiny_file_path);
//    std::string jstr((std::istreambuf_iterator<char>(ifs)),
//                     std::istreambuf_iterator<char>());
// 
//    auto before = std::chrono::steady_clock::now();
//    auto hloh_j = nlohmann::json::parse(jstr.c_str());
//    auto after = std::chrono::steady_clock::now();
//    auto nloh_fixed = std::chrono::duration<double, std::micro>(after - before).count()/1000;
//    
////    std::cout << "nlohmann:\n";
////    std::cout << "parse " << jstr.size() << " bytes " << nloh_fixed << "ms" << "\n";
////    std::cout << hloh_j.dump() << "\n";;
//}
