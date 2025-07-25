#include "gtest/gtest.h"
#include "2TypesTestsCommon.h"

using namespace jsonland;

template <typename T>
class ConstructorTypedTests : public JsonTypedTest<T> {};

TYPED_TEST_SUITE(ConstructorTypedTests, json_node_andJsOn);

TYPED_TEST(ConstructorTypedTests, obj_ctor_value_type)
{
    using j_t = typename TestFixture::JsonType;
   // check that type is set correctly to value_type_obj
    EXPECT_TRUE(j_t(jsonland::value_type::object_t).is_object()) << "j_t(value_type_obj).is_object() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(j_t(jsonland::value_type::object_t).num_elements(), 0) << "j_t(value_type_obj) should be empty";
    EXPECT_EQ(j_t(jsonland::value_type::object_t).num_elements(), 0) << "j_t(value_type_obj).as_obj() should be empty";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t(jsonland::value_type::object_t).is_array()) << "j_t(value_type_obj).is_array() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::object_t).is_string()) << "j_t(value_type_obj).is_string() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::object_t).is_number()) << "j_t(value_type_obj).is_number() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::object_t).is_bool()) << "j_t(value_type_obj).is_bool() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::object_t).is_null()) << "j_t(value_type_obj).is_null() should return false";
}

TYPED_TEST(ConstructorTypedTests, array_ctor_value_type)
{
    using j_t = typename TestFixture::JsonType;
    // check that type is set correctly to value_type_array
    EXPECT_TRUE(j_t(jsonland::value_type::array_t).is_array()) << "j_t(value_type_array).is_array() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(j_t(jsonland::value_type::array_t).num_elements(), 0) << "j_t(value_type_array) should be empty";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t(jsonland::value_type::array_t).is_object()) << "j_t(value_type_array).is_object() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::array_t).is_string()) << "j_t(value_type_array).is_string() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::array_t).is_number()) << "j_t(value_type_array).is_number() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::array_t).is_bool()) << "j_t(value_type_array).is_bool() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::array_t).is_null()) << "j_t(value_type_array).is_null() should return false";
}

TYPED_TEST(ConstructorTypedTests, str_ctor_with_value_type)
{
    using j_t = typename TestFixture::JsonType;
    // check that type is set correctly to value_type_str
    EXPECT_TRUE(j_t(jsonland::value_type::string_t).is_string()) << "j_t(value_type_str).is_string() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(j_t(jsonland::value_type::string_t).get_string("????"), ""sv) << "j_t(value_type_str).get_string('????') should return empty string";
    
    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t(jsonland::value_type::string_t).is_object()) << "j_t(value_type_str).is_object() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::string_t).is_array()) << "j_t(value_type_str).is_array() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::string_t).is_number()) << "j_t(value_type_str).is_number() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::string_t).is_bool()) << "j_t(value_type_str).is_bool() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::string_t).is_null()) << "j_t(value_type_str).is_null() should return false";
}

TYPED_TEST(ConstructorTypedTests, str_ctor_with_value)
{
    using j_t = typename TestFixture::JsonType;
    // check that type is set correctly to value_type_str
    EXPECT_TRUE(j_t("babushka", string_t).is_string()) << R"(j_t("babushka").is_string() should return true)";

    // check that default value is initialized correctly
    EXPECT_EQ(j_t("babushka", string_t).get_string("????"), "babushka"sv) << R"(j_t("babushka", string_t
    EXPECT_FALSE(j_t("babushka", string_t).is_object()) << R"(j_t("babushka", string_t).is_object() should return false)";
    EXPECT_FALSE(j_t("babushka", string_t).is_array()) << R"(j_t("babushka", string_t).is_array() should return false)";
    EXPECT_FALSE(j_t("babushka", string_t).is_number()) << R"(j_t("babushka", string_t).is_number() should return false)";
    EXPECT_FALSE(j_t("babushka", string_t).is_bool()) << R"(j_t("babushka", string_t).is_bool() should return false)";
    EXPECT_FALSE(j_t("babushka", string_t).is_null()) << R"(j_t("babushka", string_t).is_null() should return false)";
}

