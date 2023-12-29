#include <string>
#include <iostream>
#include <fstream>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(TestEscape, just_a_string)
{
    std::string input_file_path = "./test_data/";
    input_file_path += ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
    input_file_path += ".";
    input_file_path += ::testing::UnitTest::GetInstance()->current_test_info()->name();
    input_file_path += ".json";
    
    std::ifstream ifin(input_file_path);
    std::string input_json_str((std::istreambuf_iterator<char>(ifin)), std::istreambuf_iterator<char>());

    jsonland::json_doc jl;
    jl.parse_insitu(const_cast<char*>(input_json_str.c_str()), const_cast<char*>(input_json_str.c_str())+input_json_str.size());
    std::string out_json_string(jl.get_string());
    std::cout << out_json_string << "\n";


    std::string expected_file_path = "./test_data/";
    expected_file_path += ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
    expected_file_path += ".";
    expected_file_path += ::testing::UnitTest::GetInstance()->current_test_info()->name();
    expected_file_path += ".expected";
    std::ifstream ifexpect(expected_file_path);
    std::string expected_str((std::istreambuf_iterator<char>(ifexpect)), std::istreambuf_iterator<char>());

    EXPECT_EQ(out_json_string, expected_str);
}
