#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(TestOwnership, explicit_constructors)
{
    {
        jsonland::json_node jn;
        EXPECT_TRUE(jn.is_full_owner());
    }
    {
        jsonland::json_node jn(nullptr);
        EXPECT_TRUE(jn.is_full_owner());
    }
    {
        jsonland::json_node jnt(true);
        EXPECT_TRUE(jnt.is_full_owner());
        jsonland::json_node jnf(false);
        EXPECT_TRUE(jnf.is_full_owner());
    }
    {
        jsonland::json_node jns(jsonland::string_t);
        EXPECT_TRUE(jns.is_full_owner());
        jsonland::json_node jnc("marmalade");
        EXPECT_TRUE(jnc.is_full_owner());
    }
    {
        jsonland::json_node jnn(jsonland::number_t);
        EXPECT_TRUE(jnn.is_full_owner());
        jsonland::json_node jni(17);
        EXPECT_TRUE(jni.is_full_owner());

        jsonland::json_node jnns1("1234", jsonland::number_t);
        EXPECT_FALSE(jnns1.is_full_owner());

        std::string a_num_str{"987.654"};
        jsonland::json_node jnns2(a_num_str, jsonland::number_t);
        EXPECT_FALSE(jnns2.is_full_owner());

        jsonland::json_node jnns3(std::move(a_num_str), jsonland::number_t);
        EXPECT_FALSE(jnns3.is_full_owner());
    }
    {
        jsonland::json_node jna(jsonland::array_t);
        EXPECT_TRUE(jna.is_full_owner());
        jsonland::json_node jnc("marmalade");
        EXPECT_TRUE(jnc.is_full_owner());
    }
}

TEST(TestOwnership, take_ownership)
{
    jsonland::json_node jn_cp;
    {
        std::string_view j_str{R"({"one":1, "two": 2.00, "three": "3C:\\PO"})"};
        jsonland::json_doc jd;
        jd.parse_insitu(j_str);
        EXPECT_FALSE(jd.is_full_owner());

        jn_cp = jd;
        EXPECT_FALSE(jn_cp.is_full_owner());
    }
    EXPECT_FALSE(jn_cp.is_full_owner());

    jn_cp.take_ownership();
    EXPECT_TRUE(jn_cp.is_full_owner());
}
