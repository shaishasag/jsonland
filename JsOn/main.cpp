#include <string_view>
using namespace std::literals;
#include <format>
#include <chrono>

#include "JsOn.h"

int main(int argc, const char * argv[])
{
    [[maybe_unused]] std::string_view json_object_text = R"({ "one" : "one" ,  "two"   :     2,"three":3.0})"sv;
    [[maybe_unused]] std::string_view json_array_text = "[ \"one\" , [ \"suki\"  ]  , \t 2,\n true  , false   , null   ]"sv;
    [[maybe_unused]] std::string_view json_empty_object_text = " {  "sv;
    [[maybe_unused]] std::string_view json_string_text = R"("one")"sv;
    [[maybe_unused]] std::string_view json_number_text = R"(1234)"sv;
    
    [[maybe_unused]] std::string_view json_combina_text = R"({"arrr" : [{
}, [], {"a":"a", "b": "b"}], "obj": {"ca":"ca", "cb": "cb"}, 
"num": 123, "yes": true, "no": false, 
"undefined": null
})"sv;
    
    try
    {
        std::string_view text_to_parse = json_combina_text;
        
        JsOn the_json;
        int32_t parse_err = the_json.parse_inplace(text_to_parse);
        
        std::string dumpster;
        the_json.dump(dumpster);
        std::cout << "before:\n" << text_to_parse << "\n";
        if (0 == parse_err)
        {
            std::cout << "after:\n"  << dumpster << "\n";
        }
        else
        {
            std::cout << "parse error: "  << parse_err << "\n";
        }
    }
    catch (std::format_error fe)
    {
        std::cout << "json error: " << fe.exception::what() << std::endl;
    }
    
    
    return 0;
}
