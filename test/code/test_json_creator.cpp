#include "gtest/gtest.h"
#include <string_view>
using namespace std::literals;

#include "fstring/fstring.h"
#include "jsonland/json_creator.h"
#include "jsonland/json_node.h"

// tests are templated to check the two types of json creators:
// - object_json_creator & array_json_creator based on fstr::fstr
// - object_json_creator & array_json_creator based on std::string

template <typename T>
class CreateJsonTemplatedTest : public ::testing::Test {
};

// Register the pairs of types to test
// std::pair::first => object_json_creator<...>
// std::pair::second => array_json_creator<...>
typedef ::testing::Types<std::pair<jl_fixed::object_json_creator<511>,
                                    jl_fixed::array_json_creator<511>>,
                         std::pair<jl_dyna::object_json_creator,
                                    jl_dyna::array_json_creator>> ObjArrPair;

TYPED_TEST_SUITE(CreateJsonTemplatedTest, ObjArrPair);

TYPED_TEST(CreateJsonTemplatedTest, SOMETHING_TYPED) {
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    obj_creator_t joc1;
    joc1.append_value("mitsi", "pitsi");
    EXPECT_STREQ(joc1.c_str(), R"|({"mitsi": "pitsi"})|");
}


TYPED_TEST(CreateJsonTemplatedTest, merge_from_another)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    obj_creator_t joc1;
    joc1.append_value("mitsi", "pitsi");
    EXPECT_STREQ(joc1.c_str(), R"|({"mitsi": "pitsi"})|");
    
    obj_creator_t joc2;
    joc2.append_values_from(joc1); // merge to empty
    EXPECT_STREQ(joc1.c_str(), R"|({"mitsi": "pitsi"})|");
    EXPECT_STREQ(joc2.c_str(), R"|({"mitsi": "pitsi"})|");
    
    obj_creator_t joc3;
    joc3.append_value("Ani", "Purim");
    joc3.append_values_from(joc2); // merge to jl_fixed::object_json_creator that has some values
    EXPECT_STREQ(joc1.c_str(), R"|({"mitsi": "pitsi"})|");
    EXPECT_STREQ(joc2.c_str(), R"|({"mitsi": "pitsi"})|");
    EXPECT_STREQ(joc3.c_str(), R"|({"Ani": "Purim", "mitsi": "pitsi"})|");
    
    obj_creator_t joc_empty;
    joc3.append_values_from(joc_empty); // merge from an empty jl_fixed::object_json_creator
    EXPECT_STREQ(joc1.c_str(), R"|({"mitsi": "pitsi"})|");
    EXPECT_STREQ(joc2.c_str(), R"|({"mitsi": "pitsi"})|");
    EXPECT_STREQ(joc3.c_str(), R"|({"Ani": "Purim", "mitsi": "pitsi"})|");
}

TYPED_TEST(CreateJsonTemplatedTest, intertwine_obj_and_arr)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    obj_creator_t joc1;
    
    joc1.append_value("one", 1);
    auto arr1 = joc1.append_array("ARRAY!");
    auto joc2 = arr1.append_object();
    joc2.append_value("unit", "milliseconds");
    
    arr1.append_value("Lou Reed");
    
    auto joc3 = arr1.append_object();
    joc3.append_value("Part", "Time");
 
    EXPECT_STREQ(joc3.c_str(), R"|({"one": 1, "ARRAY!": [{"unit": "milliseconds"}, "Lou Reed", {"Part": "Time"}]})|");
}


TYPED_TEST(CreateJsonTemplatedTest, append_json_str)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    obj_creator_t joc1;
    obj_creator_t joc_small;
    joc_small.append_value("mama", "mia");
    
    // append_json_str (which is an object) on empty object
    joc1.append_json_str("first", joc_small.c_str());
    EXPECT_STREQ(joc1.c_str(), R"|({"first": {"mama": "mia"}})|");
    
    joc_small.clear();
    // append_json_str (which is an object) on object with values
    joc_small.append_value("dancing", "queen");
    joc1.append_json_str("second", joc_small.c_str());
    EXPECT_STREQ(joc1.c_str(), R"|({"first": {"mama": "mia"}, "second": {"dancing": "queen"}})|");
    
    joc1.clear();
    jl_fixed::array_json_creator<31> jac_small;
    jac_small.append_value("mama", "mia");
    // append_json_str (which is an array) on empty object
    joc1.append_json_str("first", jac_small.c_str());
    EXPECT_STREQ(joc1.c_str(), R"|({"first": ["mama", "mia"]})|");
    
    jac_small.clear();
    // append_json_str (which is an array) on object with values
    jac_small.append_value("dancing", "queen");
    joc1.append_json_str("second", jac_small.c_str());
    EXPECT_STREQ(joc1.c_str(), R"|({"first": ["mama", "mia"], "second": ["dancing", "queen"]})|");
}


