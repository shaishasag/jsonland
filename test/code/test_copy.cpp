#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include "jsonland/JsOn.h"
#include "2TypesTestsCommon.h"

using namespace jsonland;

template <typename T>
class CopyTypedTests : public JsonTypedTest<T> {};

TYPED_TEST_SUITE(CopyTypedTests, JsonImplementations);

TYPED_TEST(CopyTypedTests, size_as)
{
    using j_t = typename TestFixture::JsonType;
    j_t jo(object_t);
    
    {
        j_t ja(array_t);
        ja.push_back(1);
        ja.push_back(2);
        ja.push_back(3);
        jo["numbers"] = std::move(ja);
    }
    
    int sum{0};
    if constexpr (std::is_same_v<j_t, json_node>)
    {
        for (auto& num_j : jo["numbers"])
        {
            sum += num_j.template get_int<int>();
        }
    }
    else if constexpr (std::is_same_v<j_t, JsOn>)
    {
        for (auto& num_j : jo["numbers"].array_range())
        {
            sum += num_j.get_int();
        }
    }
    
    EXPECT_EQ(sum, 6);
}
#if 0
TEST(Copy_JsOn, whole_jsons)
{
    JsOn jo(object_t);
    
    {
        JsOn& ja = jo["numbers"];
        ja.push_back(1);
        ja.push_back(2);
        ja.push_back(3);
    }
    
    int sum{0};
    for (auto& num_j : jo["numbers"].object_range())
    {
        sum += num_j.get_int<int>();
    }
    
    EXPECT_EQ(sum, 6);
}
#endif
