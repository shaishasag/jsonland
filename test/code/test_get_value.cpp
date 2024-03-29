#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(MemberValue, numbers)
{
    std::string_view a_number{"a number"};
    std::string_view not_number{"not number"};
    std::string_view not_member{"not a member"};

    json_node jObj{object_t};
    jObj[a_number] = 17;
    jObj[not_number] = "not a number";
    
    int32_t i = jObj.member_value(a_number, 1990);
    EXPECT_EQ(i, 17);
    i = jObj.member_value<int32_t>(a_number);
    EXPECT_EQ(i, 17);
    i = jObj.member_value<int32_t>(not_member, 1234);
    EXPECT_EQ(i, 1234);

    int32_t j = jObj.member_value(not_number, 1990);
    EXPECT_EQ(j, 1990);
    j = jObj.member_value<int32_t>(not_number);
    EXPECT_EQ(j, 0);
    j = jObj.member_value<int32_t>(not_member);
    EXPECT_EQ(j, 0);

    double d = jObj.member_value(a_number, 1990);
    EXPECT_EQ(d, 17.0);
    double e = jObj.member_value(not_number, 1990);
    EXPECT_EQ(e, 1990.0);
    e = jObj.member_value(not_member, 1990);
    EXPECT_EQ(e, 1990.0);

    // member_value<bool> without default value for value assigned as number should return false
    bool b1 = jObj.member_value<bool>(a_number);
    EXPECT_EQ(b1, false);
    // member_value<bool> with default value for value assigned as number should return the deafult value
    b1 = jObj.member_value<bool>(a_number, true);
    EXPECT_EQ(b1, true);
    // member_value<bool> without default value for value assigned as string should return false
    b1 = jObj.member_value<bool>(not_number);
    EXPECT_EQ(b1, false);
}

TEST(MemberValue, as_type)
{
    jsonland::json_node s1("I am a string");
    jsonland::json_node b1(true);
    jsonland::json_node p1(nullptr);
    
    EXPECT_EQ(s1.dump(), "\"I am a string\"");
    EXPECT_EQ(s1.get_string(), "I am a string");
    EXPECT_EQ(s1.get_as<std::string>(), "I am a string");
    EXPECT_EQ(s1.get_as<std::string_view>(), "I am a string");
    EXPECT_EQ(s1.get_int<unsigned>(), 0);
    EXPECT_EQ(s1.get_int(17), 17);
    EXPECT_EQ(s1.get_int<int64_t>(), 0);
    EXPECT_EQ(s1.get_as<int64_t>(), 0);
    EXPECT_EQ(s1.get_float<long double>(), 0.0);
    EXPECT_EQ(s1.get_float(17.0), 17.0);
    EXPECT_EQ(s1.get_float<float>(), 0);
    EXPECT_EQ(s1.get_as<float>(), 0.0);
    EXPECT_EQ(s1.get_bool(), false);
    EXPECT_EQ(s1.get_bool(true), (true));
    EXPECT_EQ(s1.get_as<bool>(), false);
    EXPECT_EQ(s1.get_as<bool>(true), true);

    jsonland::json_node n1(1234567);
    EXPECT_EQ(n1.dump(), "1234567");
    EXPECT_EQ(n1.get_int<unsigned>(), 1234567);
    EXPECT_EQ(n1.get_int(17), 1234567);
    EXPECT_EQ(n1.get_int<int64_t>(), 1234567);
    EXPECT_EQ(n1.get_as<int64_t>(), 1234567);
    EXPECT_EQ(n1.get_float<long double>(), 1234567.0);
    EXPECT_EQ(n1.get_float(17.0), 1234567.0);
    EXPECT_EQ(n1.get_float<float>(), 1234567.0f);
    EXPECT_EQ(n1.get_as<float>(), 1234567.0f);
    EXPECT_EQ(n1.get_string(), "");
    EXPECT_EQ(n1.get_as<std::string>(), "");
    EXPECT_EQ(n1.get_as<std::string_view>(), "");

}