TYPED_TEST(CreateJsonTemplatedTest, object_prepend_json_str)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    obj_creator_t joc1;
    obj_creator_t joc_small;
    joc_small.append_value("mama", "mia");
    
    // prepend_json_str (which is an object) on empty object
    joc1.prepend_json_str("first", joc_small.c_str());
    EXPECT_STREQ(joc1.c_str(), R"|({"first": {"mama": "mia"}})|");
    
    joc_small.clear();
    // prepend_json_str (which is an object) on object with values
    joc_small.append_value("dancing", "queen");
    joc1.prepend_json_str("second", joc_small.c_str());
    EXPECT_STREQ(joc1.c_str(), R"|({"second": {"dancing": "queen"}, "first": {"mama": "mia"}})|");
    
    joc1.clear();
    jl_fixed::array_json_creator<31> jac_small;
    jac_small.append_value("mama", "mia");
    // prepend_json_str (which is an array) on empty object
    joc1.prepend_json_str("first", jac_small.c_str());
    EXPECT_STREQ(joc1.c_str(), R"|({"first": ["mama", "mia"]})|");
    
    jac_small.clear();
    // prepend_json_str (which is an array) on object with values
    jac_small.append_value("dancing", "queen");
    joc1.prepend_json_str("second", jac_small.c_str());
    EXPECT_STREQ(joc1.c_str(), R"|({"second": ["dancing", "queen"], "first": ["mama", "mia"]})|");
}



TYPED_TEST(CreateJsonTemplatedTest, array_prepend_json_str)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    arr_creator_t jac1;
    obj_creator_t joc_small;
    joc_small.append_value("mama", "mia");
    
    // prepend_json_str (which is an object) on empty object
    jac1.prepend_json_str(joc_small.c_str());
    EXPECT_STREQ(jac1.c_str(), R"|([{"mama": "mia"}])|");
    
    joc_small.clear();
    // prepend_json_str (which is an object) on object with values
    joc_small.append_value("dancing", "queen");
    jac1.prepend_json_str(joc_small.c_str());
    EXPECT_STREQ(jac1.c_str(), R"|([{"dancing": "queen"}, {"mama": "mia"}])|");
    
    jac1.clear();
    arr_creator_t jac_small;
    jac_small.append_value("mama", "mia");
    // prepend_json_str (which is an array) on empty object
    jac1.prepend_json_str(jac_small.c_str());
    EXPECT_STREQ(jac1.c_str(), R"|([["mama", "mia"]])|");
    
    jac_small.clear();
    // prepend_json_str (which is an array) on object with values
    jac_small.append_value("dancing", "queen");
    jac1.prepend_json_str(jac_small.c_str());
    EXPECT_STREQ(jac1.c_str(), R"|([["dancing", "queen"], ["mama", "mia"]])|");
}

TYPED_TEST(CreateJsonTemplatedTest, object_append_values_from)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    obj_creator_t joc1;
    obj_creator_t joc_small;
    joc1.append_values_from(joc_small);
    EXPECT_STREQ(joc1.c_str(), R"|({})|"); // append empty obj to to empty obj

    joc_small.append_value("mama", "mia");
    joc1.append_values_from(joc_small);
    EXPECT_STREQ(joc1.c_str(), R"|({"mama": "mia"})|");

    joc_small.clear();
    joc_small.append_value("forget", "me not");
    joc1.append_values_from(joc_small);
    EXPECT_STREQ(joc1.c_str(), R"|({"mama": "mia", "forget": "me not"})|");
}

TYPED_TEST(CreateJsonTemplatedTest, object_prepend_values_from)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    obj_creator_t joc1;
    obj_creator_t joc_small;
    joc1.prepend_values_from(joc_small);
    EXPECT_STREQ(joc1.c_str(), R"|({})|"); // prepend empty obj to to empty obj

    joc_small.append_value("mama", "mia");
    joc1.prepend_values_from(joc_small);
    EXPECT_STREQ(joc1.c_str(), R"|({"mama": "mia"})|");

    joc_small.clear();
    joc_small.append_value("forget", "me not");
    joc1.prepend_values_from(joc_small);
    EXPECT_STREQ(joc1.c_str(), R"|({"forget": "me not", "mama": "mia"})|");
}