TYPED_TEST(ConstructorTypedTests, num_ctor_with_value_type)
{
    using j_t = typename TestFixture::JsonType;
    // check that type is set correctly to value_type_num
    EXPECT_TRUE(j_t(jsonland::value_type::number_t).is_number()) << "j_t(value_type_num).is_number() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(j_t(jsonland::value_type::number_t).get_float(45.6), 0.0) << "j_t(value_type_num).get_float(45.6) should return 0.0";
    EXPECT_EQ(j_t(jsonland::value_type::number_t).get_int(45), 0) << "j_t(value_type_num).get_int(45.6) should return 0";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t(jsonland::value_type::number_t).is_object()) << "j_t(value_type_num).is_object() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::number_t).is_array()) << "j_t(value_type_num).is_array() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::number_t).is_string()) << "j_t(value_type_num).is_string() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::number_t).is_bool()) << "j_t(value_type_num).is_bool() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::number_t).is_null()) << "j_t(value_type_num).is_null() should return false";
}

TYPED_TEST(ConstructorTypedTests, num_ctor_with_value)
{
    using j_t = typename TestFixture::JsonType;
    // check that type is set correctly to value_type_num
    EXPECT_TRUE(j_t(17).is_number()) << "j_t(value_type_num).is_number() should return true";

    // check that default value is initialized correctly
    EXPECT_EQ(j_t(17.3).get_float(), 17.3) << "j_t(17.3).get_float() should return 17.3";
    EXPECT_EQ(j_t(17.3).get_int(), 17) << "j_t(17.3).get_int() should return 17";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t(17.3).is_object()) << "j_t(17.3).is_object() should return false";
    EXPECT_FALSE(j_t(17.3).is_array()) << "j_t(17.3).is_array() should return false";
    EXPECT_FALSE(j_t(17.3).is_string()) << "j_t(17.3).is_string() should return false";
    EXPECT_TRUE(j_t(17.3).is_number()) << "j_t(17.3).is_number() should return true";
    EXPECT_FALSE(j_t(17.3).is_bool()) << "j_t(17.3).is_bool() should return false";
    EXPECT_FALSE(j_t(17.3).is_null()) << "j_t(17.3).is_bool() should return false";
}

TYPED_TEST(ConstructorTypedTests, bool_ctor_with_value_type)
{
    using j_t = typename TestFixture::JsonType;
    // check that type is set correctly to value_type_bool
    EXPECT_TRUE(j_t(jsonland::value_type::bool_t).is_bool()) << "j_t(value_type_bool).is_bool() should return true";
    EXPECT_FALSE(j_t(jsonland::value_type::bool_t).get_bool()) << "j_t(value_type_bool).get_bool() should return false";
    
    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t(jsonland::value_type::bool_t).is_object()) << "j_t(value_type_bool).is_object() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::bool_t).is_array()) << "j_t(value_type_bool).is_array() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::bool_t).is_string()) << "j_t(value_type_bool).is_string() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::bool_t).is_number()) << "j_t(value_type_bool).is_number() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::bool_t).is_null()) << "j_t(value_type_bool).is_null() should return false";
}
 
TYPED_TEST(ConstructorTypedTests, bool_ctor_with_value)
{
    using j_t = typename TestFixture::JsonType;
    j_t bool_json_false(false);
    j_t bool_json_true(true);
    
    // check that type is set correctly to value_type_bool
    EXPECT_TRUE(bool_json_false.is_bool()) << "j_t(false).is_bool() should return true";
    EXPECT_TRUE(bool_json_true.is_bool()) << "j_t(true).is_bool() should return true";
    
    // check that default value is initialized correctly
    EXPECT_FALSE(bool_json_false.get_bool()) << "j_t(false).get_bool() should return false";
    EXPECT_TRUE(bool_json_true.get_bool()) << "j_t(true).get_bool() should return true";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(bool_json_false.is_object()) << "j_t(false).is_object() should return false";
    EXPECT_FALSE(bool_json_true.is_object()) << "j_t(true).is_object() should return false";
    EXPECT_FALSE(bool_json_false.is_array()) << "j_t(false).is_array() should return false";
    EXPECT_FALSE(bool_json_true.is_array()) << "j_t(true).is_array() should return false";
    EXPECT_FALSE(bool_json_false.is_string()) << "j_t(false).is_string() should return false";
    EXPECT_FALSE(bool_json_true.is_string()) << "j_t(true).is_string() should return false";
    EXPECT_FALSE(bool_json_false.is_number()) << "j_t(false).is_number() should return false";
    EXPECT_FALSE(bool_json_true.is_number()) << "j_t(true).is_number() should return false";
    EXPECT_FALSE(bool_json_false.is_null()) << "j_t(false).is_null() should return false";
    EXPECT_FALSE(bool_json_true.is_null()) << "j_t(true).is_null() should return false";
}

