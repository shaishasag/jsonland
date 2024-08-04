#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(TestConstruction, copy_ctor)
{
    
}

TEST(TestConstruction, obj_ctor_value_type)
{
    // check that type is set correctly to value_type_obj
    EXPECT_TRUE(json_node(jsonland::value_type::object_t).is_object()) << "json_node(value_type_obj).is_object() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(json_node(jsonland::value_type::object_t).num_elements(), 0) << "json_node(value_type_obj) should be empty";
    EXPECT_EQ(json_node(jsonland::value_type::object_t).num_elements(), 0) << "json_node(value_type_obj).as_obj() should be empty";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node(jsonland::value_type::object_t).is_array()) << "json_node(value_type_obj).is_array() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::object_t).is_string()) << "json_node(value_type_obj).is_string() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::object_t).is_number()) << "json_node(value_type_obj).is_number() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::object_t).is_bool()) << "json_node(value_type_obj).is_bool() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::object_t).is_null()) << "json_node(value_type_obj).is_null() should return false";
}

TEST(TestConstruction, array_ctor_value_type)
{
    // check that type is set correctly to value_type_array
    EXPECT_TRUE(json_node(jsonland::value_type::array_t).is_array()) << "json_node(value_type_array).is_array() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(json_node(jsonland::value_type::array_t).num_elements(), 0) << "json_node(value_type_array) should be empty";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node(jsonland::value_type::array_t).is_object()) << "json_node(value_type_array).is_object() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::array_t).is_string()) << "json_node(value_type_array).is_string() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::array_t).is_number()) << "json_node(value_type_array).is_number() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::array_t).is_bool()) << "json_node(value_type_array).is_bool() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::array_t).is_null()) << "json_node(value_type_array).is_null() should return false";
}

TEST(TestConstruction, str_ctor_with_value_type)
{
    // check that type is set correctly to value_type_str
    EXPECT_TRUE(json_node(jsonland::value_type::string_t).is_string()) << "json_node(value_type_str).is_string() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(json_node(jsonland::value_type::string_t).get_string("????"), ""sv) << "json_node(value_type_str).get_string('????') should return empty string";
    
    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node(jsonland::value_type::string_t).is_object()) << "json_node(value_type_str).is_object() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::string_t).is_array()) << "json_node(value_type_str).is_array() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::string_t).is_number()) << "json_node(value_type_str).is_number() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::string_t).is_bool()) << "json_node(value_type_str).is_bool() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::string_t).is_null()) << "json_node(value_type_str).is_null() should return false";
}

TEST(TestConstruction, str_ctor_with_value)
{
    // check that type is set correctly to value_type_str
    EXPECT_TRUE(json_node("babushka", string_t).is_string()) << R"(json_node("babushka").is_string() should return true)";

    // check that default value is initialized correctly
    EXPECT_EQ(json_node("babushka", string_t).get_string("????"), "babushka"sv) << R"(json_node("babushka", string_t
    EXPECT_FALSE(json_node("babushka", string_t).is_object()) << R"(json_node("babushka", string_t).is_object() should return false)";
    EXPECT_FALSE(json_node("babushka", string_t).is_array()) << R"(json_node("babushka", string_t).is_array() should return false)";
    EXPECT_FALSE(json_node("babushka", string_t).is_number()) << R"(json_node("babushka", string_t).is_number() should return false)";
    EXPECT_FALSE(json_node("babushka", string_t).is_bool()) << R"(json_node("babushka", string_t).is_bool() should return false)";
    EXPECT_FALSE(json_node("babushka", string_t).is_null()) << R"(json_node("babushka", string_t).is_null() should return false)";
}

TEST(TestConstruction, num_ctor_with_value_type)
{
    // check that type is set correctly to value_type_num
    EXPECT_TRUE(json_node(jsonland::value_type::number_t).is_number()) << "json_node(value_type_num).is_number() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(json_node(jsonland::value_type::number_t).get_float(45.6), 0.0) << "json_node(value_type_num).get_float(45.6) should return 0.0";
    EXPECT_EQ(json_node(jsonland::value_type::number_t).get_int(45), 0) << "json_node(value_type_num).get_int(45.6) should return 0";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node(jsonland::value_type::number_t).is_object()) << "json_node(value_type_num).is_object() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::number_t).is_array()) << "json_node(value_type_num).is_array() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::number_t).is_string()) << "json_node(value_type_num).is_string() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::number_t).is_bool()) << "json_node(value_type_num).is_bool() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::number_t).is_null()) << "json_node(value_type_num).is_null() should return false";
}

