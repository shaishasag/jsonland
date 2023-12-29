#include "gtest/gtest.h"
#include <array>
#include "jsonland/json_node.h"

using namespace jsonland;
std::array<std::string, 3> numbers{"one", "two", "three"};

TEST(TestIter, array)
{
    json_node jn(jsonland::value_type::array_t);
    for (auto& s : numbers) {
        jn.push_back(s);
    }
    
    EXPECT_EQ(numbers[0], jn[0].get_string());
    EXPECT_EQ(numbers[1], jn[1].get_string());
    EXPECT_EQ(numbers[2], jn[2].get_string());
}

TEST(TestIter, obj)
{
    json_node jn(jsonland::value_type::object_t);
    for (auto& s : numbers) {
        jn[s] = s;
    }
    EXPECT_EQ(numbers[0], jn[numbers[0]]);
    EXPECT_EQ(numbers[1], jn[numbers[1]]);
    EXPECT_EQ(numbers[2], jn[numbers[2]]);

    for (auto& iter : jn)
    {
        std::string new_value(iter.get_string());
        new_value += "!";
        iter = new_value;
    }
    
    int i{0};
    for (auto& iter : jn)
    {
        EXPECT_EQ(iter.get_string(), numbers[i]+"!");
        ++i;
    }
}
