#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"

TEST(ParseConsts, good_bools)
{
    std::vector<std::string_view> bool_array = {
        "false", " false",
        "false ", " false ",
        "true", " true",
        "true ", " true ",
    };
    for (auto bool_str : bool_array)
    {
        jsonland::json_doc doc;
        jsonland::ParseResult parse_result = doc.parse(bool_str);
        ASSERT_TRUE(parse_result.ok()) << "parsing '" << bool_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_FALSE(doc.is_string()) << "parsing '" << bool_str << "' should not yield a string";;
        ASSERT_TRUE(doc.is_bool()) << "parsing '" << bool_str << "' should yield a bool";;
    }
}

TEST(ParseConsts, good_nulls)
{
    std::vector<std::string_view> null_array = {
        "null", " null",
        "null ", " null ",
    };
    for (auto null_str : null_array)
    {
        jsonland::json_doc doc;
        jsonland::ParseResult parse_result = doc.parse(null_str);
        ASSERT_TRUE(parse_result.ok()) << "parsing '" << null_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_FALSE(doc.is_string()) << "parsing '" << null_str << "' should not yield a string";;
        ASSERT_TRUE(doc.is_null()) << "parsing '" << null_str << "' should yield a bool";;
    }
}

TEST(ParseConsts, bad_consts)
{
    std::vector<std::string_view> str_array = {
        "tRUE",     // wrong case
        "tru",      //  too short
        "tru ",      //  too short + space
        "true_"     // extra chars
        "fAlse",    // wrong case
        "fals",     //  too short
        "fals ",     //  too short + space
        "false_"    // extra chars
        "nUll",     // wrong case
        "nul",      //  too short
        "nul ",      //  too short + space
        "null+"     // extra chars
        "null + "     // extra chars
    };


    for (auto const_str : str_array)
    {
        jsonland::json_doc doc;
        jsonland::ParseResult parse_result = doc.parse(const_str);
        ASSERT_FALSE(parse_result.ok()) << "parsing '" << const_str << "' should fail";
        ASSERT_FALSE(doc.is_valid());
        ASSERT_FALSE(doc.is_bool());
        ASSERT_FALSE(doc.is_null());
        ASSERT_FALSE(doc.is_string()) << "parsing '" << const_str << "' should not yield a string_t";;
        //std::cout << std::fixed <<  doc.get_string()  << "\n";
    }
}
