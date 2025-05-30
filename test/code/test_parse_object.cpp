#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"

TEST(ParseObjects, good_objects)
{
    std::vector<std::string_view> obj_array = {
        "{}", "  {}",
        "{} ", " {} ",
        "{ }", "  {  }",
        "{    }    ",
        "{\"a\": true}", "{ \"a\":true}",
        "{\"a\":true }", "{ \"a\" : true }",
    };
    for (auto obj_str : obj_array)
    {
        jsonland::json_doc doc;
        int parse_err = doc.parse(obj_str);
        ASSERT_EQ(0, parse_err) << "parsing '" << obj_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_TRUE(doc.is_object()) << "parsing '" << obj_str << "' should not yield an array";;
    }
}

TEST(ParseObjects, bad_objects)
{
    std::vector<std::string_view> object_array = {
        "{", "  {",
        "{ ", " { ",
        "}", "  }",
        "} ", " } ",
        "{true,}", "{ true ,}",
        "{true, }", "{ , true }",
        "{1:1}", "{ 1.2 : 1.2}",
        "{3.4  :true }", "{  5.6 : null}",
        "{true : \"a\"}", "{ \"a\": }",
        "{ \"a\" : \"a\" ]", "{ \"a\" : \"a\" ",
        "\"a\",\"a\"", "\"a\":,\"a\"",
        " { \"a\" : 1, \"b\" : false, \"c\":\"mama",     // no closing ", no closing }
        " { \"a\" : 1, \"b\" : false, \"c\":\"mama\"  "  // no closing }
        " { \"a\" : 1, \"b\" : false, \"c\":\"mama}  "   // no closing "
    };
    for (auto object_str : object_array)
    {
        jsonland::json_doc doc;
        int parse_err = doc.parse(object_str);
        ASSERT_NE(0, parse_err) << "parsing '" << object_str << "' should not succeed";
        ASSERT_FALSE(doc.is_valid());
        ASSERT_FALSE(doc.is_object()) << "parsing '" << object_str << "' should not yield an object";
    }
}

