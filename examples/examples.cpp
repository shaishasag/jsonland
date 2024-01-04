#include <string_view>
#include <iostream>

#include "jsonland/json_node.h"

int main(int argc, char* argv[])
{
    
    std::string_view json_str = R"(
        [   {"name": "a bee", "price"  : 10.10, "valid": true},
            {"name": "a fly", "price": 21.21, "valid": false},
            {"name": "a bird", "price": 31.31, "valid": true},
            {"name": "a deer", "price": 41.41}
        ]
    )";

    jsonland::json_doc jdoc;
    jdoc.parse(json_str);
    
    assert(jdoc.is_array());
    
    std::cout << "json before: " << jdoc << "\n";

    double total = 0.0;
    int num_valid_items = 0;
    // sum all items marked "valid": true ,
    // if item does not have "valid" field defaukt to false
    // if item does not have price field, add one with value 0.0
    for (auto& j : jdoc)
    {
        j.get_value("price", 17);
        j.get_value<int>("price", 17);
        j.get_value<double>("price", 17);
        j.get_value<const char*>("name", "petri dish");
        j.get_value<std::string_view>("name", "petri dish");
        j.get_value<std::string>("name", "petri dish");

        if (j.is_object()) {
            
            if (! j.contains("price")) {
                j["price"] = 0.0;
            }

            bool is_valid = j.get_value<bool_t>("valid", false);
            if (is_valid)
            {
                total += j["price"].get_float();
                num_valid_items += 1;
            }
            
        }
    }
    
    
    std::cout << "json after: " << jdoc << "\n";

    std::cout << "found " << num_valid_items << " valid items" << "\n";
    std::cout << "total " << total << "\n";

}


