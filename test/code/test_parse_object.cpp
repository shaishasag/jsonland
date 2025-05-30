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
