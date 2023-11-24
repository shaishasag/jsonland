#include "jsonland/json_node.h"

// recursive_copy will copy one json_node to another
// and is intended for testing and debugging of
// json_node creation with code and by parsing

using namespace jsonland;

json_node recursive_copy(const json_node& jNodeIn)
{
    json_node jNodeOut;
    if (jNodeIn.is_null())
    {
        jNodeOut = json_node(null_t);
    }
    else if (jNodeIn.is_bool())
    {
        jNodeOut = json_node(jNodeIn.as_bool());
    }
    else if (jNodeIn.is_num())
    {
        jNodeOut = json_node(jNodeIn.as_double());
    }
    else if (jNodeIn.is_string())
    {
        std::string_view the_string = jNodeIn.as_string();
        jNodeOut = json_node(the_string);
    }
    else if (jNodeIn.is_array())
    {
        jNodeOut = json_node(array_t);
        for (auto& inItem : jNodeIn)
        {
            json_node outItem = recursive_copy(inItem);
            jNodeOut.push_back(outItem);
        }
    }
    else if (jNodeIn.is_object())
    {
        jNodeOut = json_node(object_t);

        for (auto& inItem : jNodeIn)
        {
            json_node outItem = recursive_copy(inItem);
            std::string_view the_key = inItem.key();
            jNodeOut[the_key] = outItem;
        }
    }
    
    return jNodeOut;
}
