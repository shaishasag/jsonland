#include "gtest/gtest.h"
#include "string_and_view.h"

#include <vector>
#include <unordered_map>

const char* some_text = "mama mia";

TEST(StringAndViewTest, DefaultConstructor)
{
    string_and_view sav;
    EXPECT_TRUE(sav.sv().empty()) << "default created string_and_view should be empty";
    EXPECT_TRUE(sav.is_owner()) << "empty string_and_view should be considered 'owned'";
}

TEST(StringAndViewTest, StringTypesConstructors)
{
    {
        string_and_view sav_std_string(some_text);
        EXPECT_TRUE(sav_std_string.is_owner());

        EXPECT_EQ(sav_std_string.sv(), some_text);
    }
    {
        std::string data = some_text;
        string_and_view sav_std_string(data);
        EXPECT_TRUE(sav_std_string.is_owner());

        EXPECT_EQ(sav_std_string.sv(), data);

        data += "!";
        EXPECT_NE(sav_std_string.sv(), data) << "changing the original should not change the owned copy";
    }
    {
        std::string_view data = some_text;
        string_and_view sav_std_string(data);
        EXPECT_TRUE(sav_std_string.is_owner());

        EXPECT_EQ(sav_std_string.sv(), data);

        data.remove_prefix(1);
        EXPECT_NE(sav_std_string.sv(), data) << "changing the original should not change the owned copy";
    }
}

TEST(StringAndViewTest, CopyConstructor_Owned)
{
    string_and_view original(some_text);
    EXPECT_TRUE(original.is_owner());

    string_and_view a_copy(original);
    EXPECT_TRUE(a_copy.is_owner());

    EXPECT_EQ(a_copy.sv(), original.sv());
}

TEST(StringAndViewTest, CopyConstructor_Referenced)
{
    string_and_view original;
    original.set_string_view_only(some_text);
    EXPECT_FALSE(original.is_owner());

    string_and_view a_copy(original);
    EXPECT_TRUE(a_copy.is_owner()) << "copy constructor from non-owned string_and_view should own";
    EXPECT_EQ(a_copy.sv(), original.sv());

    string_and_view moved(std::move(original));
    EXPECT_FALSE(moved.is_owner()) << "move constructor from non-owned string_and_view should not own";
    EXPECT_TRUE(original.sv().empty());

    EXPECT_EQ(moved.sv(), some_text);
    EXPECT_NE(moved.sv(), original.sv());

}

TEST(StringAndViewTest, MoveConstructor_Owned)
{
    string_and_view original(some_text);
    EXPECT_TRUE(original.is_owner());

    string_and_view moved(std::move(original));
    EXPECT_TRUE(original.sv().empty());
    EXPECT_TRUE(moved.is_owner());

    EXPECT_NE(original.sv(), some_text);
    EXPECT_NE(moved.sv(), original.sv());
    EXPECT_EQ(moved.sv(), some_text);
}

TEST(StringAndViewTest, MoveConstructor_Referenced)
{
    string_and_view original;
    original.set_string_view_only(some_text);
    EXPECT_FALSE(original.is_owner());

    string_and_view moved(std::move(original));
    EXPECT_FALSE(moved.is_owner());
    EXPECT_TRUE(original.sv().empty());

    EXPECT_EQ(moved.sv(), some_text);
}

TEST(StringAndViewTest, CopyAssignment_Owned)
{
    string_and_view original(some_text);
    EXPECT_TRUE(original.is_owner());

    string_and_view a_copy;
    a_copy = original;
    EXPECT_TRUE(a_copy.is_owner());

    EXPECT_EQ(a_copy.sv(), original.sv());
}

TEST(StringAndViewTest, CopyAssignment_Referenced)
{
    string_and_view original;
    original.set_string_view_only(some_text);
    EXPECT_FALSE(original.is_owner());

    string_and_view a_copy;
    a_copy = original;
    EXPECT_TRUE(a_copy.is_owner());

    EXPECT_EQ(a_copy.sv(), original.sv());
}

TEST(StringAndViewTest, MoveAssignment_Owned)
{
    string_and_view original(some_text);
    EXPECT_TRUE(original.is_owner());

    string_and_view moved;
    moved = std::move(original);
    EXPECT_TRUE(original.sv().empty());
    EXPECT_TRUE(moved.is_owner());

    EXPECT_NE(original.sv(), some_text);
    EXPECT_NE(moved.sv(), original.sv());
    EXPECT_EQ(moved.sv(), some_text);
}

TEST(StringAndViewTest, MoveAssignment_Referenced)
{
    string_and_view original;
    original.set_string_view_only(some_text);
    EXPECT_FALSE(original.is_owner());

    string_and_view moved;
    moved = std::move(original);
    EXPECT_FALSE(moved.is_owner());
    EXPECT_TRUE(original.sv().empty());

    EXPECT_EQ(moved.sv(), some_text);
}

TEST(StringAndViewTest, StringViewAssignment)
{
    string_and_view sav;
    sav = std::string_view(some_text);

    EXPECT_EQ(sav.sv(), some_text);
    EXPECT_TRUE(sav.is_owner());
}

TEST(StringAndViewTest, SetStringViewOnly)
{
    std::string data = some_text;
    string_and_view sav;
    sav.set_string_view_only(data);

    EXPECT_EQ(sav.sv(), data);
    EXPECT_FALSE(sav.is_owner());
}

