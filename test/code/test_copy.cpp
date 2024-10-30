#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(Copy, whole_jsons)
{
    json_node jo(object_t);

    {
        json_node ja(array_t);
        ja.push_back(1);
        ja.push_back(2);
        ja.push_back(3);
        jo["numbers"] = std::move(ja);
    }

    int sum{0};
    for (auto& num_j : jo["numbers"])
    {
        sum += num_j.get_int<int>();
    }

    EXPECT_EQ(sum, 6);
}
