#include "jsonland/json_node.h"
#include "gtest/gtest.h"

#include <set>
#include <iostream>
#include <filesystem>
#include <fstream>

bool is_allowed_file_name(const std::string& in_file_name)
{
    bool retVal = true;
    
    static std::set<std::string> only_files;
    static bool init_only_files = false;
    if (! init_only_files)
    {
        //only_files.emplace("n_structure_end_array.json");
        init_only_files = true;
    }
    
    static std::set<std::string> exempt_files;
    static bool init_exempt_files = false;
    if (! init_exempt_files)
    {
        exempt_files.emplace("n_string_unescaped_tab.json");
        exempt_files.emplace("n_string_unescaped_newline.json");
        exempt_files.emplace("fail1.json"); // "A JSON payload should be an object or array, not a string." - not true
        
        init_exempt_files = true;
    }

    if (!only_files.empty())
    {
        retVal = only_files.count(in_file_name);
    }
    else if (exempt_files.count(in_file_name) == 1)
    {
        retVal = false;
    }
    else if ('i' == in_file_name[0])
    {
        retVal = false;
    }

    
    return retVal;
}
#if 0

TEST(Suite, JSON)
{
    const std::filesystem::path test_parsing_folder{"./test_data/JSONTestSuite/test_parsing"};
   
    auto dir_iter = std::filesystem::directory_iterator{test_parsing_folder};
    size_t num_files = 0;
    for (const auto& dir_entry : dir_iter)
    {
        std::string file_name = dir_entry.path().filename().string();
        if (is_allowed_file_name(file_name))
        {
            std::ifstream ifs(dir_entry.path());
            std::string contents((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            
            jsonland::json_doc doc;
            jsonland::ParseResult parse_result = doc.parse_insitu(contents);
            
            if ('y' == file_name[0])
                EXPECT_TRUE(0 == parse_err) << "parsing " << file_name << " should not fail";
            else if ('n' == file_name[0])
                EXPECT_TRUE(0 != parse_err) << "parsing " << file_name << " should fail";
           
            ++num_files;
        }
    }
    std::cout << "tested " << num_files << " files in 'JSONTestSuite/test_parsing' folder" << "\n";;
}

TEST(Suite, ORG)
{
    const std::filesystem::path test_parsing_folder{"./test_data/json.org"};
   
    auto dir_iter = std::filesystem::directory_iterator{test_parsing_folder};
    size_t num_files = 0;
    for (const auto& dir_entry : dir_iter)
    {
        std::string file_name = dir_entry.path().filename().string();
        if (is_allowed_file_name(file_name))
        {
            std::ifstream ifs(dir_entry.path());
            std::string contents((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            
            jsonland::json_doc doc;
            doc.set_max_nesting_level(19); // fail18.json has 20 nesting levels
            jsonland::ParseResult parse_result = doc.parse_insitu(contents);
            
            if ('p' == file_name[0])
                EXPECT_TRUE(0 == parse_err) << "parsing " << file_name << " should not fail";
            else if ('f' == file_name[0])
                EXPECT_TRUE(0 != parse_err) << "parsing " << file_name << " should fail";
           
            ++num_files;
        }
    }
    std::cout << "tested " << num_files << " files in 'JSONTestSuite/test_parsing' folder" << "\n";;
}
#endif