TEST(StringAndViewTest, IsOwner_Empty)
{
    {
        string_and_view sav;
        EXPECT_TRUE(sav.is_owner()) << "empty string_and_view should be considered owned";
    }
    {
        string_and_view sav(some_text);
        EXPECT_TRUE(sav.is_owner());
    }
    {
        string_and_view sav;
        sav.set_string_view_only(std::string_view(some_text));
        EXPECT_FALSE(sav.is_owner()) << "string_and_view set with set_string_view_only  should not be owned";
    }
}

// pushing to a vector activates move constructors and/or move assignment when reallocating
TEST(StringAndViewTest, InsideContainerTestOwned)
{
    std::vector<string_and_view> v;
    v.push_back(string_and_view("one"));
    EXPECT_EQ(v[0].sv(), "one");
    EXPECT_TRUE(v[0].is_owner());

    v.push_back(string_and_view("two"));
    EXPECT_EQ(v[0].sv(), "one") << "vector member value should not change after reallocation";
    EXPECT_TRUE(v[0].is_owner()) << "vector member owned state should not change after reallocation";
    EXPECT_EQ(v[1].sv(), "two");
    EXPECT_TRUE(v[1].is_owner());

    v[0] = v[1];
    EXPECT_EQ(v[0].sv(), "two") << "vector member value should change after assignment from same vector";
    EXPECT_TRUE(v[0].is_owner()) << "vector member owned state should not change after assignment from same vector";
    EXPECT_EQ(v[1].sv(), "two");
    EXPECT_TRUE(v[1].is_owner());
}

TEST(StringAndViewTest, InsideContainerTestNonOwned)
{
    std::vector<string_and_view> v;
    v.emplace_back().set_string_view_only("one");
    EXPECT_EQ(v[0].sv(), "one");
    EXPECT_FALSE(v[0].is_owner());

    v.emplace_back().set_string_view_only("two");
    EXPECT_EQ(v[0].sv(), "one") << "vector member value should not change after reallocation";
    EXPECT_FALSE(v[0].is_owner()) << "vector member owned state should not change after reallocation";
    EXPECT_EQ(v[1].sv(), "two");
    EXPECT_FALSE(v[1].is_owner());

    v[0] = v[1];
    EXPECT_EQ(v[0].sv(), "two") << "vector member value should change after assignment from same vector";
    EXPECT_TRUE(v[0].is_owner()) << "vector member owned state should change after assignment from same vector";
    EXPECT_EQ(v[1].sv(), "two");
    EXPECT_FALSE(v[1].is_owner());
}


TEST(StringAndViewTest, OperatorStarship)
{
    {   // EqualStrings
        string_and_view sav1("hello");
        string_and_view sav2("hello");

        EXPECT_EQ(sav1 <=> sav2, std::strong_ordering::equal);
        EXPECT_FALSE(sav1 < sav2);
        EXPECT_FALSE(sav1 > sav2);
    }

    {   // LexicographicalOrder
        string_and_view sav1("apple");
        string_and_view sav2("banana");

        EXPECT_EQ(sav1 <=> sav2, std::strong_ordering::less);
        EXPECT_LT(sav1, sav2);
        EXPECT_FALSE(sav1 > sav2);

        EXPECT_EQ(sav2 <=> sav1, std::strong_ordering::greater);
        EXPECT_GT(sav2, sav1);
        EXPECT_FALSE(sav2 < sav1);
    }

    {   // EmptyStrings
        string_and_view sav1("");
        string_and_view sav2("");

        EXPECT_EQ(sav1 <=> sav2, std::strong_ordering::equal);
        EXPECT_FALSE(sav1 < sav2);
        EXPECT_FALSE(sav1 > sav2);
    }

    {   // MixedOwnership
        std::string_view view1 = "persistent_string";
        string_and_view sav1(view1);

        string_and_view sav2("persistent_string");
        sav2.set_string_view_only(view1);

        EXPECT_EQ(sav1 <=> sav2, std::strong_ordering::equal);
        EXPECT_FALSE(sav1 < sav2);
        EXPECT_FALSE(sav1 > sav2);
    }

    {   // DifferentLengths
        string_and_view sav1("short");
        string_and_view sav2("shorter");

        EXPECT_EQ(sav1 <=> sav2, std::strong_ordering::less);
        EXPECT_LT(sav1, sav2);
        EXPECT_FALSE(sav1 > sav2);

        EXPECT_EQ(sav2 <=> sav1, std::strong_ordering::greater);
        EXPECT_GT(sav2, sav1);
        EXPECT_FALSE(sav2 < sav1);
    }
}

TEST(StringAndViewTest, AsKeyToUnorderedMap)
{
    std::unordered_map<string_and_view, string_and_view> uom;

    uom[string_and_view("mama")] = "mia";
    EXPECT_EQ(uom[string_and_view("mama")].sv(), "mia");
    EXPECT_TRUE(uom[string_and_view("mama")].is_owner());

    uom[string_and_view("pil")] = "pilon";
    EXPECT_EQ(uom[string_and_view("mama")].sv(), "mia");
    EXPECT_TRUE(uom[string_and_view("mama")].is_owner());
    EXPECT_EQ(uom[string_and_view("pil")].sv(), "pilon");
    EXPECT_TRUE(uom[string_and_view("pil")].is_owner());

    uom[string_and_view("mama pil")] = uom[string_and_view("pil")];
    EXPECT_EQ(uom[string_and_view("mama")].sv(), "mia");
    EXPECT_TRUE(uom[string_and_view("mama")].is_owner());
    EXPECT_EQ(uom[string_and_view("pil")].sv(), "pilon");
    EXPECT_TRUE(uom[string_and_view("pil")].is_owner());
    EXPECT_EQ(uom[string_and_view("mama pil")].sv(), "pilon");
    EXPECT_TRUE(uom[string_and_view("mama pil")].is_owner());
}
