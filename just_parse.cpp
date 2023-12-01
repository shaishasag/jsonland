#include <filesystem>
#include <string>
#include <chrono>
#include <fstream>
#include <iostream>
#include "jsonland/json_node.h"

int main(int argc, char* argv[])
{
    std::filesystem::path file_in_folder = std::filesystem::path(__FILE__);
    file_in_folder = file_in_folder.parent_path();
    file_in_folder.append("benchmark");
    file_in_folder.append("files");
    file_in_folder.append("example_2.json");
    
    std::ifstream ifs(file_in_folder);
    std::string contents = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    int repeat = 1000;
    
    auto before = std::chrono::steady_clock::now();
    for (int i = 0; i < repeat; ++i)
    {
        jsonland::json_doc jdoc;
        jdoc.parse_insitu(contents);
    }
    auto after = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> file_parse_duration_milli = after - before;

    std::cout << "repeat " << repeat << " parsing " << file_in_folder.filename() << ", " << contents.size() << " bytes, " << file_parse_duration_milli.count() << " ms" << std::endl;
}
