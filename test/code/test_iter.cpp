#include "gtest/gtest.h"
#include <array>
#include "jsonland/json_node.h"

using namespace jsonland;
std::array<std::string, 3> numbers{"one", "two", "three"};

TEST(Iter, array)
{
    json_node jn(jsonland::value_type::array_t);
    for (auto& s : numbers) {
        jn.push_back(s);
    }
    
    EXPECT_EQ(numbers[0], jn[0].get_string());
    EXPECT_EQ(numbers[1], jn[1].get_string());
    EXPECT_EQ(numbers[2], jn[2].get_string());
}

TEST(Iter, obj)
{
    json_node jn(jsonland::value_type::object_t);
    for (auto& s : numbers) {
        jn[s] = s;
    }
    EXPECT_EQ(numbers[0], jn[numbers[0]].get_string());
    EXPECT_EQ(numbers[1], jn[numbers[1]].get_string());
    EXPECT_EQ(numbers[2], jn[numbers[2]].get_string());

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

TEST(Iter, obj_to_array)
{
    json_node jn(jsonland::value_type::object_t);
    std::string_view obj_to_array_jstr = R"(
    {"A": [1],
    "B": [2, 3],
    "C": [4,5,6]}
                                           )";

    json_doc jdoc;
    jdoc.parse(obj_to_array_jstr);

    int total = 0;

    for (auto& itm : jdoc)
    {
        for (auto& num : itm)
        {
            total += num.get_int();
        }
    }
    EXPECT_EQ(total, 21);
}
