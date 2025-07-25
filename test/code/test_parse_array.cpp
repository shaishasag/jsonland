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

TYPED_TEST_SUITE(ParseArrayTests, just_json_doc);


TYPED_TEST(ParseArrayTests, valid_arrays)
{
    using j_t = typename TestFixture::JsonType;

    std::vector<std::string_view> array_array = {
        "[]",  // 0
        "  []",
        "[]  ",
        "  []  ",
        "[ ]",
        "   [ ]",
        "[     ]                         ",
        "[true]",
        "[ true]",
        "[true ]",
        "[ true ]", // 10
        "[1]",
        "[ 2]",
        "[3 ]",
        "[ 4 ]",
        "[1, \"a\"]",
        "[ 2, \"a\"]",
        "[3 , \"a\" ]",
        "[ 4,\"a\" ]",
    };
    
    int index{0};
    for (auto array_str : array_array)
    {
        j_t doc;
        jsonland::ParseResult parsimony = doc.parse_inplace(array_str);
        ASSERT_TRUE(parsimony.ok());
        ASSERT_TRUE(bool(parsimony));
        ASSERT_EQ(0, parsimony.error_code()) << "parsing #" << index << " '" << array_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_TRUE(doc.is_array()) << "parsing #" << index << " '" << array_str << "' should not yield an array";;
        ++index;
    }
}


TYPED_TEST(ParseArrayTests, invalid_arrays)
{
    using j_t = typename TestFixture::JsonType;
    
    std::vector<std::string_view> array_array = {
        "[",  // 0
        "  [",
        "[ ",
        " [ ",
        "]",
        "  ]",
        "] ",
        " ] ",
        "[true,]",
        "[ true ,]",
        "[true, ]",  // 10
        "[ , true ]",
        " [1,2,3,]",
        " [1, \"a\" ",
        " [1, \"a] ",
        "[1, \"a\"",
        "[1, \"a\"    ",
        "[1, \"a\" 2   ",
        "[1, \"a\"]   Zizi",
        "[1"
    };

    int index{0};
    for (auto array_str : array_array)
    {
        j_t doc;
        jsonland::ParseResult parsimony = doc.parse_inplace(array_str);
        ASSERT_FALSE(parsimony.ok());
        ASSERT_FALSE(bool(parsimony));
        ASSERT_NE(0, parsimony.error_code()) << "parsing #" << index << " '" << array_str << "' should not succeed";
        ASSERT_FALSE(doc.is_valid());
        ASSERT_FALSE(doc.is_array()) << "parsing #" << index << " '" << array_str << "' should not yield an array";;
        ++index;
    }
}

