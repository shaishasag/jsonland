#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include "jsonland/JsOn.h"
#include "2TypesTestsCommon.h"

using namespace jsonland;

template <typename T>
class ParseArrayTests : public JsonTypedTest<T> {};

TYPED_TEST_SUITE(ParseArrayTests, json_doc_andJsOn);

TYPED_TEST(ParseArrayTests, valid_arrays)
{
    using j_t = typename TestFixture::JsonType;

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
        j_t doc;
        int parse_err = doc.parse_inplace(array_str);
        ASSERT_EQ(0, parse_err) << "parsing '" << array_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_TRUE(doc.is_array()) << "parsing '" << array_str << "' should not yield an array";;
    }
}

TYPED_TEST(ParseArrayTests, invalid_arrays)
{
    using j_t = typename TestFixture::JsonType;
    
    std::vector<std::string_view> array_array = {
        "[", "  [",
        "[ ", " [ ",
        "]", "  ]",
        "] ", " ] ",
        "[true,]", "[ true ,]",
        "[true, ]", "[ , true ]",
        " [1,2,3,]",
        " [1, \"a\" ",
        " [1, \"a] "
    };

    for (auto array_str : array_array)
    {
        j_t doc;
        int parse_err = doc.parse_inplace(array_str);
        ASSERT_NE(0, parse_err) << "parsing '" << array_str << "' should not succeed";
        ASSERT_FALSE(doc.is_valid());
        ASSERT_FALSE(doc.is_array()) << "parsing '" << array_str << "' should not yield an array";;
    }
}

