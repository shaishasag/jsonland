#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include <array>
#include <map>
#include <map>
#include <unordered_map>
#include <deque>
#include <list>
#include <forward_list>
#include <unordered_map>

using namespace jsonland;

TEST(Extend, VectorArray)
{
    {   // empty vector
        std::vector<int> v_in;
        json_node j;
        j.extend(v_in);

        EXPECT_TRUE(j.is_array()); // should be converted to an array even is v is empty
        EXPECT_TRUE(j.empty());
        EXPECT_TRUE(j.empty_as(array_t));
        EXPECT_EQ(j.size_as<array_t>(), 0);

        std::vector<int> v_out;
        j.extract(v_out);
        ASSERT_EQ(v_out, v_in);

        // push_back some more
        j.push_back(5);
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j.empty_as(array_t));
        EXPECT_EQ(j.size_as<array_t>(), 1);
        EXPECT_EQ(j[0].get_int(), 5);
    }
    {   // non-empty vector
        std::vector<int> v_in{1,2,3,4};
        json_node j;
        j.extend(v_in);

        EXPECT_TRUE(j.is_array());
        EXPECT_FALSE(j.empty());
        EXPECT_EQ(j.size_as<array_t>(), 4);
        EXPECT_EQ(j[0].get_int(), 1);
        EXPECT_EQ(j[1].get_int(), 2);
        EXPECT_EQ(j[2].get_int(), 3);
        EXPECT_EQ(j[3].get_int(), 4);

        std::vector<int> v_out;
        j.extract(v_out);
        ASSERT_EQ(v_out, v_in);

        // push_back some more
        j.push_back(5);
        EXPECT_EQ(j.size_as<array_t>(), 5);
        EXPECT_EQ(j[0].get_int(), 1);
        EXPECT_EQ(j[1].get_int(), 2);
        EXPECT_EQ(j[2].get_int(), 3);
        EXPECT_EQ(j[3].get_int(), 4);
        EXPECT_EQ(j[4].get_int(), 5);

        // extend with values of different type
        std::array<std::string_view, 2> svv{"mama"sv, "mia"sv};
        j.extend(svv);

        EXPECT_EQ(j.size_as<array_t>(), 7);
        EXPECT_EQ(j[0].get_int(), 1);
        EXPECT_EQ(j[1].get_int(), 2);
        EXPECT_EQ(j[2].get_int(), 3);
        EXPECT_EQ(j[3].get_int(), 4);
        EXPECT_EQ(j[4].get_int(), 5);
        EXPECT_EQ(j[5].get_string(), "mama"sv);
        EXPECT_EQ(j[6].get_string(), "mia"sv);
    }
}

TEST(Extend, AllSequenceTypes)
{
    json_node j;

    std::vector<int> v{1,2};
    j.extend(v);

    std::array<std::string_view, 2> svv{"mama"sv, "mia"sv};
    j.extend(svv);

    std::deque<double> dq{123.456, 789.012};
    j.extend(dq);

    std::list<bool> lst{true, false};
    j.extend(lst);

    std::forward_list<uint16_t> frwdl{888, 999};
    j.extend(frwdl);

    EXPECT_TRUE(j.is_array());
    EXPECT_FALSE(j.empty());
    EXPECT_EQ(j.size_as<array_t>(), 10);
    EXPECT_EQ(j[0].get_int(), 1);
    EXPECT_EQ(j[1].get_int(), 2);
    EXPECT_EQ(j[2].get_string(), "mama"sv);
    EXPECT_EQ(j[3].get_string(), "mia"sv);
    EXPECT_EQ(j[4].get_double(), 123.456);
    EXPECT_EQ(j[5].get_double(), 789.012);
    EXPECT_EQ(j[6].get_bool(), true);
    EXPECT_EQ(j[7].get_bool(), false);
    EXPECT_EQ(j[8].get_int(), 888);
    EXPECT_EQ(j[9].get_int(), 999);
}