TYPED_TEST(ConstructorTypedTests, null_ctor_with_value_type)
{
    using j_t = typename TestFixture::JsonType;
   // check that type is set correctly to value_type_null
    EXPECT_TRUE(j_t(jsonland::value_type::null_t).is_null()) << "j_t(value_type_null).is_null() should return true";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t(jsonland::value_type::null_t).is_object()) << "j_t(value_type_null).is_object() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::null_t).is_array()) << "j_t(value_type_null).is_array() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::null_t).is_string()) << "j_t(value_type_null).is_string() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::null_t).is_number()) << "j_t(value_type_null).is_number() should return false";
    EXPECT_FALSE(j_t(jsonland::value_type::null_t).is_bool()) << "j_t(value_type_null).is_bool() should return true";
}
 
TYPED_TEST(ConstructorTypedTests, null_ctor_with_value)
{
    using j_t = typename TestFixture::JsonType;
    // check that type is set correctly to value_type_null
    EXPECT_TRUE(j_t(nullptr).is_null()) << "j_t(nullptr).is_null() should return true";

    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t(nullptr).is_object()) << "j_t(nullptr).is_object() should return false";
    EXPECT_FALSE(j_t(nullptr).is_array()) << "j_t(nullptr).is_array() should return false";
    EXPECT_FALSE(j_t(nullptr).is_string()) << "j_t(nullptr).is_string() should return false";
    EXPECT_FALSE(j_t(nullptr).is_number()) << "j_t(nullptr).is_number() should return false";
    EXPECT_FALSE(j_t(nullptr).is_bool()) << "j_t(nullptr).is_bool() should return false";
}

TYPED_TEST(ConstructorTypedTests, default_ctor)
{
    using j_t = typename TestFixture::JsonType;
    // check that type is set correctly to value_type_null
    EXPECT_TRUE(j_t().is_null()) << "j_t().is_null() should return true";
    
    // check that type is not set incorrectly as another value_type
    EXPECT_FALSE(j_t().is_object()) << "j_t().is_object() should return false";
    EXPECT_FALSE(j_t().is_array()) << "j_t().is_array() should return false";
    EXPECT_FALSE(j_t().is_string()) << "j_t().is_string() should return false";
    EXPECT_FALSE(j_t().is_number()) << "j_t().is_number() should return false";
    EXPECT_FALSE(j_t().is_bool()) << "j_t().is_bool() should return false";
}

