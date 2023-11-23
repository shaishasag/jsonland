#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include <string_view>

using namespace jsonland;

TEST(TestAssign, repeated_rotating_assign)
{
    std::string_view a_sv = "lu lu lin"sv;
    std::string a_str(a_sv);

    json_node jn_sources[6]{json_node("Margot Robbie"), 19.19, json_node(false), json_node(nullptr), json_node(true), json_node(jsonland::node_type::array_t)};
    
    json_node obj(jsonland::node_type::object_t);
    obj["one"] = 3.1415;
    obj["two"] = true;
    obj["three"] = json_node("234.567", jsonland::node_type::number_t);
    
    json_node a = obj["three"];
    
    json_node array(jsonland::node_type::array_t);
    array.push_back(json_node(1.2));
    array.push_back(json_node(true));
    array.push_back(obj["one"]);
    array.push_back(obj["two"]);
    array.push_back(obj["three"]);

    json_node jn_targets[7]{};
    
    int i = 0;
    while (i < 36)
    {
        int target = i++ % 7;
        jn_targets[target] = jn_sources[0];
        EXPECT_TRUE(jn_targets[target].is_string()) << R"(json_node = json_node("Margot Robbie"), is_string() should return true)";
        EXPECT_EQ(jn_targets[target].as_string_view(), "Margot Robbie"sv) <<  R"(json_node = json_node("Margot Robbie"),  as_string() should return "Margot Robbie")";
        
        target = i++ % 7;
        jn_targets[target] = jn_sources[1];
        EXPECT_TRUE(jn_targets[target].is_num()) << "json_node = json_node(19.19), is_num() should return true";
        EXPECT_EQ(jn_targets[target].as_double(), 19.19) << "json_node = json_node(19.19), as_double() should return 19.19";
        
        target = i++ % 7;
        jn_targets[target] = jn_sources[2];
        EXPECT_TRUE(jn_targets[target].is_bool()) << "json_node = json_node(false), is_bool() should return true";
        EXPECT_FALSE(jn_targets[target].as_bool()) << "json_node = json_node(false), as_bool() should return false";
        
        target = i++ % 7;
       jn_targets[target] = jn_sources[3];
        // check that type is set correctly to node_type_null
        EXPECT_TRUE(jn_targets[target].is_null()) << "json_node = json_node(nullptr), is_null() should return true";
        
        target = i++ % 7;
        jn_targets[target] = jn_sources[4];
        EXPECT_TRUE(jn_targets[target].is_bool()) << "json_node = json_node(true), is_bool() should return true";
        EXPECT_TRUE(jn_targets[target].as_bool()) << "json_node = json_node(true), as_bool() should return true";
    }
}

TEST(TestAssign, repeated_assign_constants)
{
    json_node jn;
    
    for (int i = 0; i < 3; ++i)
    {
        jn = 1.1;
        EXPECT_TRUE(jn.is_num()) << "json_node = 1.1, is_num() should return true";
        EXPECT_EQ(jn.as_double(), 1.1) << "json_node = 1.1, as_double() should return 1.1";
        
        jn = false;
        EXPECT_TRUE(jn.is_bool()) << "json_node = false, is_bool() should return true";
        EXPECT_FALSE(jn.as_bool()) << "json_node = false, as_bool() should return false";
        
        jn = "Bimbatron";
        EXPECT_TRUE(jn.is_string()) << R"(json_node = "Bimbatron", is_string() should return true)";
        EXPECT_EQ(jn.as_string(), "Bimbatron") <<  R"(json_node = "Bimbatron",  as_string() should return "Bimbatron")";

        jn = nullptr;
        // check that type is set correctly to node_type_null
        EXPECT_TRUE(jn.is_null()) << "json_node = nullptr, is_null() should return true";

        jn = true;
        EXPECT_TRUE(jn.is_bool()) << "json_node = true, is_bool() should return true";
        EXPECT_TRUE(jn.as_bool()) << "json_node = true, as_bool() should return true";
    }
}


