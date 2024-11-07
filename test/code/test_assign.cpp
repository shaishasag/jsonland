#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include <string_view>

using namespace jsonland;

TEST(Assign, repeated_rotating_assign)
{
    std::string_view a_sv = "lu lu lin"sv;
    std::string a_str(a_sv);

    json_node jn_sources[6]{json_node("Margot Robbie"), 19.19f, json_node(false), json_node(nullptr), json_node(true), json_node(jsonland::value_type::array_t)};
    
    json_node obj(jsonland::value_type::object_t);
    obj["one"] = 3.1415;
    obj["two"] = true;
    obj["three"] = json_node("234.567", jsonland::value_type::number_t);
    
    json_node a(obj["three"]);
    
    json_node array(jsonland::value_type::array_t);
    array.push_back(json_node(1.2));
    array.push_back(json_node(true));
    array.push_back(obj["one"]);
    array.push_back(obj["two"]);
    array.push_back(obj["three"]);

    json_node jn_targets[7];
    
    int i = 0;
    while (i < 36)
    {
        int target = i++ % 7;
        jn_targets[target] = jn_sources[0];
        EXPECT_TRUE(jn_targets[target].is_string()) << R"(json_node = json_node("Margot Robbie"), is_string() should return true)";
        EXPECT_EQ(jn_targets[target].as_string_view(), "Margot Robbie"sv) <<  R"(json_node = json_node("Margot Robbie"),  get_string() should return "Margot Robbie")";
        
        target = i++ % 7;
        jn_targets[target] = jn_sources[1];
        EXPECT_TRUE(jn_targets[target].is_number()) << "json_node = json_node(19.19), is_number() should return true";
        EXPECT_EQ(jn_targets[target].get_float<float>(), 19.19f) << "json_node = json_node(19.19), get_float() should return 19.19";
        
        target = i++ % 7;
        jn_targets[target] = jn_sources[2];
        EXPECT_TRUE(jn_targets[target].is_bool()) << "json_node = json_node(false), is_bool() should return true";
        EXPECT_FALSE(jn_targets[target].get_bool()) << "json_node = json_node(false), get_bool() should return false";
        
        target = i++ % 7;
       jn_targets[target] = jn_sources[3];
        // check that type is set correctly to value_type_null
        EXPECT_TRUE(jn_targets[target].is_null()) << "json_node = json_node(nullptr), is_null() should return true";
        
        target = i++ % 7;
        jn_targets[target] = jn_sources[4];
        EXPECT_TRUE(jn_targets[target].is_bool()) << "json_node = json_node(true), is_bool() should return true";
        EXPECT_TRUE(jn_targets[target].get_bool()) << "json_node = json_node(true), get_bool() should return true";
    }
}

TEST(Assign, repeated_assign_constants)
{
    json_node jn;
    
    for (int i = 0; i < 3; ++i)
    {
        jn = 1.1;
        EXPECT_TRUE(jn.is_number()) << "json_node = 1.1, is_number() should return true";
        EXPECT_EQ(jn.get_float<double>(), 1.1) << "json_node = 1.1, get_float() should return 1.1";
        
        jn = false;
        EXPECT_TRUE(jn.is_bool()) << "json_node = false, is_bool() should return true";
        EXPECT_FALSE(jn.get_bool()) << "json_node = false, get_bool() should return false";
        
        jn = "Bimbatron";
        EXPECT_TRUE(jn.is_string()) << R"(json_node = "Bimbatron", is_string() should return true)";
        EXPECT_EQ(jn.get_string(), "Bimbatron") <<  R"(json_node = "Bimbatron",  get_string() should return "Bimbatron")";

        jn = nullptr;
        // check that type is set correctly to value_type_null
        EXPECT_TRUE(jn.is_null()) << "json_node = nullptr, is_null() should return true";

        jn = true;
        EXPECT_TRUE(jn.is_bool()) << "json_node = true, is_bool() should return true";
        EXPECT_TRUE(jn.get_bool()) << "json_node = true, get_bool() should return true";
    }
}