TYPED_TEST(ConstructorTypedTests, ctor_with_string_and_type)
{
    using j_t = typename TestFixture::JsonType;
    j_t n1("123", number_t);
    EXPECT_EQ(n1.get_int(), 123);
    EXPECT_EQ(n1.get_float(), 123.0);
    EXPECT_EQ(n1.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(n1.is_number());
    EXPECT_TRUE(n1.is_int());
    EXPECT_FALSE(n1.is_float());
    
    j_t n2("123.456", number_t);
    EXPECT_EQ(n2.get_int(), 123);
    EXPECT_EQ(n2.get_float(), 123.456);
    EXPECT_EQ(n2.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(n2.is_number());
    EXPECT_FALSE(n2.is_int());
    EXPECT_TRUE(n2.is_float());
    
    j_t bT("true", bool_t);
    EXPECT_TRUE(bT.get_bool());
    EXPECT_EQ(bT.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(bT.is_bool());
    
    j_t bF("false", bool_t);
    EXPECT_FALSE(bF.get_bool());
    EXPECT_EQ(bF.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(bF.is_bool());
    
    // invalid value for bool, what to do?
    j_t bWTF("WTF", bool_t);
    EXPECT_FALSE(bWTF.get_bool());
    EXPECT_EQ(bWTF.get_string("shoshana"), "shoshana");  // not a string! so return default value
    EXPECT_TRUE(bWTF.is_bool());
    
    j_t s1("123", string_t);  // same as calling j_t s1("123")
    EXPECT_EQ(s1.get_string(), "123");
    EXPECT_EQ(s1.get_int(), 0);  // not a number! return default value
    EXPECT_EQ(s1.get_float(123.0), 123.0); // not a number! return default value
    EXPECT_EQ(s1.get_string("shoshana"), "123");
    EXPECT_FALSE(s1.is_number());
    EXPECT_FALSE(s1.is_int());
    EXPECT_FALSE(s1.is_float());
    
    // init array with a value, weird, should it work?
    j_t a1("123", array_t);
    EXPECT_TRUE(a1.is_array());
    EXPECT_EQ(a1.num_elements(), 0);
    EXPECT_EQ(a1.get_string("shoshana"), "shoshana");// not a string! return default value
    EXPECT_EQ(a1.get_int(), 0);  // not a number! return default value
    EXPECT_EQ(a1.get_float(123.0), 123.0); // not a number! return default value
    EXPECT_FALSE(a1.is_number());
    EXPECT_FALSE(a1.is_int());
    EXPECT_FALSE(a1.is_float());
}

TYPED_TEST(ConstructorTypedTests, ctor_enum)
{
    using j_t = typename TestFixture::JsonType;
    {
        enum some_enum
        {
            nothing,
            something,
            everything
        };

        {
            j_t nothing_j(nothing);
            EXPECT_TRUE(nothing_j.is_int());
            EXPECT_EQ(nothing_j.get_int(), static_cast<int>(nothing));
        }

        {
            j_t something_j(something);
            EXPECT_TRUE(something_j.is_int());
            EXPECT_EQ(something_j.get_int(), static_cast<int>(something));
        }
        {
            j_t everything_j(everything);
            EXPECT_TRUE(everything_j.is_int());
            EXPECT_EQ(everything_j.get_int(), static_cast<int>(everything));
        }
    }
    {
        enum some_enum_typed : int64_t
        {
            nothing,
            something = 3'147'483'647,  // too big for int32_t
            everything = -3'147'483'648,// too small for int32_t
        };

        {
            j_t nothing_j(nothing);
            EXPECT_TRUE(nothing_j.is_int());
            EXPECT_EQ(nothing_j.get_int(), static_cast<int64_t>(nothing));
        }

        {
            j_t something_j(something);
            EXPECT_TRUE(something_j.is_int());
            EXPECT_EQ(something_j.get_int(), static_cast<int64_t>(something));
        }
        {
            j_t everything_j(everything);
            EXPECT_TRUE(everything_j.is_int());
            EXPECT_EQ(everything_j.get_int(), static_cast<int64_t>(everything));
        }
    }
    {
        enum class some_enum_class
        {
            nothing,
            something,
            everything
        };

        {
            j_t nothing_j(some_enum_class::nothing);
            EXPECT_TRUE(nothing_j.is_int());
            EXPECT_EQ(nothing_j.get_int(), static_cast<int>(some_enum_class::nothing));
        }

        {
            j_t something_j(some_enum_class::something);
            EXPECT_TRUE(something_j.is_int());
            EXPECT_EQ(something_j.get_int(), static_cast<int>(some_enum_class::something));
        }
        {
            j_t everything_j(some_enum_class::everything);
            EXPECT_TRUE(everything_j.is_int());
            EXPECT_EQ(everything_j.get_int(), static_cast<int>(some_enum_class::everything));
        }
    }

    {   // curious case of enum type bool
        enum class bool_enum_class : bool
        {
            its_false,
            its_true,
        };

        {
            j_t false_j(bool_enum_class::its_false);
            EXPECT_TRUE(false_j.is_bool());
            EXPECT_EQ(false_j.get_bool(), static_cast<bool>(bool_enum_class::its_false));
            EXPECT_EQ(false_j.get_int(), 0);
        }

        {
            j_t true_j(bool_enum_class::its_true);
            EXPECT_TRUE(true_j.is_bool());
            EXPECT_EQ(true_j.get_bool(), static_cast<bool>(bool_enum_class::its_true));
            EXPECT_EQ(true_j.get_int(), 1);
        }
    }
}
