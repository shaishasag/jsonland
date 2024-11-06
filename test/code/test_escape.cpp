#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include <iostream>

using namespace jsonland;

TEST(Escape, backslashes)
{
    json_node jo(object_t);

    jo["win_path"] = "c:\\something\\something";

    std::string take_a_dump = jo.dump();

    std::cout << take_a_dump << std::endl;

    EXPECT_EQ(take_a_dump, "{\"win_path\":\"c:\\\\something\\\\something\"}");
}
