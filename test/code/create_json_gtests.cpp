#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include <stdio.h>

using namespace jsonland;

TEST(Write, to_ostream)
{
    std::ostringstream oss;
    
    oss << json_node();
    EXPECT_STREQ(oss.str().c_str(), "null");
    oss.str("");
    
    oss << json_node(true);
    oss << ", ";
    oss << json_node(false);
    EXPECT_STREQ(oss.str().c_str(), "true, false");
    oss.str("");
    
    oss << json_node("Moti");
    oss << " ";
    oss << json_node("Shneor");
    EXPECT_STREQ(oss.str().c_str(), R"("Moti" "Shneor")");
    oss.str("");
    
    oss << json_node(7.1);
    oss << "+";
    oss  << json_node(5);
    oss << "=";
    oss  << json_node(12.1);
    EXPECT_STREQ(oss.str().c_str(), R"(7.099999999999999645+5=12.099999999999999645)");
    oss.str("");
    
    json_node ajn(jsonland::value_type::array_t);
    oss << ajn;
    EXPECT_STREQ(oss.str().c_str(), "[]");
    oss.str("");
    
    ajn.push_back(1);
    oss << ajn;
    EXPECT_STREQ(oss.str().c_str(), "[1]");
    oss.str("");
    
    ajn.push_back("Motish");
    ajn.push_back("Neor");
    oss << ajn;
    std::string m(oss.str().c_str());
    EXPECT_STREQ(oss.str().c_str(), R"([1,"Motish","Neor"])");
    oss.str("");

    json_node ojn(jsonland::value_type::object_t);
    oss << ojn;
    EXPECT_STREQ(oss.str().c_str(), "{}");
    oss.str("");
    
    ojn["Shmoti"] = "Neor";
    oss << ojn;
    EXPECT_STREQ(oss.str().c_str(), R"({"Shmoti":"Neor"})");
    oss.str("");

    ojn["array"] = std::move(ajn);
    oss << ojn;
    EXPECT_STREQ(oss.str().c_str(), R"({"Shmoti":"Neor","array":[1,"Motish","Neor"]})");
    oss.str("");
}

TEST(Create, one)
{
    json_node jn(jsonland::value_type::object_t);
    jn["one"] = "one";
    jn["two"] = 2;
    jn["three"] = true;
    jn["four"].push_back(jn["one"]);
    jn["four"].push_back(jn["two"]);
    jn["four"].push_back(jn["three"]);
}

TEST(Array, set_and_get)
{
    json_node jn(jsonland::value_type::array_t);
    jn.push_back(9);
    EXPECT_EQ(jn[0].get_int<int>(), 9);
    
    jn.push_back(0.1234);
    EXPECT_EQ(jn[0].get_int<int>(), 9);
    EXPECT_EQ(jn[1].get_float<double>(), 0.1234);
    
    json_node jn2(jsonland::value_type::array_t);
    jn2.push_back(0);
    jn2.push_back(1);
    jn2.push_back(2);
    
    int n = 0;
    for (auto i = jn2.begin(); i != jn2.end(); ++i)
    {
        int64_t the_int = i->get_int<int>();
        EXPECT_EQ(the_int, n++);
    }

}

TEST(SetValue, set_str)
{
    const char* some_str = "babushka";
    const char* some_other_str = "pirushki";
    
    json_node jn;
    jn = some_str;
    EXPECT_EQ(jn.get_string(), some_str)
                << "json_node.operator=(" << some_str << ").get_string()" << " should return "  << '"' << some_str << '"';
    jn = some_other_str;
    EXPECT_EQ(jn.get_string(), some_other_str)
                << "json_node.operator=(" << some_other_str << ").get_string()" << " should return "  << '"' << some_other_str << '"';
}