TEST(Assign, str_value)
{
    json_node jn_str(178);
    jn_str = "Shmulik";

    // check that type is set correctly to value_type_str
    EXPECT_TRUE(jn_str.is_string()) << R"(jn_str = "Shmulik", should return true)";

    // check that default value is initialized correctly
    EXPECT_EQ(jn_str.get_string("????"), "Shmulik") << R"(jn_str = "Shmulik", get_string('????') should return "Shmulik")";

    EXPECT_FALSE(jn_str.is_object()) << R"(jn_str = "Shmulik", is_object() should return false)";
    EXPECT_FALSE(jn_str.is_array()) << R"(jn_str = "Shmulik", is_array() should return false)";
    EXPECT_FALSE(jn_str.is_number()) << R"(jn_str = "Shmulik", is_number() should return false)";
    EXPECT_FALSE(jn_str.is_bool()) << R"(jn_str = "Shmulik", is_bool() should return false)";
    EXPECT_FALSE(jn_str.is_null()) << R"(jn_str = "Shmulik", is_null() should return false)";
}

TEST(Assign, num_value)
{
    json_node jn17("kombucha");
    jn17 = 17.3;
    
    // check that type is set correctly to value_type_num
    EXPECT_TRUE(jn17.is_number()) << "json_node = 17.3, is_number() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(jn17.get_float<long double>(), 17.3) << "json_node = 17.3, get_float() should return 17.3";
    EXPECT_EQ(jn17.get_int<int>(), 17) << "json_node = 17.3, get_int() should return 17";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(jn17.is_object()) << "json_node = 17.3, is_object() should return false";
    EXPECT_FALSE(jn17.is_array()) << "json_node = 17.3, is_array() should return false";
    EXPECT_FALSE(jn17.is_string()) << "json_node = 17.3, is_string() should return false";
    EXPECT_FALSE(jn17.is_bool()) << "json_node = 17.3, is_bool() should return false";
    EXPECT_FALSE(jn17.is_null()) << "json_node = 17.3, is_bool() should return false";
}

TEST(Assign, bool_value)
{
    json_node bool_json_false("bonanza");
    bool_json_false = false;
    json_node bool_json_true("prognanza");
    bool_json_true = true;
    
    // check that type is set correctly to value_type_bool
    EXPECT_TRUE(bool_json_false.is_bool()) << "json_node = false, is_bool() should return true";
    EXPECT_TRUE(bool_json_true.is_bool()) << "json_node = true, is_bool() should return true";
    
    // check that default value is initialized correctly
    EXPECT_FALSE(bool_json_false.get_bool()) << "json_node = false, get_bool() should return false";
    EXPECT_TRUE(bool_json_true.get_bool()) << "json_node = true, get_bool() should return true";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(bool_json_false.is_object()) << "json_node = false, is_object() should return false";
    EXPECT_FALSE(bool_json_true.is_object()) << "json_node = true, is_object() should return false";
    EXPECT_FALSE(bool_json_false.is_array()) << "json_node = false, is_array() should return false";
    EXPECT_FALSE(bool_json_true.is_array()) << "json_node = true, is_array() should return false";
    EXPECT_FALSE(bool_json_false.is_string()) << "json_node = false, is_string() should return false";
    EXPECT_FALSE(bool_json_true.is_string()) << "json_node = true, is_string() should return false";
    EXPECT_FALSE(bool_json_false.is_number()) << "json_node = false, is_number() should return false";
    EXPECT_FALSE(bool_json_true.is_number()) << "json_node = true, is_number() should return false";
    EXPECT_FALSE(bool_json_false.is_null()) << "json_node = false, is_null() should return false";
    EXPECT_FALSE(bool_json_true.is_null()) << "json_node = true, is_null() should return false";
}

TEST(Assign, null_value)
{
    json_node jn_null(12345);
    jn_null = nullptr;
    
    // check that type is set correctly to value_type_null
    EXPECT_TRUE(jn_null.is_null()) << "json_node = nullptr, is_null() should return true";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(jn_null.is_object()) << "json_node = nullptr, is_object() should return false";
    EXPECT_FALSE(jn_null.is_array()) << "json_node = nullptr, is_array() should return false";
    EXPECT_FALSE(jn_null.is_string()) << "json_node = nullptr, is_string() should return false";
    EXPECT_FALSE(jn_null.is_number()) << "json_node = nullptr, is_number() should return false";
    EXPECT_FALSE(jn_null.is_bool()) << "json_node = nullptr, is_bool() should return false";
}

