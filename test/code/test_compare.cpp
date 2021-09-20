#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(TestCompare, all)
{
    json_node jn_sources[6]{json_node("Margot Robbie"), 19.19, json_node(false), json_node(nullptr), json_node(true), json_node("Jackson Pollock")};
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            if (i == j)
            {
                EXPECT_EQ(jn_sources[i], jn_sources[j]);
            }
            else
            {
                EXPECT_NE(jn_sources[i], jn_sources[j]);
            }
        }
    }
}

TEST(TestCompare, numbers)
{
    auto num1_text = "1.000000000000000011";
    //double num1_binary = 1.000000000000000011;
    auto num2_text = "1.000000000000000012";
    double num2_binary = 1.000000000000000012;

    json_node jn_num1(num1_text, json_node::node_type_num);
    json_node jn_num2(num2_text, json_node::node_type_num);
    EXPECT_NE(jn_num1, jn_num2); // not equal since both constructed with strings but the strings are different

    auto as_num1 = jn_num1.as_double();
    auto as_num2 = jn_num2.as_double();
    EXPECT_EQ(as_num1, as_num2); // equal because the binary representation is the same

    json_node jn_num3(num2_binary);
    EXPECT_EQ(jn_num1, jn_num3); // equal because the binary representation is the same
}