TEST(TestConstruction, num_ctor_with_value)
{
    // check that type is set correctly to value_type_num
    EXPECT_TRUE(json_node(17).is_number()) << "json_node(value_type_num).is_number() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(json_node(17.3).get_float<double>(), 17.3) << "json_node(17.3).get_float() should return 17.3";
    EXPECT_EQ(json_node(17.3).get_int<int>(), 17) << "json_node(17.3).get_int() should return 17";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node(17.3).is_object()) << "json_node(17.3).is_object() should return false";
    EXPECT_FALSE(json_node(17.3).is_array()) << "json_node(17.3).is_array() should return false";
    EXPECT_FALSE(json_node(17.3).is_string()) << "json_node(17.3).is_string() should return false";
    EXPECT_TRUE(json_node(17.3).is_number()) << "json_node(17.3).is_number() should return true";
    EXPECT_FALSE(json_node(17.3).is_bool()) << "json_node(17.3).is_bool() should return false";
    EXPECT_FALSE(json_node(17.3).is_null()) << "json_node(17.3).is_bool() should return false";
}

TEST(TestConstruction, bool_ctor_with_value_type)
{
    // check that type is set correctly to value_type_bool
    EXPECT_TRUE(json_node(jsonland::value_type::bool_t).is_bool()) << "json_node(value_type_bool).is_bool() should return true";
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).get_bool()) << "json_node(value_type_bool).get_bool() should return false";
    
    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).is_object()) << "json_node(value_type_bool).is_object() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).is_array()) << "json_node(value_type_bool).is_array() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).is_string()) << "json_node(value_type_bool).is_string() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).is_number()) << "json_node(value_type_bool).is_number() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::bool_t).is_null()) << "json_node(value_type_bool).is_null() should return false";
}
 
TEST(TestConstruction, bool_ctor_with_value)
{
    json_node bool_json_false(false);
    json_node bool_json_true(true);
    
    // check that type is set correctly to value_type_bool
    EXPECT_TRUE(bool_json_false.is_bool()) << "json_node(false).is_bool() should return true";
    EXPECT_TRUE(bool_json_true.is_bool()) << "json_node(true).is_bool() should return true";
    
    // check that default value is initialized correctly
    EXPECT_FALSE(bool_json_false.get_bool()) << "json_node(false).get_bool() should return false";
    EXPECT_TRUE(bool_json_true.get_bool()) << "json_node(true).get_bool() should return true";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(bool_json_false.is_object()) << "json_node(false).is_object() should return false";
    EXPECT_FALSE(bool_json_true.is_object()) << "json_node(true).is_object() should return false";
    EXPECT_FALSE(bool_json_false.is_array()) << "json_node(false).is_array() should return false";
    EXPECT_FALSE(bool_json_true.is_array()) << "json_node(true).is_array() should return false";
    EXPECT_FALSE(bool_json_false.is_string()) << "json_node(false).is_string() should return false";
    EXPECT_FALSE(bool_json_true.is_string()) << "json_node(true).is_string() should return false";
    EXPECT_FALSE(bool_json_false.is_number()) << "json_node(false).is_number() should return false";
    EXPECT_FALSE(bool_json_true.is_number()) << "json_node(true).is_number() should return false";
    EXPECT_FALSE(bool_json_false.is_null()) << "json_node(false).is_null() should return false";
    EXPECT_FALSE(bool_json_true.is_null()) << "json_node(true).is_null() should return false";
}

TEST(TestConstruction, null_ctor_with_value_type)
{
    // check that type is set correctly to value_type_null
    EXPECT_TRUE(json_node(jsonland::value_type::null_t).is_null()) << "json_node(value_type_null).is_null() should return true";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node(jsonland::value_type::null_t).is_object()) << "json_node(value_type_null).is_object() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::null_t).is_array()) << "json_node(value_type_null).is_array() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::null_t).is_string()) << "json_node(value_type_null).is_string() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::null_t).is_number()) << "json_node(value_type_null).is_number() should return false";
    EXPECT_FALSE(json_node(jsonland::value_type::null_t).is_bool()) << "json_node(value_type_null).is_bool() should return true";
}
 
