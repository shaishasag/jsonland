#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(TestIter, array)
{
    json_node jn(jsonland::node_type::_array);
    
    jn.push_back(json_node("one"));
    jn.push_back(json_node("two"));
    jn.push_back(json_node("three"));
    
    for (auto iter : jn)
    {
        std::cout << iter.as_string() << "\n";
    }
}

TEST(TestIter, obj)
{
    json_node jn(jsonland::node_type::_object);
    
    jn["one"] = "one!";
    jn["two"] = "two!";
    jn["three"] = "three!";
    
    for (auto& iter : jn)
    {
        std::cout << iter.as_string() << "\n";
        iter = "sss";
    }
    
    const json_node jn2(jn);
    for (auto& iter : jn2)
    {
        std::cout << iter.as_string() << "\n";
    }
}