TEST(Assign, move_assign_array)
{
    // moving array should not allocate new memory
    
    jsonland::json_node j;
    j.push_back(true);
    j.push_back(false);
    j.push_back("mamaliga");
    j.push_back(987);
    EXPECT_TRUE(j.is_array());
    EXPECT_EQ(j.num_elements(), 4);
    EXPECT_TRUE(j[0].is_bool());
    EXPECT_TRUE(j[1].is_bool());
    EXPECT_TRUE(j[2].is_string());
    EXPECT_TRUE(j[3].is_number());

    jsonland::json_node moved_j;
    moved_j = std::move(j);
    
    EXPECT_TRUE(j.is_array());
    EXPECT_EQ(j.num_elements(), 0);

    EXPECT_TRUE(moved_j.is_array());
    EXPECT_EQ(moved_j.num_elements(), 4);
    EXPECT_TRUE(moved_j[0].is_bool());
    EXPECT_TRUE(moved_j[1].is_bool());
    EXPECT_TRUE(moved_j[2].is_string());
    EXPECT_TRUE(moved_j[3].is_number());
}


TEST(Assign, move_assign_object)
{
    // moving object should not allocate new memory
    
    jsonland::json_node j;
    j["1st"] = true;
    j["2nd"] = false;
    j["3rd"] = "mamaliga";
    j["4th"] = 987;
    j["5th"] = jsonland::json_node(jsonland::object_t);
    j["5th"]["sub1"] = "Matityahu";
    j["5th"]["sub2"] = 175.009;

    EXPECT_TRUE(j.is_object());
    EXPECT_EQ(j.num_elements(), 5);
    EXPECT_TRUE(j["1st"].is_bool());
    EXPECT_TRUE(j["2nd"].is_bool());
    EXPECT_TRUE(j["3rd"].is_string());
    EXPECT_TRUE(j["4th"].is_number());
    EXPECT_TRUE(j["5th"].is_object());
    EXPECT_EQ(j["5th"].num_elements(), 2);
    EXPECT_TRUE(j["5th"]["sub1"].is_string());
    EXPECT_TRUE(j["5th"]["sub2"].is_number());

    jsonland::json_node moved_j;
    moved_j = std::move(j);
    
    EXPECT_TRUE(j.is_object());
    EXPECT_EQ(j.num_elements(), 0);

    EXPECT_TRUE(moved_j.is_object());
    EXPECT_EQ(moved_j.num_elements(), 5);
    EXPECT_TRUE(moved_j["1st"].is_bool());
    EXPECT_TRUE(moved_j["2nd"].is_bool());
    EXPECT_TRUE(moved_j["3rd"].is_string());
    EXPECT_TRUE(moved_j["4th"].is_number());
    EXPECT_TRUE(moved_j["5th"].is_object());
    EXPECT_EQ(moved_j["5th"].num_elements(), 2);
    EXPECT_TRUE(moved_j["5th"]["sub1"].is_string());
    EXPECT_TRUE(moved_j["5th"]["sub2"].is_number());
}

TEST(Assign, mixed_types)
{
    jsonland::json_node obj(jsonland::object_t);
    obj["1"] = 1;
    
    jsonland::json_node arr1(jsonland::array_t);
    arr1.push_back(1);
    arr1.push_back(2);
    
    obj["2"] = arr1;
    EXPECT_EQ(obj["1"], 1);
    EXPECT_TRUE(obj["2"].is_array());
    EXPECT_EQ(obj["2"].num_elements(), 2);
    EXPECT_EQ(obj["2"][0], 1);
    EXPECT_EQ(obj["2"][1], 2);
    
    jsonland::json_node arr2(jsonland::array_t);
    arr2.push_back(3);
    arr2.push_back(4);
    arr2.push_back(5);
    obj["3"] = std::move(arr2);
}

TEST(Assign, rvalue)
{
    jsonland::json_node arr1(jsonland::array_t);
    std::string_view sv("sv");
    arr1.push_back(sv);
}
