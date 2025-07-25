#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"

TEST(ParseNumbers, good_numbers)
{
    std::vector<std::string_view> num_array = {
        "0","0.0","-0","-0.0","0.1","-0.1",
        "0e0","-0e0","0e-0","-0e-0","0e+0","-0e+0",
        "0e1","-0e2","0e-3","-0e-4","0e+5","-0e+6",
        "1e1","-1e2","1e-3","-1e-4","1e+5","-1e+6",
        "123.123","-123.123","1.123e45","1.123e-4",
        "1.123e+45","-1.123e45","-1.123e-4","-1.123e+45",
        "1", "12", "1234567890"
    };
    

    for (auto num_str : num_array)
    {
        jsonland::json_doc doc;
        jsonland::ParseResult parse_result = doc.parse(num_str);
        ASSERT_TRUE(parse_result.ok()) << "parsing '" << num_str << "' should succeed";
        ASSERT_TRUE(doc.is_valid());
        ASSERT_TRUE(doc.is_number()) << "parsing '" << num_str << "' should yield a number";;
        ASSERT_EQ(doc.get_float<double>(), std::atof(num_str.data()));
        ASSERT_EQ(doc.get_int<int64_t>(), std::atoll(num_str.data()));
        //std::cout << std::fixed << doc.get_float()  << "\n";
    }
}

TEST(ParseNumbers, bad_numbers)
{
    std::vector<std::string_view> num_array = {
        "-"     // - must be followed by digits
        "+"     // + is not valid prefix
        ".",    // Missing integer and fraction digits
        ".0",   // Must have integer before .
        "-.0",  // Must have integer before .
        "-0.",  // Decimal point must be followed by digits
        ".1",   // leading dot not allowed
        "-.1",  // leading dot not allowed, after - sign
        "0e",   // Exponent missing digits
        "-0e",  // Exponent missing digits
        "0-0",  // Invalid syntax
        "-1.123e",  // Exponent missing digits
        "1.123e+",  // Exponent missing digits after +
        "-1.123e-", // Exponent missing digits after -
        "-1.123e+", // Exponent missing digits after +
        "1.123e-",  // Exponent missing digits after -
        "-0-0",     // Invalid syntax
        "0+0",      // + not allowed in number body
        "-0+0",     // Decimal point must be followed by digits
        "123.",     // Decimal point must be followed by digits
        "-123.",    // Exponent incomplete
        "001"       // Leading zeros not allowed
    };
    
    
    for (auto num_str : num_array)
    {
        jsonland::json_doc doc;
        jsonland::ParseResult parse_result = doc.parse(num_str);
        ASSERT_FALSE(parse_result.ok()) << "parsing '" << num_str << "' should fail";
        ASSERT_FALSE(doc.is_valid());
        ASSERT_FALSE(doc.is_number());
        ASSERT_FALSE(doc.is_int());
        ASSERT_FALSE(doc.is_float());
        ASSERT_EQ(0.0, doc.get_float<double>());
        //std::cout << std::fixed <<  doc.get_float()  << "\n";
    }
}
