#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"

TEST(Numbers, good_float)
{
    std::vector<std::string_view> num_array = {
        "0.0","0","-0","-0.0","0.1","-0.1",
        "0e0","-0e0","0e-0","-0e-0","0e+0","-0e+0",
        "0e1","-0e2","0e-3","-0e-4","0e+5","-0e+6",
        "1e1","-1e2","1e-3","-1e-4","1e+5","-1e+6",
        "123.123","-123.123","1.123e45","1.123e-4",
        "1.123e+45","-1.123e45","-1.123e-4","-1.123e+45",
        "1", "12", "1234567890"
    };
    

    for (auto& num_str : num_array)
    {
        jsonland::json_doc doc;
        int parse_err = doc.parse(num_str);
        ASSERT_EQ(0, parse_err) << "parsing '" << num_str << "' should succeed";
        ASSERT_TRUE(doc.is_number()) << "parsing '" << num_str << "' should yield a number";;
        ASSERT_EQ(doc.get_float<double>(), std::atof(num_str.data()));
        ASSERT_EQ(doc.get_int<int64_t>(), std::atoll(num_str.data()));
        //std::cout << std::fixed << doc.get_float()  << "\n";
    }
}

TEST(Numbers, bad_numbers)
{
    std::vector<std::string_view> num_array = {
        ".",".0","-.0","-0.",".1","-.1",    // json numbers cannot start with '.' or -.
        "0e","-0e","0-0","-1.123e",         // json numbers cannot end with 'e'
        "1.123e+","-1.123e-","-1.123e+","1.123e-",   // json numbers cannot end with 'e+' or 'e-'
       "-0-0","0+0","-0+0",
        "123.","-123.","1.123e"   // json numbers cannot end with '.'
        };
    
    
    for (auto& num : num_array)
    {
        jsonland::json_doc doc;
        int parse_err = doc.parse(num);
        ASSERT_NE(0, parse_err) << "parsing '" << num << "' should fail";
        ASSERT_EQ(0.0, doc.get_float<double>());
        //std::cout << std::fixed <<  doc.get_float()  << "\n";
    }
}
