#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(Deduction, all_types_to_all_types)
{
    {
        json_node j_null{nullptr};
        EXPECT_EQ(j_null.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_null.get_as<bool>(), false);
        EXPECT_EQ(j_null.get_as<int>(), 0);
        EXPECT_EQ(j_null.get_as<float>(), 0.0);
        EXPECT_EQ(j_null.get_as<std::string_view>(), "null");
    }

    {
        json_node j_false{false};
        EXPECT_EQ(j_false.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_false.get_as<bool>(), false);
        EXPECT_EQ(j_false.get_as<int>(), 0);
        EXPECT_EQ(j_false.get_as<float>(), 0.0);
        EXPECT_EQ(j_false.get_as<std::string_view>(), "false");
    }

    {
        json_node j_true{true};
        EXPECT_EQ(j_true.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_true.get_as<bool>(), true);
        EXPECT_EQ(j_true.get_as<int>(), 1);
        EXPECT_EQ(j_true.get_as<float>(), 1.0);
        EXPECT_EQ(j_true.get_as<std::string_view>(), "true");
    }

    {
        json_node j_int{10};
        EXPECT_EQ(j_int.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_int.get_as<bool>(), true);
        EXPECT_EQ(j_int.get_as<int>(), 10);
        EXPECT_EQ(j_int.get_as<float>(), 10.0);
        //EXPECT_EQ(j_int.get_as<std::string_view>(), std::string_view("10"));
    }

    {
        json_node j_zero_int{0};
        EXPECT_EQ(j_zero_int.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_zero_int.get_as<bool>(), false);
        EXPECT_EQ(j_zero_int.get_as<int>(), 0);
        EXPECT_EQ(j_zero_int.get_as<float>(), 0.0);
        //EXPECT_EQ(j_zero_int.get_as<std::string_view>(), std::string_view("10"));
    }
    {
        json_node j_float{3.1415};
        EXPECT_EQ(j_float.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_float.get_as<bool>(), true);
        EXPECT_EQ(j_float.get_as<int>(), 3);
        EXPECT_NEAR(j_float.get_as<float>(), 3.1415, 0.0001);
        //EXPECT_EQ(j_float.get_as<std::string_view>(), std::string_view("10"));
    }

    {
        json_node j_str_num{"3.1415"};
        EXPECT_EQ(j_str_num.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_str_num.get_as<bool>(), false);
        EXPECT_EQ(j_str_num.get_as<int>(), 3);
        EXPECT_NEAR(j_str_num.get_as<float>(), 3.1415, 0.0001);
        //EXPECT_EQ(j_float.get_as<std::string_view>(), std::string_view("10"));
    }

    {
        json_node j_zero_float{0.0};
        EXPECT_EQ(j_zero_float.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_zero_float.get_as<bool>(), false);
        EXPECT_EQ(j_zero_float.get_as<int>(), 0);
        EXPECT_NEAR(j_zero_float.get_as<float>(), 0.0, 0.0001);
        //EXPECT_EQ(j_zero_float.get_as<std::string_view>(), std::string_view("10"));
    }

    {
        json_node j_str_np_num{"mama mia"};
        EXPECT_EQ(j_str_np_num.get_as<nullptr_t>(), nullptr);
        EXPECT_EQ(j_str_np_num.get_as<bool>(), false);
        EXPECT_EQ(j_str_np_num.get_as<int>(), 0);
        EXPECT_NEAR(j_str_np_num.get_as<float>(), 0.0, 0.0001);
        //EXPECT_EQ(j_float.get_as<std::string_view>(), std::string_view("10"));
    }

}

TEST(Deduction, deduce_array)
{
    json_node j; // create json_node without specifing type - should default to null_t
    EXPECT_TRUE(j.is_null());

    json_node& jo = j["o"];     // add member without specifing type
    EXPECT_TRUE(j.is_object()); // ... j should now have type object_t
    EXPECT_TRUE(jo.is_null());  // ... new member should default to null_t

    json_node& ja1 = j.append_array("a1"); // use append_array
    EXPECT_TRUE(j.is_object());
    EXPECT_TRUE(ja1.is_array());            // ... appended member should be array_t

    json_node& ja2 = j["a2"];   // add member without specifing type
    EXPECT_TRUE(ja2.is_null());
    ja2.push_back("banana");    // ...  but later use it as an array
    ja2.push_back("rama");
    EXPECT_TRUE(ja2.is_array());// using push_back should have set ja2 to array_t

    json_node& ja3 = j["a3"];   // add member without specifing type
    EXPECT_TRUE(ja3.is_null());
    ja3 = array_t;              // ...  but later explicitly set it to array_t
    EXPECT_TRUE(ja3.is_array());
    EXPECT_TRUE(j.is_object());
}

TEST(Deduction, deduce_object)
{
    json_node j;

    json_node& jo = j["o"];     // add member without specifing type of new member
    EXPECT_TRUE(j.is_object()); // ... j should now have type object_t
    EXPECT_TRUE(jo.is_null());  // ... new member should default to null_t

    json_node& jo1 = j.append_object("o1"); // use append_object
    EXPECT_TRUE(j.is_object());
    EXPECT_TRUE(jo1.is_object());            // ... appended member should be object_t

    json_node& jo2 = j["o2"]; // add member without specifing type
    EXPECT_TRUE(jo2.is_null());
    jo2["banana"] = "rama"; // ...  but later use it as an object
    EXPECT_TRUE(jo2.is_object());

    json_node& jo3 = j["o3"];   // add member without specifing type
    EXPECT_TRUE(jo3.is_null());
    jo3 = object_t;              // ...  but later explicitly set it as an object
    EXPECT_TRUE(jo3.is_object());
    EXPECT_TRUE(j.is_object());
}

TEST(Deduction, read_mixed_types)
{
    json_node j;
    j.push_back(0);
    j.push_back(json_node());
    j.push_back("");
    j.push_back("mama mia");
    j.push_back("1 is one");
    j.push_back(1);
    j.push_back("2");
    j.push_back(3);
    j.push_back(5);

    int sum{0};

    for (auto& n : j)
    {
        sum += n.get_as<int>();
    }
    std::cout << sum << std::endl;
}
