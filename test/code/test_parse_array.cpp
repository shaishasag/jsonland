#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"

TEST(ParseArrays, good_arrays)
{
    std::vector<std::string_view> array_array = {
        "[]", "  []",
        "[]  ", "  []  ",
        "[ ]", "   [ ]",
        "[     ]                         ",
        "[true]", "[ true]",
        "[true ]", "[ true ]",
        "[1]", "[ 2]",
        "[3 ]", "[ 4 ]",
        "[1, \"a\"]", "[ 2, \"a\"]",
        "[3 , \"a\" ]", "[ 4,\"a\" ]",
    };
    for (auto array_str : array_array)
    {
        jsonland::json_doc doc;
        int parse_err = doc.parse(array_str);
        ASSERT_EQ(0, parse_err) << "parsing '" << array_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_TRUE(doc.is_array()) << "parsing '" << array_str << "' should not yield an array";;
    }
}

TEST(ParseArrays, bad_arrays)
{
    std::vector<std::string_view> array_array = {
        "[", "  [",
        "[ ", " [ ",
        "]", "  ]",
        "] ", " ] ",
        "[true,]", "[ true ,]",
        "[true, ]", "[ , true ]",
    };
    for (auto array_str : array_array)
    {
        jsonland::json_doc doc;
        int parse_err = doc.parse(array_str);
        ASSERT_NE(0, parse_err) << "parsing '" << array_str << "' should not succeed";
        ASSERT_FALSE(doc.is_valid());
        ASSERT_FALSE(doc.is_array()) << "parsing '" << array_str << "' should not yield an array";;
    }
}