TEST(TestAssign, str_value)
{
    json_node jn_str(178);
    jn_str = "Shmulik";

    // check that type is set correctly to node_type_str
    EXPECT_TRUE(jn_str.is_string()) << R"(jn_str = "Shmulik", should return true)";

    // check that default value is initialized correctly
    EXPECT_EQ(jn_str.as_string("????"), "Shmulik") << R"(jn_str = "Shmulik", as_string('????') should return "Shmulik")";

    EXPECT_FALSE(jn_str.is_object()) << R"(jn_str = "Shmulik", is_object() should return false)";
    EXPECT_FALSE(jn_str.is_array()) << R"(jn_str = "Shmulik", is_array() should return false)";
    EXPECT_FALSE(jn_str.is_num()) << R"(jn_str = "Shmulik", is_num() should return false)";
    EXPECT_FALSE(jn_str.is_bool()) << R"(jn_str = "Shmulik", is_bool() should return false)";
    EXPECT_FALSE(jn_str.is_null()) << R"(jn_str = "Shmulik", is_null() should return false)";
}

TEST(TestAssign, num_value)
{
    json_node jn17("kombucha");
    jn17 = 17.3;
    
    // check that type is set correctly to node_type_num
    EXPECT_TRUE(jn17.is_num()) << "json_node = 17.3, is_num() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(jn17.as_double(), 17.3) << "json_node = 17.3, as_double() should return 17.3";
    EXPECT_EQ(jn17.as_int<int>(), 17) << "json_node = 17.3, as_int() should return 17";

    // check that type is not set incorrectly as another node_type
    EXPECT_FALSE(jn17.is_object()) << "json_node = 17.3, is_object() should return false";
    EXPECT_FALSE(jn17.is_array()) << "json_node = 17.3, is_array() should return false";
    EXPECT_FALSE(jn17.is_string()) << "json_node = 17.3, is_string() should return false";
    EXPECT_FALSE(jn17.is_bool()) << "json_node = 17.3, is_bool() should return false";
    EXPECT_FALSE(jn17.is_null()) << "json_node = 17.3, is_bool() should return false";
}

TEST(TestAssign, bool_value)
{
    json_node bool_json_false("bonanza");
    bool_json_false = false;
    json_node bool_json_true("prognanza");
    bool_json_true = true;
    
    // check that type is set correctly to node_type_bool
    EXPECT_TRUE(bool_json_false.is_bool()) << "json_node = false, is_bool() should return true";
    EXPECT_TRUE(bool_json_true.is_bool()) << "json_node = true, is_bool() should return true";
    
    // check that default value is initialized correctly
    EXPECT_FALSE(bool_json_false.as_bool()) << "json_node = false, as_bool() should return false";
    EXPECT_TRUE(bool_json_true.as_bool()) << "json_node = true, as_bool() should return true";

    // check that type is not set incorrectly as another node_type
    EXPECT_FALSE(bool_json_false.is_object()) << "json_node = false, is_object() should return false";
    EXPECT_FALSE(bool_json_true.is_object()) << "json_node = true, is_object() should return false";
    EXPECT_FALSE(bool_json_false.is_array()) << "json_node = false, is_array() should return false";
    EXPECT_FALSE(bool_json_true.is_array()) << "json_node = true, is_array() should return false";
    EXPECT_FALSE(bool_json_false.is_string()) << "json_node = false, is_string() should return false";
    EXPECT_FALSE(bool_json_true.is_string()) << "json_node = true, is_string() should return false";
    EXPECT_FALSE(bool_json_false.is_num()) << "json_node = false, is_num() should return false";
    EXPECT_FALSE(bool_json_true.is_num()) << "json_node = true, is_num() should return false";
    EXPECT_FALSE(bool_json_false.is_null()) << "json_node = false, is_null() should return false";
    EXPECT_FALSE(bool_json_true.is_null()) << "json_node = true, is_null() should return false";
}

TEST(TestAssign, null_value)
{
    json_node jn_null(12345);
    jn_null = nullptr;
    
    // check that type is set correctly to node_type_null
    EXPECT_TRUE(jn_null.is_null()) << "json_node = nullptr, is_null() should return true";

    // check that type is not set incorrectly as another node_type
    EXPECT_FALSE(jn_null.is_object()) << "json_node = nullptr, is_object() should return false";
    EXPECT_FALSE(jn_null.is_array()) << "json_node = nullptr, is_array() should return false";
    EXPECT_FALSE(jn_null.is_string()) << "json_node = nullptr, is_string() should return false";
    EXPECT_FALSE(jn_null.is_num()) << "json_node = nullptr, is_num() should return false";
    EXPECT_FALSE(jn_null.is_bool()) << "json_node = nullptr, is_bool() should return false";
}
