#include <string>
#include <iostream>
#include <fstream>
#include <array>
#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include "fstring/fstring.h"
#include "2TypesTestsCommon.h"

using namespace jsonland;

template <typename T>
class AsTypedTests : public JsonTypedTest<T> {};

TYPED_TEST_SUITE(AsTypedTests, json_node_andJsOn);

TYPED_TEST(AsTypedTests, size_as)
{
    {
        typename TestFixture::JsonType jn; // default ctor, type is null_t
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 0);
    }
    {
        typename TestFixture::JsonType jn(true); // bool
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 1);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 0);
    }
    {
        typename TestFixture::JsonType jn(""); // empty string
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 0);
    }
    {
        typename TestFixture::JsonType jn("a string"); // string
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 8);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 0);
    }
    {
        typename TestFixture::JsonType jn(777.0); // number
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 1);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 0);
    }
    {
        typename TestFixture::JsonType jn(jsonland::array_t); // empty array
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 0);
    }
    {
        typename TestFixture::JsonType jn(jsonland::array_t); // array
        jn.push_back(1);
        jn.push_back(2);
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 2);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 0);
    }
    {
        typename TestFixture::JsonType jn(jsonland::object_t); // empty object
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 0);
    }
    {
        typename TestFixture::JsonType jn(jsonland::object_t); // object
        jn["one"] = 1;
        jn["two"] = 2;
        ASSERT_EQ(jn.size_as(jsonland::null_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::bool_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::string_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::number_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::array_t), 0);
        ASSERT_EQ(jn.size_as(jsonland::object_t), 2);
    }
}

TYPED_TEST(AsTypedTests, empty_as)
{
    {
        typename TestFixture::JsonType jn; // default ctor, type is null_t
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_TRUE(jn.empty_as(jsonland::bool_t));
        ASSERT_TRUE(jn.empty_as(jsonland::string_t));
        ASSERT_TRUE(jn.empty_as(jsonland::number_t));
        ASSERT_TRUE(jn.empty_as(jsonland::array_t));
        ASSERT_TRUE(jn.empty_as(jsonland::object_t));
    }
    {
        typename TestFixture::JsonType jn(true); // bool
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_FALSE(jn.empty_as(jsonland::bool_t));
        ASSERT_TRUE(jn.empty_as(jsonland::string_t));
        ASSERT_TRUE(jn.empty_as(jsonland::number_t));
        ASSERT_TRUE(jn.empty_as(jsonland::array_t));
        ASSERT_TRUE(jn.empty_as(jsonland::object_t));
    }
    {
        typename TestFixture::JsonType jn(""); // empty string
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_TRUE(jn.empty_as(jsonland::bool_t));
        ASSERT_TRUE(jn.empty_as(jsonland::string_t));
        ASSERT_TRUE(jn.empty_as(jsonland::number_t));
        ASSERT_TRUE(jn.empty_as(jsonland::array_t));
        ASSERT_TRUE(jn.empty_as(jsonland::object_t));
    }
    {
        typename TestFixture::JsonType jn("a string"); // string
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_TRUE(jn.empty_as(jsonland::bool_t));
        ASSERT_FALSE(jn.empty_as(jsonland::string_t));
        ASSERT_TRUE(jn.empty_as(jsonland::number_t));
        ASSERT_TRUE(jn.empty_as(jsonland::array_t));
        ASSERT_TRUE(jn.empty_as(jsonland::object_t));
    }
    {
        typename TestFixture::JsonType jn(777.0); // number
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_TRUE(jn.empty_as(jsonland::bool_t));
        ASSERT_TRUE(jn.empty_as(jsonland::string_t));
        ASSERT_FALSE(jn.empty_as(jsonland::number_t));
        ASSERT_TRUE(jn.empty_as(jsonland::array_t));
        ASSERT_TRUE(jn.empty_as(jsonland::object_t));
    }
    {
        typename TestFixture::JsonType jn(jsonland::array_t); // empty array
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_TRUE(jn.empty_as(jsonland::bool_t));
        ASSERT_TRUE(jn.empty_as(jsonland::string_t));
        ASSERT_TRUE(jn.empty_as(jsonland::number_t));
        ASSERT_TRUE(jn.empty_as(jsonland::array_t));
        ASSERT_TRUE(jn.empty_as(jsonland::object_t));
    }
    {
        typename TestFixture::JsonType jn(jsonland::array_t); // array
        jn.push_back(1);
        jn.push_back(2);
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_TRUE(jn.empty_as(jsonland::bool_t));
        ASSERT_TRUE(jn.empty_as(jsonland::string_t));
        ASSERT_TRUE(jn.empty_as(jsonland::number_t));
        ASSERT_FALSE(jn.empty_as(jsonland::array_t));
        ASSERT_TRUE(jn.empty_as(jsonland::object_t));
    }
    {
        typename TestFixture::JsonType jn(jsonland::object_t); // empty object
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_TRUE(jn.empty_as(jsonland::bool_t));
        ASSERT_TRUE(jn.empty_as(jsonland::string_t));
        ASSERT_TRUE(jn.empty_as(jsonland::number_t));
        ASSERT_TRUE(jn.empty_as(jsonland::array_t));
        ASSERT_TRUE(jn.empty_as(jsonland::object_t));
    }
    {
        typename TestFixture::JsonType jn(jsonland::object_t); // object
        jn["one"] = 1;
        jn["two"] = 2;
        ASSERT_TRUE(jn.empty_as(jsonland::null_t));
        ASSERT_TRUE(jn.empty_as(jsonland::bool_t));
        ASSERT_TRUE(jn.empty_as(jsonland::string_t));
        ASSERT_TRUE(jn.empty_as(jsonland::number_t));
        ASSERT_TRUE(jn.empty_as(jsonland::array_t));
        ASSERT_FALSE(jn.empty_as(jsonland::object_t));
    }
}

TYPED_TEST(AsTypedTests, contain_as)
{
    using type_and_name = std::pair<jsonland::value_type, fstr::fstr15>;
    std::array<type_and_name, 6> all_types
    {
        type_and_name{jsonland::null_t, "null"},
        type_and_name{jsonland::bool_t, "bool"},
        type_and_name{jsonland::string_t, "string"},
        type_and_name{jsonland::number_t, "number"},
        type_and_name{jsonland::array_t, "array"},
        type_and_name{jsonland::object_t, "object"}
    };
    typename TestFixture::JsonType jn(jsonland::object_t);
    jn["null"] = jsonland::null_t;
    jn["bool"] = true;
    jn["string"] = "a string";
    jn["number"] = 17;
    jn["array"] = jsonland::array_t;
    jn["object"] = jsonland::object_t;

    for (auto type : all_types)
    {
        ASSERT_FALSE(jn.contains_as("no such member", type.first));
        for (auto type2 : all_types)
        {
            if (type2.first != type.first)
            {
                ASSERT_FALSE(jn.contains_as(type.second, type2.first));
            }
            else
            {
                ASSERT_TRUE(jn.contains_as(type.second, type2.first));
            }
        }
    }
}
