#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "2TypesTestsCommon.h"

using namespace jsonland;

template <typename T>
class ParseObjectTests : public JsonTypedTest<T> {};

TYPED_TEST_SUITE(ParseObjectTests, json_doc_andJsOn);

TYPED_TEST(ParseObjectTests, valid_objects)
{
    using j_t = typename TestFixture::JsonType;

    std::vector<std::string_view> obj_array = {
        "{}", // 0
        "  {}",
        "{} ",
        " {} ",
        "{ }",
        "  {  }",
        "{    }    ",
        "{\"a\": true}",
        "{ \"a\":true}",
        "{\"a\":true }",
        "{ \"a\" : true }",  // 10
    };
    int index{0};
    for (auto object_str : obj_array)
    {
        j_t doc;
        jsonland::ParseResult parsimony = doc.parse_inplace(object_str);
        ASSERT_TRUE(parsimony.ok()) << "parsing #" << index << " '" << object_str << "' should succeed";
        ASSERT_TRUE(bool(parsimony)) << "parsing #" << index << " '" << object_str << "' should succeed";
        ASSERT_EQ(0, parsimony.error_code()) << "parsing #" << index << " '" << object_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_TRUE(doc.is_object()) << "parsing #" << index << " '" << object_str << "' should not yield an array";
        ++index;
    }
}

TYPED_TEST(ParseObjectTests, invalid_objects)
{
    using j_t = typename TestFixture::JsonType;

    std::vector<std::string_view> object_array = {
        "{", // 0
        "  {",
        "{ ",
        " { ",
        "}",
        "  }",
        "} ",
        " } ",
        "{true,}",
        "{ true ,}",
        "{true, }", // 10
        "{ , true }",
        "{1:1}",
        "{ 1.2 : 1.2}",
        "{3.4  :true }",
        "{123   ",
        "{  5.6 : null}",
        "{true : \"a\"}",
        "{ \"a\": }",
        "{ \"a\" : \"a\" ]",
        "{ \"a\" : \"a\" ", // 20
        "\"a\",\"a\"",
        "\"a\":,\"a\"",
        " { \"a\" : 1, \"b\" : false, \"c\":\"mama",     // no closing ", no closing }
        " { \"a\" : 1, \"b\" : false, \"c\":\"mama\"  "  // no closing }
        " { \"a\" : 1, \"b\" : false, \"c\":\"mama}  "   // no closing "
    };
    int index{0};
    for (auto object_str : object_array)
    {
        j_t doc;
        jsonland::ParseResult parsimony = doc.parse_inplace(object_str);
        ASSERT_FALSE(parsimony.ok()) << "parsing #" << index << " '" << object_str << "' should not succeed";
        ASSERT_FALSE(bool(parsimony)) << "parsing #" << index << " '" << object_str << "' should not succeed";
        ASSERT_NE(0, parsimony.error_code()) << "parsing #" << index << " '" << object_str << "' should not succeed";
        ASSERT_FALSE(doc.is_valid());
        ASSERT_FALSE(doc.is_object()) << "parsing #" << index << " '" << object_str << "' should not yield an object";
        ++index;
    }
}

TEST(ParseObjects, bad_objects1)
{
    jsonland::json_doc doc;
    jsonland::ParseResult parsimony = doc.parse_inplace("{");
    ASSERT_NE(0, parsimony.error_code()) << "parsing '" << "{" << "' should not succeed";
    ASSERT_FALSE(doc.is_valid());
    ASSERT_FALSE(doc.is_object()) << "parsing '" << "{" << "' should not yield an object";

}
