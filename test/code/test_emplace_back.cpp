#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include "jsonland/JsOn.h"
#include "2TypesTestsCommon.h"

using namespace jsonland;

template <typename T>
class EmplaceBackTests : public JsonTypedTest<T> {};

TYPED_TEST_SUITE(EmplaceBackTests, JsonImplementations);

TYPED_TEST(EmplaceBackTests, EmplaceStringType)
{
    typename TestFixture::JsonType arr(array_t);
    
    auto& a_str_j = arr.emplace_back(string_t);
    EXPECT_TRUE(a_str_j.is_string());
    EXPECT_EQ(a_str_j.get_string(), ""sv);
}

TYPED_TEST(EmplaceBackTests, EmplaceStringValue)
{
    typename TestFixture::JsonType arr(array_t);
    
    auto& a_str_j = arr.emplace_back("bananarama"sv);
    EXPECT_TRUE(a_str_j.is_string());
    EXPECT_EQ(a_str_j.get_string(), "bananarama"sv);
}

TYPED_TEST(EmplaceBackTests, EmplaceNumberType)
{
    typename TestFixture::JsonType arr(array_t);
    
    auto& a_num_j = arr.emplace_back(number_t);
    EXPECT_TRUE(a_num_j.is_number());
    EXPECT_FALSE(a_num_j.is_int());
    EXPECT_TRUE(a_num_j.is_float());
    EXPECT_EQ(a_num_j.get_int(), 0);
    EXPECT_EQ(a_num_j.get_float(), 0.0);
}

TYPED_TEST(EmplaceBackTests, EmplaceIntValue)
{
    typename TestFixture::JsonType arr(array_t);
    
    auto& a_num_j = arr.emplace_back(10);
    EXPECT_TRUE(a_num_j.is_number());
    EXPECT_TRUE(a_num_j.is_int());
    EXPECT_FALSE(a_num_j.is_float());
    EXPECT_EQ(a_num_j.get_int(), 10);
    EXPECT_EQ(a_num_j.get_float(), 10.0);
}

TYPED_TEST(EmplaceBackTests, EmplaceFloatValue)
{
    typename TestFixture::JsonType arr(array_t);
    
    auto& a_num_j = arr.emplace_back(10.10);
    EXPECT_TRUE(a_num_j.is_number());
    EXPECT_TRUE(a_num_j.is_float());
    EXPECT_FALSE(a_num_j.is_int());
    EXPECT_EQ(a_num_j.get_int(), 10);
    EXPECT_EQ(a_num_j.get_float(), 10.10f);
    EXPECT_EQ(a_num_j.get_double(), 10.10);
}

TYPED_TEST(EmplaceBackTests, EmplaceBoolType)
{
    typename TestFixture::JsonType arr(array_t);
    
    auto& a_bool_j = arr.emplace_back(bool_t);
    EXPECT_TRUE(a_bool_j.is_bool());
    EXPECT_EQ(a_bool_j.get_bool(), false);
}

TYPED_TEST(EmplaceBackTests, EmplaceBoolValue)
{
    typename TestFixture::JsonType arr(array_t);
    
    {
        auto& a_false_j = arr.emplace_back(false);
        EXPECT_TRUE(a_false_j.is_bool());
        EXPECT_FALSE(a_false_j.get_bool());
    }

    {
        auto& a_true_j = arr.emplace_back(true);
        EXPECT_TRUE(a_true_j.is_bool());
        EXPECT_TRUE(a_true_j.get_bool());
    }
}

TYPED_TEST(EmplaceBackTests, EmplaceNullType)
{
    typename TestFixture::JsonType arr(array_t);
    
    {
        auto& a_null_j = arr.emplace_back(null_t);
        EXPECT_TRUE(a_null_j.is_null());
        EXPECT_EQ(a_null_j.get_null(), nullptr);
    }
    {
        auto& a_null_j = arr.emplace_back(nullptr);
        EXPECT_TRUE(a_null_j.is_null());
        EXPECT_EQ(a_null_j.get_null(), nullptr);
    }
}

TYPED_TEST(EmplaceBackTests, EmplaceArrayType)
{
    typename TestFixture::JsonType arr(array_t);
    
    auto& a_arr_j = arr.emplace_back(array_t);
    EXPECT_TRUE(a_arr_j.is_array());
    a_arr_j.emplace_back(nullptr);
    EXPECT_EQ(a_arr_j.size_as(array_t), 1);
}

TYPED_TEST(EmplaceBackTests, EmplaceObjectType)
{
    typename TestFixture::JsonType arr(array_t);
    
    auto& a_obj_j = arr.emplace_back(object_t);
    EXPECT_TRUE(a_obj_j.is_object());
    a_obj_j["banana"sv] = "rama"sv;
    EXPECT_EQ(a_obj_j.size_as(object_t), 1);
}
