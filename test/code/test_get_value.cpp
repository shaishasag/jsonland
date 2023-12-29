#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(GetValue, numbers)
{
    std::string_view a_number{"a number"};
    std::string_view not_number{"not number"};
    
    json_node jObj{object_t};
    jObj[a_number] = 17;
    jObj[not_number] = "not a number";
    
    int32_t i = jObj.get_value(a_number, 1990);
    EXPECT_EQ(i, 17);
    i = jObj.get_value<int32_t>(a_number);
    EXPECT_EQ(i, 17);

    int32_t j = jObj.get_value(not_number, 1990);
    EXPECT_EQ(j, 1990);
    j = jObj.get_value<int32_t>(not_number);
    EXPECT_EQ(j, 0);

    double d = jObj.get_value(a_number, 1990);
    EXPECT_EQ(d, 17.0);
    double e = jObj.get_value(not_number, 1990);
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