TYPED_TEST(CreateJsonTemplatedTest, array_append_values_from)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    arr_creator_t jac1;
    arr_creator_t jac_small;
    jac1.append_values_from(jac_small);
    EXPECT_STREQ(jac1.c_str(), R"|([])|"); // append empty obj to to empty obj

    jac_small.append_value("mama", "mia");
    jac1.append_values_from(jac_small);
    EXPECT_STREQ(jac1.c_str(), R"|(["mama", "mia"])|");

    jac_small.clear();
    jac_small.append_value("forget", "me not");
    jac1.append_values_from(jac_small);
    EXPECT_STREQ(jac1.c_str(), R"|(["mama", "mia", "forget", "me not"])|");
}

TYPED_TEST(CreateJsonTemplatedTest, array_prepend_values_from)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    arr_creator_t jac1;
    arr_creator_t jac_small;
    jac1.prepend_values_from(jac_small);
    EXPECT_STREQ(jac1.c_str(), R"|([])|"); // append empty obj to to empty obj

    jac_small.append_value("mama", "mia");
    jac1.prepend_values_from(jac_small);
    EXPECT_STREQ(jac1.c_str(), R"|(["mama", "mia"])|");

    jac_small.clear();
    jac_small.append_value("forget", "me not");
    jac1.prepend_values_from(jac_small);
    EXPECT_STREQ(jac1.c_str(), R"|(["forget", "me not", "mama", "mia"])|");
}


TYPED_TEST(CreateJsonTemplatedTest, append_value_variadric)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    arr_creator_t jac1;
    jac1.append_value(17,
                      "18",
                      std::string("19"),
                      std::string_view("20"),
                      fstr::fstr31("21"));
    EXPECT_STREQ(jac1.c_str(), R"|([17, "18", "19", "20", "21"])|");
    
    obj_creator_t joc1;
    joc1.append_value("17", 17,
                      fstr::fstr31("19"), std::string("19"),
                      std::string("20"), std::string_view("20"),
                      std::string_view("21"), fstr::fstr31("21"));
    EXPECT_STREQ(joc1.c_str(), R"|({"17": 17, "19": "19", "20": "20", "21": "21"})|");
}

TYPED_TEST(CreateJsonTemplatedTest, append_all_number_types)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;
 
    arr_creator_t jac1;
    jac1.append_value((signed char){-126});
    jac1.append_value((unsigned char){202});
    jac1.append_value((short){-1234});
    jac1.append_value((unsigned short){1234});
    jac1.append_value(long{-123456789});
    jac1.append_value((unsigned long){123456789});
    jac1.append_value((long long){-987654321012345678});
    jac1.append_value((unsigned long long){9876543210123456789ULL});
    EXPECT_STREQ(jac1.c_str(), R"|([-126, 202, -1234, 1234, -123456789, 123456789, -987654321012345678, 9876543210123456789])|");
    
    jac1.clear();
    jac1.append_value((float){-12345.542969f});
    jac1.append_value((float){12345.542969f});
    EXPECT_STREQ(jac1.c_str(), R"|([-12345.542969, 12345.542969])|");

    jac1.clear();
    jac1.append_value((double){-1234512345.543216});
    jac1.append_value((double){1234512345.543216});
    EXPECT_STREQ(jac1.c_str(), R"|([-1234512345.543216, 1234512345.543216])|");

    jac1.clear();
    jac1.append_value((long double){-1234512345123.543213});
    jac1.append_value((long double){1234512345123.543213});
    EXPECT_STREQ(jac1.c_str(), R"|([-1234512345123.543213, 1234512345123.543213])|");
}

TYPED_TEST(CreateJsonTemplatedTest, append_all_string_types)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;
    
    arr_creator_t jac1;
    jac1.append_value("paprika A");
    jac1.append_value(std::string_view("paprika B"));
    jac1.append_value(std::string("paprika C"));
    jac1.append_value(fstr::fstr15("paprika D"));
    EXPECT_STREQ(jac1.c_str(), R"|(["paprika A", "paprika B", "paprika C", "paprika D"])|");
}

