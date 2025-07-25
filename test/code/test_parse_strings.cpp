#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"

TEST(ParseStrings, good_strings)
{
    std::vector<std::string_view> str_array = {
        "\"\"",
        "\"a\"",
        "\"ab\"",
        "\"a b \"",
        "\" a b\"",
        "\" a b \"",
        "\"0\"",
        "\"00\"",
        "\"01\"",
        "\"1 2 \"",
        "\" 3 4\"",
        "\" 5 6 \"",
        "\"He said: \\\"Hi\\\"\"",     // Escaped quote
        "\"Line\\nBreak\"",         // Escaped newline
        "\"Tab\\tIndent\"",         // Escaped tab
        "\"😊\"",                  // Unicode emoji (UTF-8 encoded)
        "\"中文测试\"",              // Unicode characters
        "\"C:\\\\Users\\\\Name\""      // Windows path with escaped backslashes
    };


    for (auto str_str : str_array)
    {
        jsonland::json_doc doc;
        jsonland::ParseResult parse_result = doc.parse(str_str);
        ASSERT_TRUE(parse_result.ok())  << "parsing '" << str_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_TRUE(doc.is_string()) << "parsing '" << str_str << "' should yield a string";;
        ASSERT_FALSE(doc.is_number()) << "parsing '" << str_str << "' should not yield a number";;
        //std::cout << std::fixed << doc.get_string()  << "\n";
    }
}

TEST(ParseStrings, bad_strings)
{
    std::vector<std::string_view> str_array = {
        "hello",   // Not in double quotes
        "'hello'"  //  Single quotes not allowed in JSON
        "\"Unescaped \" quote\"",  // Inner quote not escaped
        "\"Bad\\escape\"",    // \e is not valid escape
        "\"Dangling\\\"",     // Ends with an incomplete escape
        "\"Tab\tHere\"",    // Literal tab not escaped (\t required)
        "\"C:\\Path\""       // Backslash not escaped (\\ needed)
    };


    for (auto str_str : str_array)
    {
        jsonland::json_doc doc;
        jsonland::ParseResult parse_result = doc.parse(str_str);
        ASSERT_FALSE(parse_result.ok()) << "parsing '" << str_str << "' should fail";
        ASSERT_FALSE(doc.is_valid());
        ASSERT_FALSE(doc.is_number());
        ASSERT_FALSE(doc.is_string()) << "parsing '" << str_str << "' should not yield a string_t";;
        //std::cout << std::fixed <<  doc.get_string()  << "\n";
    }
}
