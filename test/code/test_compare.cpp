#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(Compare, all)
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

TEST(Compare, numbers)
{
    auto num1_text = "1.000000000000000011";
    //double num1_binary = 1.000000000000000011;
    auto num2_text = "1.000000000000000012";
    double num2_binary = 1.000000000000000012;

    json_node jn_num1(num1_text, jsonland::value_type::number_t);
    json_node jn_num2(num2_text, jsonland::value_type::number_t);
    EXPECT_NE(jn_num1, jn_num2); // not equal since both constructed with strings but the strings are different

    auto as_num1 = jn_num1.get_float<double>();
    auto as_num2 = jn_num2.get_float<double>();
    EXPECT_EQ(as_num1, as_num2); // equal because the binary representation is the same

    json_node jn_num3(num2_binary);
    EXPECT_EQ(jn_num1, jn_num3); // equal because the binary representation is the same
}

TEST(Compare, copy_object)
{
    json_node jn_copy_1;
    json_node jn_copy_2;

    {
        json_node jn_original(jsonland::value_type::object_t);
        jn_original["one"] = 1;
        jn_original["two"] = 2.0;
        auto& arrr = jn_original.append_array("three");
        arrr.push_back(1);
        arrr.push_back(2);
        arrr.push_back(3);

        EXPECT_EQ(jn_original, jn_original); // should be equal to itself

        json_node copy_the_arrr_1{arrr};
        json_node copy_the_arrr_2{jn_original["three"]};
        EXPECT_EQ(copy_the_arrr_1, copy_the_arrr_2);

        jn_copy_1 = jn_original.clone();
        jn_copy_2 = std::move(jn_original);
        jn_original.clear();
    } // here jn_original goes out of scope making sure jn_copy_1 & jn_copy_2 are independant
    EXPECT_EQ(jn_copy_1, jn_copy_2);

    jn_copy_1["three"].push_back(4);
    jn_copy_1["four"] = "rock";
    jn_copy_2["three"].push_back(4);
    jn_copy_2["four"] = "rock";
    EXPECT_EQ(jn_copy_1, jn_copy_2); // should still be equal

    jn_copy_1["five"] = "O";
    jn_copy_2["five"] = "0";
    EXPECT_NE(jn_copy_1, jn_copy_2); // now they are different
}

TEST(Compare, move_object)
{
    json_node jn_copy_1;
    json_node jn_copy_2;

    {
        json_node jn_original(jsonland::value_type::object_t);
        jn_original["one"] = 1;
        jn_original["two"] = 2.0;
        auto& arrr = jn_original.append_array("three");
        arrr.push_back(1);
        arrr.push_back(2);
        arrr.push_back(3);

        jn_copy_1 = jn_original.clone();
        jn_copy_2 = std::move(jn_original);
        jn_original.clear();
    } // here jn_original goes out of scope making sure jn_copy_1 & jn_copy_2 are independant
    EXPECT_EQ(jn_copy_1, jn_copy_2);

    jn_copy_1["three"].push_back(4);
    jn_copy_1["four"] = "rock";
    jn_copy_2["three"].push_back(4);
    jn_copy_2["four"] = "rock";
    EXPECT_EQ(jn_copy_1, jn_copy_2); // should still be equal

    jn_copy_1["five"] = "O";
    jn_copy_2["five"] = "0";
    EXPECT_NE(jn_copy_1, jn_copy_2); // now they are different
}