TEST(GetValue, bool_value)
{
    
    EXPECT_FALSE(json_node().get_bool())
                << "json_node().get_bool() should return " << "false";
    EXPECT_FALSE(json_node().get<bool>())
                << "json_node().get_bool() should return " << "false";
    EXPECT_TRUE(json_node().get_bool(true))
                << "json_node().get_bool(true) should return " << "true";
    EXPECT_TRUE(json_node().get<bool>(true))
                << "json_node().get_bool(true) should return " << "true";

    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).get_bool())
                << "json_node(jsonland::value_type::_bool).get_bool() should return " << "false";
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).get<bool>())
                << "json_node(jsonland::value_type::_bool).get_bool() should return " << "false";
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).get_bool(true))
                << "json_node(jsonland::value_type::_bool).get_bool(true) should return " << "false";
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).get<bool>(true))
                << "json_node(jsonland::value_type::_bool).get_bool(true) should return " << "false";

    EXPECT_FALSE(json_node(false).get_bool())
                << "json_node(" << "false" << ").get_bool() should return " << "false";
    EXPECT_FALSE(json_node(false).get<bool>())
                << "json_node(" << "false" << ").get_bool() should return " << "false";
    
    EXPECT_TRUE(json_node(true).get_bool())
                << "json_node(" << "true" << ").get_bool() should return " << "true";
    EXPECT_TRUE(json_node(true).get<bool>())
                << "json_node(" << "true" << ").get_bool() should return " << "true";
    
    EXPECT_FALSE(json_node(false).get_bool(true))
                << "json_node(" << "false" << ").get_bool(true) should return " << "false";
    EXPECT_FALSE(json_node(false).get<bool>(true))
                << "json_node(" << "false" << ").get_bool(true) should return " << "false";
    
    EXPECT_TRUE(json_node(true).get_bool(false))
                << "json_node(" << "true" << ").get_bool(false) should return " << "true";
    EXPECT_TRUE(json_node(true).get<bool>(false))
                << "json_node(" << "true" << ").get_bool(false) should return " << "true";
}

TEST(GetValue, integer_value)
{
    const int64_t int_num = 17.19;
    const int64_t another_int_number = 23.45;
    
    EXPECT_EQ(json_node(int_num).get_int<int>(), int_num)
                << "json_node(" << int_num << ").get_int() should return " << int_num;
    
    EXPECT_EQ(json_node(int_num).get_int(another_int_number), int_num)
                << "json_node(" << int_num << ").get_int(" << another_int_number << ") should return " << int_num;
    
    EXPECT_EQ(json_node(int_num).get_string(), "")
                << "json_node(" << int_num << ").get_string() should return " << '"' << '"';
    
    EXPECT_EQ(json_node(int_num).get_string("babushka"), "babushka")
                << "json_node(" << int_num << R"(.get_string("babushka"))" << " should return " << R"("babushka")";
    EXPECT_EQ(json_node(int_num).get<std::string_view>("babushka"), "babushka")
                << "json_node(" << int_num << R"(.get_string("babushka"))" << " should return " << R"("babushka")";

    EXPECT_EQ(json_node("babushka").get_int<int>(), 0);
    EXPECT_EQ(json_node("babushka").get<int>(), 0);

    EXPECT_EQ(json_node("babushka").get_int(190888), 190888);
    EXPECT_EQ(json_node("babushka").get<int>(190888), 190888);

}

TEST(GetValue, floating_point_value)
{
    const double fp_num = 17.19;
    const double another_fp_number = 23.45;
    
    EXPECT_EQ(json_node(fp_num).get_float<double>(), fp_num)
                << "json_node(" << fp_num << ").get_float() should return " << fp_num;
    EXPECT_EQ(json_node(fp_num).get_float<double>(another_fp_number), fp_num)
                << "json_node(" << fp_num << ").get_float(" << another_fp_number << ") should return " << fp_num;
    EXPECT_EQ(json_node(fp_num).get_string(), "")
                << "json_node(" << fp_num << ").get_string() should return " << '"' << '"';
    EXPECT_EQ(json_node(fp_num).get_string("babushka"), "babushka")
                << "json_node(" << fp_num << R"(.get_string("babushka"))" << " should return " << R"("babushka")";

    EXPECT_EQ(json_node("babushka").get_float<float>(), 0.0);
    EXPECT_EQ(json_node("babushka").get_float<double>(908.88), 908.88);
}

TEST(GetValue, str_value)
{
    const char* some_str = "babushka";
    const char* some_other_str = "pirushki";
    EXPECT_EQ(json_node(some_str).get_string(), some_str)
                << "json_node(" << some_str << ").get_string()" << " should return "  << '"' << some_str << '"';
    
    EXPECT_EQ(json_node(some_str).get_string(some_other_str), some_str)
                << "json_node(" << some_str << ").get_string(" << some_other_str << ")" << " should return "  << '"' << some_str << '"';

    EXPECT_EQ(json_node(some_str).get_float<float>(), 0.0)
                << "json_node(" << some_str << ").get_float()" << " should return 0.0";
    EXPECT_EQ(json_node(some_str).get_float<float>(456.87), 456.87f)
                << "json_node(" << some_str << ").get_float(456.87)" << " should return 456.87";

    json_node jn2(17);
    EXPECT_EQ(jn2.get_string(), "");
    EXPECT_EQ(jn2.get_string(some_str), some_str);
}
