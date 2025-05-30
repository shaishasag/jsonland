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
    
    int32_t i = jObj.get_value(a_number, 1990);
    EXPECT_EQ(i, 17);
    i = jObj.get_value<int32_t>(a_number);
    EXPECT_EQ(i, 17);
    i = jObj.get_value<int32_t>(not_member, 1234);
    EXPECT_EQ(i, 1234);

    int32_t j = jObj.get_value(not_number, 1990);
    EXPECT_EQ(j, 1990);
    j = jObj.get_value<int32_t>(not_number);
    EXPECT_EQ(j, 0);
    j = jObj.get_value<int32_t>(not_member);
    EXPECT_EQ(j, 0);

    double d = jObj.get_value(a_number, 1990);
    EXPECT_EQ(d, 17.0);
    double e = jObj.get_value(not_number, 1990);
    EXPECT_EQ(e, 1990.0);
    e = jObj.get_value(not_member, 1990);
    EXPECT_EQ(e, 1990.0);

    // get_value<bool> without default value for value assigned as number should return false
    bool b1 = jObj.get_value<bool>(a_number);
    EXPECT_EQ(b1, false);
    // get_value<bool> with default value for value assigned as number should return the deafult value
    b1 = jObj.get_value<bool>(a_number, true);
    EXPECT_EQ(b1, true);
    // get_value<bool> without default value for value assigned as string should return false
    b1 = jObj.get_value<bool>(not_number);
    EXPECT_EQ(b1, false);
}

TEST(MemberValue, as_type)
{
    jsonland::json_node s1("I am a string", string_t);
    jsonland::json_node b1(true);
    jsonland::json_node p1(nullptr);
    
    EXPECT_EQ(s1.dump(), "\"I am a string\"");
    EXPECT_EQ(s1.get_string(), "I am a string");
    EXPECT_EQ(s1.get<std::string>(), "I am a string");
    EXPECT_EQ(s1.get<std::string_view>(), "I am a string");
    EXPECT_EQ(s1.get_int<unsigned>(), 0);
    EXPECT_EQ(s1.get_int(17), 17);
    EXPECT_EQ(s1.get_int<int64_t>(), 0);
    EXPECT_EQ(s1.get<int64_t>(), 0);

    EXPECT_EQ(s1.get_float<long double>(), 0.0);
    EXPECT_EQ(s1.get_float(17.0), 17.0);
    EXPECT_EQ(s1.get_float<float>(), 0);
    EXPECT_EQ(s1.get<float>(), 0.0);
    EXPECT_EQ(s1.get<float>(), 0.0);

    EXPECT_EQ(s1.get_bool(), false);
    EXPECT_EQ(s1.get_bool(true), (true));
    EXPECT_EQ(s1.get<bool>(), false);
    EXPECT_EQ(s1.get<bool>(true), true);

    jsonland::json_node n1(1234567);
    EXPECT_EQ(n1.dump(), "1234567");
    EXPECT_EQ(n1.get_int<unsigned>(), 1234567);
    EXPECT_EQ(n1.get_int(17), 1234567);
    EXPECT_EQ(n1.get_int<int64_t>(), 1234567);
    EXPECT_EQ(n1.get<int64_t>(), 1234567);
    EXPECT_EQ(n1.get_float<long double>(), 1234567.0);
    EXPECT_EQ(n1.get_float(17.0), 1234567.0);
    EXPECT_EQ(n1.get_float<float>(), 1234567.0f);
    EXPECT_EQ(n1.get_float(), 1234567.0f);
    EXPECT_EQ(n1.get_double(), 1234567.0);
    EXPECT_EQ(n1.get<float>(), 1234567.0f);


    EXPECT_EQ(n1.get_string(), "");
    EXPECT_EQ(n1.get<std::string>(), "");
    EXPECT_EQ(n1.get<std::string_view>(), "");

}

TEST(MemberValue, get_as)
{
    {
        jsonland::json_node j_num(17);

        EXPECT_EQ(j_num.get<int>(), 17);
        EXPECT_EQ(j_num.get_as<int>(), 17);
        EXPECT_EQ(j_num.get_int(), 17);
        EXPECT_EQ(j_num.get_string(), ""sv);
        EXPECT_EQ(j_num.get_as<std::string_view>(), "17"sv);
    }

    {
        jsonland::json_node j_str_num("19");

        EXPECT_EQ(j_str_num.get<int>(), 0);
        EXPECT_EQ(j_str_num.get_as<int>(), 19);
        EXPECT_EQ(j_str_num.get_int(), 0);
        EXPECT_EQ(j_str_num.get_string(), std::string_view("19"));
        EXPECT_EQ(j_str_num.get_as<std::string_view>(), "19"sv);
    }
}

TEST(MemberValue, non_existing_in_const)
{
    {
        jsonland::json_node jo;
        jo["one"] = 1.0;

        const jsonland::json_node jo_copy(jo);

        EXPECT_EQ(jo_copy["one"].get_double(), 1.0);

        // getting a value from non existing member of an object should return the default value
        EXPECT_EQ(jo_copy["two"].get_double(17.19), 17.19);
        EXPECT_EQ(jo_copy["two"].get_int<int32_t>(17), 17);
        EXPECT_EQ(jo_copy["two"].get_string("17"), "17");
        EXPECT_EQ(jo_copy["two"].get_bool(), false);
        EXPECT_EQ(jo_copy["two"].get_bool(true), true);
        EXPECT_EQ(jo_copy["two"].get_null(), nullptr);

        // getting a value from non existing object member should be recursive:
        // getting a value from non existing member of an object accessed as an object should return the default value
        EXPECT_EQ(jo_copy["two"]["three"].get_double(23.29), 23.29);

        // getting a value from non existing member of an object accessed as an array should return the default value
        EXPECT_EQ(jo_copy["two"][0].get_double(23.29), 23.29);
    }
    {
        jsonland::json_node ja;
        ja.push_back(1.0);

        const jsonland::json_node ja_copy(ja);

        EXPECT_EQ(ja_copy[0].get_double(), 1.0);

        // getting a value from non existing member of an array should return the default value
        EXPECT_EQ(ja_copy[1].get_double(17.19), 17.19);
        EXPECT_EQ(ja_copy[1].get_int<int32_t>(17), 17);
        EXPECT_EQ(ja_copy[1].get_string("17"), "17");
        EXPECT_EQ(ja_copy[1].get_bool(), false);
        EXPECT_EQ(ja_copy[1].get_bool(true), true);
        EXPECT_EQ(ja_copy[1].get_null(), nullptr);

        // getting a value from non existing array member should be recursive:
        // getting a value from non existing member of an array accessed as an object should return the default value
        EXPECT_EQ(ja_copy[2]["three"].get_double(23.29), 23.29);

        // getting a value from non existing member of an array accessed as an array should return the default value
        EXPECT_EQ(ja_copy[2][0].get_double(23.29), 23.29);
    }
}
