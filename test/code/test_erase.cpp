#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(Erase, from_empty)
{
    {
        json_node jNull{null_t};
        size_t num_erased = jNull.erase("Motti");  // erase by key
        EXPECT_EQ(num_erased, 0);
        num_erased = jNull.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
    {
        json_node jBool{bool_t};
        size_t num_erased = jBool.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        num_erased = jBool.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
    {
        json_node jNum{number_t};
        size_t num_erased = jNum.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        num_erased = jNum.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
    {
        json_node jString{string_t};
        size_t num_erased = jString.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        num_erased = jString.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
    {
        json_node jArray{array_t};
        size_t num_erased = jArray.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        num_erased = jArray.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
    {
        json_node jObj{object_t};
        size_t num_erased = jObj.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        num_erased = jObj.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
}

TEST(Erase, from_something_with_value)
{
    {
        json_node jBool{true};
        size_t num_erased = jBool.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        num_erased = jBool.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
    {
        json_node jNum{17.9};
        size_t num_erased = jNum.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        float the_num = jNum.get_float<float>();
        EXPECT_EQ(the_num, 17.9f);
        num_erased = jNum.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
        EXPECT_EQ(the_num, 17.9f);
    }
    {
        json_node jString{"Motti"};
        size_t num_erased = jString.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        EXPECT_EQ(jString.get_string(), "Motti");
        num_erased = jString.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
    {
        json_node jArray{array_t};
        jArray.push_back("Motti");
        size_t num_erased = jArray.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        EXPECT_EQ(jArray[0].get_string(), "Motti");
        num_erased = jArray.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
    {
        json_node jObj{object_t};
        jObj["Itsik"] = "Manger";
        size_t num_erased = jObj.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        EXPECT_EQ(jObj["Itsik"].get_string(), "Manger");
        num_erased = jObj.erase(19);  // erase by index
        EXPECT_EQ(num_erased, 0);
    }
}

TEST(Erase, from_object_containing)
{
    {
        json_node jObj{object_t};
        jObj["Itsik"] = "Manger";
        jObj["Motti"] = "Shneor";
        EXPECT_EQ(jObj.num_elements(), 2);
        EXPECT_TRUE(jObj.contains("Itsik"));
        EXPECT_TRUE(jObj.contains("Motti"));

        size_t num_erased = jObj.erase(0);// erase by index should not effect object
        EXPECT_EQ(jObj.num_elements(), 2);
        EXPECT_TRUE(jObj.contains("Itsik"));
        EXPECT_TRUE(jObj.contains("Motti"));
        num_erased = jObj.erase(1);// erase by index should not effect object
        EXPECT_EQ(jObj.num_elements(), 2);
        EXPECT_TRUE(jObj.contains("Itsik"));
        EXPECT_TRUE(jObj.contains("Motti"));

        
        num_erased = jObj.erase("Motti");
        EXPECT_EQ(num_erased, 1);
        EXPECT_TRUE(jObj.contains("Itsik"));
        EXPECT_FALSE(jObj.contains("Motti"));
        EXPECT_EQ(jObj.num_elements(), 1);
        EXPECT_EQ(jObj["Itsik"].get_string(), "Manger");
        
        // erase twice
        num_erased = jObj.erase("Motti");
        EXPECT_EQ(num_erased, 0);
        EXPECT_TRUE(jObj.contains("Itsik"));
        EXPECT_FALSE(jObj.contains("Motti"));
        EXPECT_EQ(jObj.num_elements(), 1);
        EXPECT_EQ(jObj["Itsik"].get_string(), "Manger");
    }
}


TEST(Erase, from_array_containing)
{
    {
        json_node jArray{array_t};
        jArray.push_back("Manger");
        jArray.push_back("Motti");
        EXPECT_EQ(jArray.num_elements(), 2);
        EXPECT_EQ(jArray[0].get_string(), "Manger");
        EXPECT_EQ(jArray[1].get_string(), "Motti");

        size_t num_erased = jArray.erase("Manger");// erase by key should not effect array
        EXPECT_EQ(jArray.num_elements(), 2);
        EXPECT_EQ(jArray[0].get_string(), "Manger");
        EXPECT_EQ(jArray[1].get_string(), "Motti");
        num_erased = jArray.erase("Motti");// erase by key should not effect array
        EXPECT_EQ(jArray.num_elements(), 2);
        EXPECT_EQ(jArray[0].get_string(), "Manger");
        EXPECT_EQ(jArray[1].get_string(), "Motti");

        num_erased = jArray.erase(2);  // erase by non-existing index should not effect array
        EXPECT_EQ(num_erased, 0);
        EXPECT_EQ(jArray.num_elements(), 2);
        EXPECT_EQ(jArray[0].get_string(), "Manger");
        EXPECT_EQ(jArray[1].get_string(), "Motti");

        // erase from end
        num_erased = jArray.erase(1);
        EXPECT_EQ(num_erased, 1);
        EXPECT_EQ(jArray.num_elements(), 1);
        EXPECT_EQ(jArray[0].get_string(), "Manger");
        
        // erase from beginning
        num_erased = jArray.erase(0);
        EXPECT_EQ(num_erased, 1);
        EXPECT_EQ(jArray.num_elements(), 0);

        // erase again
        num_erased = jArray.erase(0);
        EXPECT_EQ(num_erased, 0);
        EXPECT_EQ(jArray.num_elements(), 0);
    }
}
