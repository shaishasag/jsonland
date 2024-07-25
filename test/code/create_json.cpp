#include "gtest/gtest.h"
#include <string_view>
using namespace std::literals;

#include "fstring/fstring.h"
#include "jsonland/json_creator.h"

// tests are templated to check the two types of json creators:
// - object_json_creator & array_json_creator based on fixed::fstring
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
                      fixed::fstring31("21"));
    EXPECT_STREQ(jac1.c_str(), R"|([17, "18", "19", "20", "21"])|");
    
    obj_creator_t joc1;
    joc1.append_value("17", 17,
                      fixed::fstring31("19"), std::string("19"),
                      std::string("20"), std::string_view("20"),
                      std::string_view("21"), fixed::fstring31("21"));
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
    jac1.append_value(fixed::fstring15("paprika D"));
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

#include <nlohmann/json.hpp>
// using nlohmann for parsing to make sure creating json with escaped char works

TYPED_TEST(CreateJsonTemplatedTest, escape_from_array)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;
    
    auto with_backslash = "it's a backward slash: \\"sv;
    auto with_quoate = "it's a quote: \""sv;
    auto with_tab = "it's a tab: \t"sv;
    auto with_newline = "it's a newline: \n"sv;
    auto with_bell = "it's a bell: \b"sv;

    arr_creator_t jac1;
    jac1.append_value(with_backslash);
    jac1.append_value(with_quoate);
    jac1.append_value(with_tab);
    jac1.append_value(with_newline);
    jac1.append_value(with_bell);
    
    nlohmann::json nj = nlohmann::json::parse(jac1.c_str());
    EXPECT_EQ(nj[0].get<std::string_view>(), with_backslash);
    EXPECT_EQ(nj[1].get<std::string_view>(), with_quoate);

    EXPECT_EQ(nj[2].get<std::string_view>(), with_tab);
    EXPECT_EQ(nj[3].get<std::string_view>(), with_newline);
    EXPECT_EQ(nj[4].get<std::string_view>(), with_bell);
}

TYPED_TEST(CreateJsonTemplatedTest, escape_from_object)
{
    using obj_creator_t = typename TypeParam::first_type;
    using arr_creator_t = typename TypeParam::second_type;
    
    auto a_backslash = "\\"s;
    auto a_quoate = "\""s;
    auto a_tab = "\t"s;
    auto a_newline = "\n"s;
    auto a_bell = "\b"s;
    
    obj_creator_t joc1;
    joc1.append_value(a_backslash, "backslash");
    joc1.append_value(a_quoate, "quoate");
    joc1.append_value(a_tab, "tab");
    joc1.append_value(a_newline, "newline");
    joc1.append_value(a_bell, "bell");
    
    nlohmann::json nj = nlohmann::json::parse(joc1.c_str());
    EXPECT_EQ(nj[a_backslash].get<std::string_view>(), "backslash");
    EXPECT_EQ(nj[a_quoate].get<std::string_view>(), "quoate");
    EXPECT_EQ(nj[a_tab].get<std::string_view>(), "tab");
    EXPECT_EQ(nj[a_newline].get<std::string_view>(), "newline");
    EXPECT_EQ(nj[a_bell].get<std::string_view>(), "bell");
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
