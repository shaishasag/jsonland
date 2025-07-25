#include "gtest/gtest.h"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(Merge, merge_no_overlap)
{
    {   // merge-clone
        json_node to(object_t);
        to["one"] = "one";
        to["two"] = 2;
        to["three"] = 3.0;
        
        json_node from(object_t);
        from["four"] = "four";
        from["five"] = 5;
        from["six"] = 6.0;

        to.merge_from(from);
        
        // check the merging object
        EXPECT_TRUE(to["one"].get_string() == "one");
        EXPECT_TRUE(to["two"].get_int() == 2);
        EXPECT_TRUE(to["three"].get_double() == 3.0);
        EXPECT_TRUE(to["four"].get_string() == "four");
        EXPECT_TRUE(to["five"].get_int() == 5);
        EXPECT_TRUE(to["six"].get_double() == 6.0);
        
        // merged object should remain unchanged
        EXPECT_TRUE(from["four"].get_string() == "four");
        EXPECT_TRUE(from["five"].get_int() == 5);
        EXPECT_TRUE(from["six"].get_double() == 6.0);
    }
    {   // merge-move
        json_node to(object_t);
        to["one"] = "one";
        to["two"] = 2;
        to["three"] = 3.0;
        
        json_node from(object_t);
        from["four"] = "four";
        from["five"] = 5;
        from["six"] = 6.0;

        to.merge_from(std::move(from));
        
        // check the merging object
        EXPECT_TRUE(to["one"].get_string() == "one");
        EXPECT_TRUE(to["two"].get_int() == 2);
        EXPECT_TRUE(to["three"].get_double() == 3.0);
        EXPECT_TRUE(to["four"].get_string() == "four");
        EXPECT_TRUE(to["five"].get_int() == 5);
        EXPECT_TRUE(to["six"].get_double() == 6.0);
        
        // merged-from object should be empty
        EXPECT_TRUE(from.empty());
    }
}

TEST(Merge, merge_some_overlap)
{
    {   // merge-clone
        json_node to(object_t);
        to["one"] = "one";
        to["two"] = 2;
        to["three"] = 3.0;
        
        json_node from(object_t);
        from["one"] = "four";
        from["five"] = 5;
        from["three"] = 6.0;

        to.merge_from(from);
        
        EXPECT_TRUE(to["one"].get_string() == "four");
        EXPECT_TRUE(to["two"].get_int() == 2);
        EXPECT_TRUE(to["three"].get_double() == 6.0);
        EXPECT_TRUE(to["five"].get_int() == 5);
        
        // merged object should remain unchanged
        EXPECT_TRUE(from["one"].get_string() == "four");
        EXPECT_TRUE(from["five"].get_int() == 5);
        EXPECT_TRUE(from["three"].get_double() == 6.0);
    }
    {   // merge-move
        json_node to(object_t);
        to["one"] = "one";
        to["two"] = 2;
        to["three"] = 3.0;
        
        json_node from(object_t);
        from["one"] = "four";
        from["five"] = 5;
        from["three"] = 6.0;

        to.merge_from(std::move(from));
        
        EXPECT_TRUE(to["one"].get_string() == "four");
        EXPECT_TRUE(to["two"].get_int() == 2);
        EXPECT_TRUE(to["three"].get_double() == 6.0);
        EXPECT_TRUE(to["five"].get_int() == 5);
        
        // merged-from object should be empty
        EXPECT_TRUE(from.empty());
    }
}

TEST(Merge, merge_all_overlap)
{
    {   // merge-clone
        json_node to(object_t);
        to["one"] = "one";
        to["two"] = 2;
        to["three"] = 3.0;
        
        json_node from(object_t);
        from["one"] = "one1";
        from["two"] = 2+2;
        from["three"] = 3.0+3.0;
        
        to.merge_from(from);
        
        EXPECT_TRUE(to["one"].get_string() == "one1");
        EXPECT_TRUE(to["two"].get_int() == 4);
        EXPECT_TRUE(to["three"].get_double() == 6.0);
        
        // merged object should remain unchanged
        EXPECT_TRUE(from["one"].get_string() == "one1");
        EXPECT_TRUE(from["two"].get_int() == 4);
        EXPECT_TRUE(from["three"].get_double() == 6.0);
    }
    {   // merge-move
        json_node to(object_t);
        to["one"] = "one";
        to["two"] = 2;
        to["three"] = 3.0;
        
        json_node from(object_t);
        from["one"] = "one1";
        from["two"] = 2+2;
        from["three"] = 3.0+3.0;
        
        to.merge_from(std::move(from));
        
        EXPECT_TRUE(to["one"].get_string() == "one1");
        EXPECT_TRUE(to["two"].get_int() == 4);
        EXPECT_TRUE(to["three"].get_double() == 6.0);
        
        // merged-from object should be empty
        EXPECT_TRUE(from.empty());
    }
}