TYPED_TEST(CreateJsonTemplatedTest, append_all_misc_types)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;
    
    arr_creator_t jac1;
    jac1.append_value(true);
    jac1.append_value(false);
    jac1.append_value(nullptr);
    EXPECT_STREQ(jac1.c_str(), R"|([true, false, null])|");
}


TYPED_TEST(CreateJsonTemplatedTest, escape_from_array)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;
    
    auto backslash = "\\"sv;
    auto quote = "\""sv;
    auto tab = "\t"sv;
    auto newline = "\n"sv;
    auto bell = "\b"sv;
    auto carriage_return = "\r"sv;

    arr_creator_t jac1;
    jac1.append_value(backslash);
    jac1.append_value(quote);
    jac1.append_value(tab);
    jac1.append_value(newline);
    jac1.append_value(bell);
    jac1.append_value(carriage_return);
    fstr::fstr63 sv1 = jac1;
    fstr::fstr63 sv_expected = R"(["\\", "\"", "\t", "\n", "\b", "\r"])";
//    std::cout << "sv_expected: " << sv1 << std::endl;
//    std::cout << "sv1: " << sv_expected << std::endl;
    EXPECT_EQ(sv1, sv_expected);
}

TYPED_TEST(CreateJsonTemplatedTest, escape_from_object)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;
    
    auto backslash = "\\"sv;
    auto quote = "\""sv;
    auto tab = "\t"sv;
    auto newline = "\n"sv;
    auto bell = "\b"sv;
    auto carriage_return = "\r"sv;

    obj_creator_t jac1;
    jac1.append_value("backslash", backslash);
    jac1.append_value("quote", quote);
    jac1.append_value("tab", tab);
    jac1.append_value("newline", newline);
    jac1.append_value("bell", bell);
    jac1.append_value("carriage_return", carriage_return);

    jac1.append_value(backslash, "backslash");
    jac1.append_value(quote, "quote");
    jac1.append_value(tab, "tab");
    jac1.append_value(newline, "newline");
    jac1.append_value(bell, "bell");
    jac1.append_value(carriage_return, "carriage_return");

    std::string_view sv1 = jac1;
    std::string_view sv_expected = R"({"backslash": "\\", "quote": "\"", "tab": "\t", "newline": "\n", "bell": "\b", "carriage_return": "\r", "\\": "backslash", "\"": "quote", "\t": "tab", "\n": "newline", "\b": "bell", "\r": "carriage_return"})";
//    std::cout << "sv_expected: " << sv1 << std::endl;
//    std::cout << "sv1: " << sv_expected << std::endl;
    EXPECT_EQ(sv1, sv_expected);
}

TYPED_TEST(CreateJsonTemplatedTest, square_brackets)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;
    
    
    obj_creator_t joc1;
    joc1["Amii"] = "Stewart";
    joc1["year"] = 1956;
    joc1["mark"] = 10.0;
    joc1["good"] = true;
    joc1["bad"] = false;
    joc1["where"] = nullptr;
    
    EXPECT_STREQ(joc1.c_str(), R"({"Amii": "Stewart", "year": 1956, "mark": 10.0, "good": true, "bad": false, "where": null})");
}


TYPED_TEST(CreateJsonTemplatedTest, if_array_if_object)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    if (arr_creator_t j_array;
        j_array)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (auto j_sub_obj = j_array.append_object();
                j_sub_obj)
            {
                j_sub_obj["i"] = i;
            }
            else
            {
                EXPECT_TRUE(false);
            }
        }
        std::string_view expected = R"([{"i": 0}, {"i": 1}, {"i": 2}])";
        EXPECT_EQ(expected, std::string_view(j_array));
    }
    else
    {
        EXPECT_TRUE(false);
    }
}

TYPED_TEST(CreateJsonTemplatedTest, if_object_if_array)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;

    if (obj_creator_t j_object;
        j_object)
    {
        if (auto j_sub_array = j_object.append_array("arrrrr");
            j_sub_array)
        {
            for (int i = 0; i < 3; ++i)
            {
                j_sub_array.push_back(i);
            }
        }
        else
        {
            EXPECT_TRUE(false);
        }
        std::string_view expected = R"({"arrrrr": [0, 1, 2]})";
        EXPECT_EQ(expected, std::string_view(j_object));
    }
    else
    {
        EXPECT_TRUE(false);
    }
}