TEST(TestConstruction, null_ctor_with_value)
{
    // check that type is set correctly to value_type_null
    EXPECT_TRUE(json_node(nullptr).is_null()) << "json_node(nullptr).is_null() should return true";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node(nullptr).is_object()) << "json_node(nullptr).is_object() should return false";
    EXPECT_FALSE(json_node(nullptr).is_array()) << "json_node(nullptr).is_array() should return false";
    EXPECT_FALSE(json_node(nullptr).is_string()) << "json_node(nullptr).is_string() should return false";
    EXPECT_FALSE(json_node(nullptr).is_number()) << "json_node(nullptr).is_number() should return false";
    EXPECT_FALSE(json_node(nullptr).is_bool()) << "json_node(nullptr).is_bool() should return false";
}

TEST(TestConstruction, default_ctor)
{
    // check that type is set correctly to value_type_null
    EXPECT_TRUE(json_node().is_null()) << "json_node().is_null() should return true";
    
    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(json_node().is_object()) << "json_node().is_object() should return false";
    EXPECT_FALSE(json_node().is_array()) << "json_node().is_array() should return false";
    EXPECT_FALSE(json_node().is_string()) << "json_node().is_string() should return false";
    EXPECT_FALSE(json_node().is_number()) << "json_node().is_number() should return false";
    EXPECT_FALSE(json_node().is_bool()) << "json_node().is_bool() should return false";
}

TEST(TestConstruction, ctor_with_string_and_type)
{
    json_node n1("123", number_t);
    EXPECT_EQ(n1.get_int<int32_t>(), 123);
    EXPECT_EQ(n1.get_float<double>(), 123.0);
    EXPECT_EQ(n1.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(n1.is_number());
    EXPECT_TRUE(n1.is_int());
    EXPECT_FALSE(n1.is_float());
    
    json_node n2("123.456", number_t);
    EXPECT_EQ(n2.get_int<int32_t>(), 123);
    EXPECT_EQ(n2.get_float<double>(), 123.456);
    EXPECT_EQ(n2.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(n2.is_number());
    EXPECT_FALSE(n2.is_int());
    EXPECT_TRUE(n2.is_float());
    
    json_node bT("true", bool_t);
    EXPECT_TRUE(bT.get_bool());
    EXPECT_EQ(bT.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(bT.is_bool());
    
    json_node bF("false", bool_t);
    EXPECT_FALSE(bF.get_bool());
    EXPECT_EQ(bF.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(bF.is_bool());
    
    // invalid value for bool, what to do?
    json_node bWTF("WTF", bool_t);
    EXPECT_FALSE(bWTF.get_bool());
    EXPECT_EQ(bWTF.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(bWTF.is_bool());
    
    json_node s1("123", string_t);  // same as calling json_node s1("123")
    EXPECT_EQ(s1.get_string(), "123");
    EXPECT_EQ(s1.get_int<int32_t>(), 0);  // not a number! return default value
    EXPECT_EQ(s1.get_float<double>(123.0), 123.0); // not a number! return default value
    EXPECT_EQ(s1.get_string("shoshana"), "123");
    EXPECT_FALSE(s1.is_number());
    EXPECT_FALSE(s1.is_int());
    EXPECT_FALSE(s1.is_float());
    
    // init array with a value, weird, should it work?
    json_node a1("123", array_t);
    EXPECT_TRUE(a1.is_array());
    EXPECT_EQ(a1.num_elements(), 0);
    EXPECT_EQ(a1.get_string("shoshana"), "shoshana");// not a string! return default value
    EXPECT_EQ(a1.get_int<int32_t>(), 0);  // not a number! return default value
    EXPECT_EQ(a1.get_float<double>(123.0), 123.0); // not a number! return default value
    EXPECT_FALSE(a1.is_number());
    EXPECT_FALSE(a1.is_int());
    EXPECT_FALSE(a1.is_float());
}
