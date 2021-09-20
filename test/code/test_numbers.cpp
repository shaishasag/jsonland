#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"

TEST(Numbers, good)
{
    std::vector<std::string> num_array = {"0.0","0","-0","-0.0","0.1","-0.1",
        "0e0","-0e0","0e-0","-0e-0","0e+0","-0e+0",
        "0e1","-0e2","0e-3","-0e-4","0e+5","-0e+6",
        "1e1","-1e2","1e-3","-1e-4","1e+5","-1e+6",
        "123.123","-123.123","1.123e45","1.123e-4",
        "1.123e+45","-1.123e45","-1.123e-4","-1.123e+45"};
    
    
    for (auto& num : num_array)
    {
        jsonland::json_doc doc;
        int parse_err = doc.parse(num);
        ASSERT_EQ(0, parse_err) << "parsing '" << num << "' should succeed";
        //std::cout << std::fixed << doc.as_double()  << "\n";
    }
}

TEST(Numbers, bad)
{
    std::vector<std::string> num_array = {".",".0","-.0","-0.",".1","-.1",
        "0e","-0e","0-0","-0-0","0+0","-0+0",
        "123.","-123.","1.123e","1.123e-",
        "1.123e+","-1.123e","-1.123e-","-1.123e+"};
    
    
    for (auto& num : num_array)
    {
        jsonland::json_doc doc;
        int parse_err = doc.parse(num);
        ASSERT_NE(0, parse_err) << "parsing '" << num << "' should fail";
        ASSERT_EQ(0.0, doc.as_double());
        //std::cout << std::fixed <<  doc.as_double()  << "\n";
    }
}
