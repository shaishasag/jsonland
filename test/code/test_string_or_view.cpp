#include "gtest/gtest.h"

#include "string_or_view.h"

#define TEST_REMOVED_FUNCTION 0 // set to 1 to test some reduncdant string_or_view functions

TEST(TestStringOrView, ctor)
{
    {   // default construct
        jsonland::string_or_view sv;
        EXPECT_TRUE(sv.empty());
        EXPECT_EQ(sv.size(), 0);
        EXPECT_TRUE(sv.is_value_referenced());
        EXPECT_FALSE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), ""sv);
    }
    {   // construct from partial const char*
#if TEST_REMOVED_FUNCTION==1
        jsonland::string_or_view sv(3, "people");
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), 3);
        EXPECT_TRUE(sv.is_value_referenced());
        EXPECT_FALSE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "peo"sv);
#endif
    }
    {   // construct from string_view
        jsonland::string_or_view sv("mon-cherry"sv);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), 10);
        EXPECT_TRUE(sv.is_value_referenced());
        EXPECT_FALSE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "mon-cherry"sv);
        
        // string_or_view constructed form referenced string_or_view should also be referenced
        jsonland::string_or_view sv_cp(sv);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), 10);
        EXPECT_TRUE(sv.is_value_referenced());
        EXPECT_FALSE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "mon-cherry"sv);
    }
    {   // construct from std::string
#if TEST_REMOVED_FUNCTION==1
        std::string str("chimi-churi");
        jsonland::string_or_view sv(str);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), str.size());
        EXPECT_FALSE(sv.is_value_referenced());
        EXPECT_TRUE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "chimi-churi"sv);
        
        // string_or_view constructed form stored string_or_view should also be stored
        jsonland::string_or_view sv_cp(sv);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), str.size());
        EXPECT_FALSE(sv.is_value_referenced());
        EXPECT_TRUE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "chimi-churi"sv);
#endif
    }
}

TEST(TestStringOrView, store_value)
{
    {   // store_value of std::string
        jsonland::string_or_view sv("lala land");
        std::string str("Django Reinhardt");
        sv.store_value(str);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), str.size());
        EXPECT_FALSE(sv.is_value_referenced());
        EXPECT_TRUE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "Django Reinhardt"sv);
        
        // check that std::string was not moved
        EXPECT_FALSE(str.empty());
        EXPECT_EQ(str, "Django Reinhardt"sv);
    }
    
    {   // store_value of std::string&&
#if TEST_REMOVED_FUNCTION==1
        jsonland::string_or_view sv("lala land");
        std::string str("Django Reinhardt");
        sv.store_value(std::move(str));
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), 16);
        EXPECT_FALSE(sv.is_value_referenced());
        EXPECT_TRUE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "Django Reinhardt"sv);
        
        // check that std::string was moved
        EXPECT_TRUE(str.empty());
#endif
    }
    
    {   // store_value of std::string_view
#if TEST_REMOVED_FUNCTION==1
        jsonland::string_or_view sv("lala land");
        std::string_view strv("Django Reinhardt");
        sv.store_value(strv);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), strv.size());
        EXPECT_FALSE(sv.is_value_referenced());
        EXPECT_TRUE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "Django Reinhardt"sv);
#endif
    }
    {   // store_value of const char*
        jsonland::string_or_view sv("lala land");
        const char* c_str{"Django Reinhardt"};
        sv.store_value(c_str);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), 16);
        EXPECT_FALSE(sv.is_value_referenced());
        EXPECT_TRUE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "Django Reinhardt"sv);
    }
}


TEST(TestStringOrView, reference_value)
{
    {   // reference_value of std::string
        jsonland::string_or_view sv("lala land");
        std::string str("Django Reinhardt");
        sv.reference_value(str);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), str.size());
        EXPECT_TRUE(sv.is_value_referenced());
        EXPECT_FALSE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "Django Reinhardt"sv);
        
        // check that std::string was not moved
        EXPECT_FALSE(str.empty());
        EXPECT_EQ(str, "Django Reinhardt"sv);
    }
    
    {   // reference_value of std::string_view
        jsonland::string_or_view sv("lala land");
        std::string_view strv("Django Reinhardt");
        sv.reference_value(strv);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), strv.size());
        EXPECT_TRUE(sv.is_value_referenced());
        EXPECT_FALSE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "Django Reinhardt"sv);
   }
    
    {   // reference_value of const char*
        jsonland::string_or_view sv("lala land");
        const char* c_str{"Django Reinhardt"};
        sv.reference_value(c_str);
        EXPECT_FALSE(sv.empty());
        EXPECT_EQ(sv.size(), 16);
        EXPECT_TRUE(sv.is_value_referenced());
        EXPECT_FALSE(sv.is_value_stored());
        EXPECT_EQ(sv.as_string_view(), "Django Reinhardt"sv);
    }
}