TEST(Merge, merge_deep)
{
    {   // merge-clone
        json_node to(object_t);
        to["one"] = "one";
        to["two"] = 2;
        to["three"] = 3.0;
        
        json_node from(object_t);
        auto& from_sub_obj1 = from.append_object("music");
        from_sub_obj1["guitar"] = "string";
        from_sub_obj1["trumpet"] = "brass";
        from_sub_obj1["drums"] = "percussion";

        to.merge_from(from);
        EXPECT_TRUE(to["one"].get_string() == "one");
        EXPECT_TRUE(to["two"].get_int() == 2);
        EXPECT_TRUE(to["three"].get_double() == 3.0);
        
        auto& to_sub_obj1 = from["music"];
        EXPECT_EQ(to_sub_obj1.size_as(object_t), 3);
        EXPECT_TRUE(to_sub_obj1["guitar"].get_string() == "string");
        EXPECT_TRUE(to_sub_obj1["trumpet"].get_string() == "brass");
        EXPECT_TRUE(to_sub_obj1["drums"].get_string() == "percussion");
    }
    {   // merge-move
        json_node to(object_t);
        to["one"] = "one";
        to["two"] = 2;
        to["three"] = 3.0;
        
        json_node from(object_t);
        auto& from_sub_obj1 = from.append_object("music");
        from_sub_obj1["guitar"] = "string";
        from_sub_obj1["trumpet"] = "brass";
        from_sub_obj1["drums"] = "percussion";

        to.merge_from(std::move(from));
        
        EXPECT_TRUE(to["one"].get_string() == "one");
        EXPECT_TRUE(to["two"].get_int() == 2);
        EXPECT_TRUE(to["three"].get_double() == 3.0);
        
        auto& to_sub_obj1 = to["music"];
        EXPECT_EQ(to_sub_obj1.size_as(object_t), 3);
        EXPECT_TRUE(to_sub_obj1["guitar"].get_string() == "string");
        EXPECT_TRUE(to_sub_obj1["trumpet"].get_string() == "brass");
        EXPECT_TRUE(to_sub_obj1["drums"].get_string() == "percussion");
        
        // merged-from object should be empty
        EXPECT_TRUE(from.empty());
    }

}

TEST(Merge, merge_parsed_insitu)
{
    {   // merge-clone
        
        std::string_view to_source = R"({"one":"one","two": 2, "three": 3.0})";
        json_doc to;
        to.parse_insitu(to_source);
        
        std::string_view from_source = R"({"music": {"guitar":"string","trumpet": "brass", "drums":"percussion"}})";
        json_doc from;
        from.parse_insitu(from_source);

        to.merge_from(from);
        std::cout << to.dump() << std::endl;
        
        EXPECT_TRUE(to["one"].get_string() == "one");
        EXPECT_TRUE(to["two"].get_int() == 2);
        EXPECT_TRUE(to["three"].get_double() == 3.0);
        
        auto& to_sub_obj1 = from["music"];
        EXPECT_EQ(to_sub_obj1.size_as(object_t), 3);
        EXPECT_TRUE(to_sub_obj1["guitar"].get_string() == "string");
        EXPECT_TRUE(to_sub_obj1["trumpet"].get_string() == "brass");
        EXPECT_TRUE(to_sub_obj1["drums"].get_string() == "percussion");
    }
    {   // merge-move
        std::string_view to_source = R"({"one":"one","two": 2, "three": 3.0})";
        json_doc to;
        to.parse_insitu(to_source);
        
        std::string_view from_source = R"({"music": {"guitar":"string","trumpet": "brass", "drums":"percussion"}})";
        json_doc from;
        from.parse_insitu(from_source);

        to.merge_from(std::move(from));
        
        EXPECT_TRUE(to["one"].get_string() == "one");
        EXPECT_TRUE(to["two"].get_int() == 2);
        EXPECT_TRUE(to["three"].get_double() == 3.0);
        
        auto& to_sub_obj1 = to["music"];
        EXPECT_EQ(to_sub_obj1.size_as(object_t), 3);
        EXPECT_TRUE(to_sub_obj1["guitar"].get_string() == "string");
        EXPECT_TRUE(to_sub_obj1["trumpet"].get_string() == "brass");
        EXPECT_TRUE(to_sub_obj1["drums"].get_string() == "percussion");
        
        // merged-from object should be empty
        EXPECT_TRUE(from.empty());
    }
    
}
