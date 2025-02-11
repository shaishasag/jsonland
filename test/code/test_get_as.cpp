#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(GetAs, all)
{
    json_node j(object_t);

    j["null"] = nullptr;
    j["bool"] = true;
    j["int"] = 17;
    j["float"] = 3.1415;
    j["string"] = "mama mia";

    EXPECT_EQ(j["null"].get_as<nullptr_t>(), nullptr);
    EXPECT_EQ(j["null"].get_as<bool>(), false);
    EXPECT_EQ(j["null"].get_as<int>(), 0);
    EXPECT_EQ(j["null"].get_as<float>(), 0.0f);
    EXPECT_EQ(j["null"].get_as<double>(), 0.0);
    EXPECT_EQ(j["null"].get_as<std::string_view>(), "null"sv);
    EXPECT_EQ(j["null"].get_as<std::string>(), "null"sv);

    EXPECT_EQ(j["bool"].get_as<nullptr_t>(), nullptr);
    EXPECT_EQ(j["bool"].get_as<bool>(), true);
    EXPECT_EQ(j["bool"].get_as<int>(), 1);
    EXPECT_EQ(j["bool"].get_as<float>(), 1.0f);
    EXPECT_EQ(j["bool"].get_as<double>(), 1.0);
    EXPECT_EQ(j["bool"].get_as<std::string_view>(), "true"sv);
    EXPECT_EQ(j["bool"].get_as<std::string>(), "true"sv);

    EXPECT_EQ(j["int"].get_as<nullptr_t>(), nullptr);
    EXPECT_EQ(j["int"].get_as<bool>(), true);
    EXPECT_EQ(j["int"].get_as<int>(), 17);
    EXPECT_EQ(j["int"].get_as<float>(), 17.0f);
    EXPECT_EQ(j["int"].get_as<double>(), 17.0);
    EXPECT_EQ(j["int"].get_as<std::string_view>(), "17"sv);
    EXPECT_EQ(j["int"].get_as<std::string>(), "17"sv);

    EXPECT_EQ(j["float"].get_as<nullptr_t>(), nullptr);
    EXPECT_EQ(j["float"].get_as<bool>(), true);
    EXPECT_EQ(j["float"].get_as<int>(), 3);
    EXPECT_EQ(j["float"].get_as<float>(), 3.1415f);
    EXPECT_EQ(j["float"].get_as<double>(), 3.1415);
    EXPECT_EQ(j["float"].get_as<std::string_view>(), "3.141500000000000181"sv);
    EXPECT_EQ(j["float"].get_as<std::string>(), "3.141500000000000181"sv);

    EXPECT_EQ(j["string"].get_as<nullptr_t>(), nullptr);
    EXPECT_EQ(j["string"].get_as<bool>(), false);
    EXPECT_EQ(j["string"].get_as<int>(), 0);
    EXPECT_EQ(j["string"].get_as<float>(), 0.0f);
    EXPECT_EQ(j["string"].get_as<double>(), 0.0);
    EXPECT_EQ(j["string"].get_as<std::string_view>(), "mama mia"sv);
    EXPECT_EQ(j["string"].get_as<std::string>(), "mama mia"sv);
}
