#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include <stdio.h>

using namespace jsonland;


TEST(TestWrite, to_ostream)
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
    
    oss << json_node(7.1) << "+" << json_node(5) << "=" << json_node(12.1);
    EXPECT_STREQ(oss.str().c_str(), R"(7.1+5=12.1)");
    oss.str("");
    
    json_node ajn(json_node::node_type_array);
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

    json_node ojn(json_node::node_type_obj);
    oss << ojn;
    EXPECT_STREQ(oss.str().c_str(), "{}");
    oss.str("");
    
    ojn["Shmoti"] = "Neor";
    oss << ojn;
    EXPECT_STREQ(oss.str().c_str(), R"({"Shmoti":"Neor"})");
    oss.str("");

    ojn["array"] = ajn;
    oss << ojn;
    EXPECT_STREQ(oss.str().c_str(), R"({"Shmoti":"Neor","array":[1,"Motish","Neor"]})");
    oss.str("");
}

#if 0
TEST(Create, one)
{
    json_node jn(json_node::node_type_obj);
    jn["one"] = "one";
    jn["two"] = 2;
    jn["three"] = true;
    jn["four"].push_back(jn["one"]);
    jn["four"].push_back(jn["two"]);
    jn["four"].push_back(jn["three"]);
    
    std::cerr << jn;
}
#endif

TEST(TestArray, set_and_get)
{
    json_node jn(json_node::node_type_array);
    jn.push_back(9);
    EXPECT_EQ(jn[0].as_int(), 9);
    jn.push_back(0.1234);
    EXPECT_EQ(jn[0].as_int(), 9);
    EXPECT_EQ(jn[1].as_double(), 0.1234);
    
    json_node jn2(json_node::node_type_array);
    jn2.push_back(0);
    jn2.push_back(1);
    jn2.push_back(2);
    
    int n = 0;
    for (auto i = jn2.begin(); i != jn2.end(); ++i)
    {
        int64_t the_int = i->as_int();
        EXPECT_EQ(the_int, n++);
    }

}

TEST(TestSetValue, set_str)
{
    const char* some_str = "babushka";
    const char* some_other_str = "pirushki";
    
    json_node jn;
    jn = some_str;
    EXPECT_STREQ(jn.as_string(), some_str)
                << "json_node.operator=(" << some_str << ").as_string()" << " should return "  << '"' << some_str << '"';
    jn = some_other_str;
    EXPECT_STREQ(jn.as_string(), some_other_str)
                << "json_node.operator=(" << some_other_str << ").as_string()" << " should return "  << '"' << some_other_str << '"';
}

TEST(TestGetValue, bool_value)
{
    
    EXPECT_FALSE(json_node().as_bool())
                << "json_node().as_bool() should return " << "false";
    EXPECT_TRUE(json_node().as_bool(true))
                << "json_node().as_bool(true) should return " << "true";
    
    EXPECT_FALSE(json_node(json_node::node_type_bool).as_bool())
                << "json_node(json_node::node_type_bool).as_bool() should return " << "false";
    EXPECT_FALSE(json_node(json_node::node_type_bool).as_bool(true))
                << "json_node(json_node::node_type_bool).as_bool(true) should return " << "false";
    
    EXPECT_FALSE(json_node(false).as_bool())
                << "json_node(" << "false" << ").as_bool() should return " << "false";
    EXPECT_TRUE(json_node(true).as_bool())
                << "json_node(" << "true" << ").as_bool() should return " << "true";
    EXPECT_FALSE(json_node(false).as_bool(true))
                << "json_node(" << "false" << ").as_bool(true) should return " << "false";
    EXPECT_TRUE(json_node(true).as_bool(false))
                << "json_node(" << "true" << ").as_bool(false) should return " << "true";
}

TEST(TestGetValue, integer_value)
{
    const int64_t int_num = 17.19;
    const int64_t another_int_number = 23.45;
    
    EXPECT_EQ(json_node(int_num).as_int(), int_num)
                << "json_node(" << int_num << ").as_double() should return " << int_num;
    EXPECT_EQ(json_node(int_num).as_int(another_int_number), int_num)
                << "json_node(" << int_num << ").as_double(" << another_int_number << ") should return " << int_num;
    EXPECT_STREQ(json_node(int_num).as_string(), "")
                << "json_node(" << int_num << ").as_string() should return " << '"' << '"';
    EXPECT_STREQ(json_node(int_num).as_string("babushka"), "babushka")
                << "json_node(" << int_num << R"(.as_string("babushka"))" << " should return " << R"("babushka")";

    EXPECT_EQ(json_node("babushka").as_int(), 0);
    EXPECT_EQ(json_node("babushka").as_int(190888), 190888);
}

TEST(TestGetValue, floating_point_value)
{
    const double fp_num = 17.19;
    const double another_fp_number = 23.45;
    
    EXPECT_EQ(json_node(fp_num).as_double(), fp_num)
                << "json_node(" << fp_num << ").as_double() should return " << fp_num;
    EXPECT_EQ(json_node(fp_num).as_double(another_fp_number), fp_num)
                << "json_node(" << fp_num << ").as_double(" << another_fp_number << ") should return " << fp_num;
    EXPECT_STREQ(json_node(fp_num).as_string(), "")
                << "json_node(" << fp_num << ").as_string() should return " << '"' << '"';
    EXPECT_STREQ(json_node(fp_num).as_string("babushka"), "babushka")
                << "json_node(" << fp_num << R"(.as_string("babushka"))" << " should return " << R"("babushka")";

    EXPECT_EQ(json_node("babushka").as_double(), 0.0);
    EXPECT_EQ(json_node("babushka").as_double(908.88), 908.88);
}

TEST(TestGetValue, str_value)
{
    const char* some_str = "babushka";
    const char* some_other_str = "pirushki";
    EXPECT_STREQ(json_node(some_str).as_string(), some_str)
                << "json_node(" << some_str << ").as_string()" << " should return "  << '"' << some_str << '"';
    
    EXPECT_STREQ(json_node(some_str).as_string(some_other_str), some_str)
                << "json_node(" << some_str << ").as_string(" << some_other_str << ")" << " should return "  << '"' << some_str << '"';

    EXPECT_EQ(json_node(some_str).as_double(), 0.0)
                << "json_node(" << some_str << ").as_double()" << " should return 0.0";
    EXPECT_EQ(json_node(some_str).as_double(456.87), 456.87)
                << "json_node(" << some_str << ").as_double(456.87)" << " should return 456.87";

    json_node jn2(17);
    EXPECT_STREQ(jn2.as_string(), "");
    EXPECT_STREQ(jn2.as_string(some_str), some_str);
}