TEST(Extend, Map)
{
    {   // empty map
        std::map<std::string_view, int> m;
        json_node j;
        j.extend(m);

        EXPECT_TRUE(j.is_object()); // should be converted to an object even is m is empty
        EXPECT_TRUE(j.empty());
        EXPECT_TRUE(j.empty_as(object_t));
        EXPECT_EQ(j.size_as(object_t), 0);

        // push_back some more
        j["five"] = 5;
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j.empty_as(object_t));
        EXPECT_EQ(j.size_as(object_t), 1);
        EXPECT_EQ(j["five"].get_int(), 5);
    }
    {   // non-empty map
        std::map<std::string_view, int> m{{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, };
        json_node j;
        j.extend(m);

        EXPECT_TRUE(j.is_object());
        EXPECT_FALSE(j.empty());
        EXPECT_EQ(j.size_as(object_t), 4);
        EXPECT_EQ(j["one"].get_int(), 1);
        EXPECT_EQ(j["two"].get_int(), 2);
        EXPECT_EQ(j["three"].get_int(), 3);
        EXPECT_EQ(j["four"].get_int(), 4);

        // push_back some more
        j["five"] = 5;
        EXPECT_EQ(j.size_as(object_t), 5);
        EXPECT_EQ(j["one"].get_int(), 1);
        EXPECT_EQ(j["two"].get_int(), 2);
        EXPECT_EQ(j["three"].get_int(), 3);
        EXPECT_EQ(j["four"].get_int(), 4);
        EXPECT_EQ(j["five"].get_int(), 5);
    }
}

TEST(Extend, UnorderedMap)
{
    {   // empty map
        std::unordered_map<std::string_view, int> m;
        json_node j;
        j.extend(m);

        EXPECT_TRUE(j.is_object()); // should be converted to an object even is m is empty
        EXPECT_TRUE(j.empty());
        EXPECT_TRUE(j.empty_as(object_t));
        EXPECT_EQ(j.size_as(object_t), 0);

        // push_back some more
        j["five"] = 5;
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j.empty_as(object_t));
        EXPECT_EQ(j.size_as(object_t), 1);
        EXPECT_EQ(j["five"].get_int(), 5);
    }
    {   // non-empty map
        std::unordered_map<std::string_view, int> m{{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, };
        json_node j;
        j.extend(m);

        EXPECT_TRUE(j.is_object());
        EXPECT_FALSE(j.empty());
        EXPECT_EQ(j.size_as(object_t), 4);
        EXPECT_EQ(j["one"].get_int(), 1);
        EXPECT_EQ(j["two"].get_int(), 2);
        EXPECT_EQ(j["three"].get_int(), 3);
        EXPECT_EQ(j["four"].get_int(), 4);

        // push_back some more
        j["five"] = 5;
        EXPECT_EQ(j.size_as(object_t), 5);
        EXPECT_EQ(j["one"].get_int(), 1);
        EXPECT_EQ(j["two"].get_int(), 2);
        EXPECT_EQ(j["three"].get_int(), 3);
        EXPECT_EQ(j["four"].get_int(), 4);
        EXPECT_EQ(j["five"].get_int(), 5);
    }
}

TEST(Extend, AllMapTypes)
{
    {
        json_node j;
        std::unordered_map<std::string_view, int> uom{{"one", 1}, {"two", 2}};
        j.extend(uom);

        std::multimap<const char*, double> mul_map{{"three", 3.0}, {"four", 4.0}};
        j.extend(mul_map);

        std::unordered_multimap<std::string, float> mul_uom{{"five", 5.0}, {"six", 6.0}};
        j.extend(mul_uom);

        std::unordered_multimap<std::string, bool> bool_uom{{"TRUE", true}, {"FALSE", false}};
        j.extend(bool_uom);

        std::unordered_multimap<std::string, std::string> str_uom{{"What?", "7"}, {"Where?", "8"}};
        j.extend(str_uom);

        EXPECT_TRUE(j.is_object());
        EXPECT_FALSE(j.empty());
        EXPECT_EQ(j.size_as(object_t), 10);
        EXPECT_EQ(j["one"].get_int(), 1);
        EXPECT_EQ(j["two"].get_int(), 2);
        EXPECT_EQ(j["three"].get_double(), 3.0);
        EXPECT_EQ(j["four"].get_double(), 4.0);
        EXPECT_EQ(j["five"].get_float(), 5.0);
        EXPECT_EQ(j["six"].get_float(), 6.0);

        // extract values as int64_t, convert to number where possible
        std::map<std::string_view, int64_t> m;
        j.extract(m);
        EXPECT_EQ(m["one"], 1);
        EXPECT_EQ(m["two"], 2);
        EXPECT_EQ(m["three"], 3);
        EXPECT_EQ(m["four"], 4);
        EXPECT_EQ(m["five"], 5);
        EXPECT_EQ(m["six"], 6);
        EXPECT_EQ(m["What?"], 7);
        EXPECT_EQ(m["Where?"], 8);
    }
}
