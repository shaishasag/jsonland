#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include <iostream>
#include "escape.h"

using namespace jsonland;
using namespace jsonland::escapism;

TEST(EscapeInfra, some_escapes)
{
    using EscapedAndUn = std::tuple<std::string_view, std::string_view, unescape_result>;
    using EscapedAndUnVec = std::vector<EscapedAndUn>;
    EscapedAndUnVec v1({
        {R"(abc)", R"(abc)", nothing_to_unescape},
        {R"(a☺c)", R"(a☺c)", nothing_to_unescape},
        {R"(a\\c)", R"(a\c)", something_was_unescaped},
        {R"(a\"b)", R"(a"b)", something_was_unescaped},
        {R"(a\tb)", "a\tb", something_was_unescaped},
        {R"(a\rb)", "a\rb", something_was_unescaped},
        {R"(a\nb)", "a\nb", something_was_unescaped},
        {R"(a\fb)", "a\fb", something_was_unescaped},
        {R"(a\bb)", "a\bb", something_was_unescaped},
        {R"(a\/b)", R"(a/b)", something_was_unescaped},
        {R"(a\\b)", R"(a\b)", something_was_unescaped},
        {"a\\u263A" "c", "a☺c", something_was_unescaped},
    });

    for (auto& EandUn : v1)
    {
        std::string rv;
        unescape_result res = unescape_json_string(std::get<0>(EandUn), rv);
        ASSERT_EQ(res, std::get<2>(EandUn)) << "unescape_json_string result does not match expected";
        if (res == nothing_to_unescape)
        {
            EXPECT_EQ(std::get<0>(EandUn), std::get<1>(EandUn)); // EandUn.first did not change
        }
        else if (res == something_was_unescaped)
        {
            EXPECT_EQ(rv, std::get<1>(EandUn)); // rv has the uescaped string
        }
    }
}

TEST(EscapeJson, parse_and_dump)
{
    std::string parse_me = R"({"backslash":"\\",
                                    "quote":"\"",
                                    "tab":"\t",
                                    "newline":"\n",
                                    "bell":"\b",
                                    "carriage_return":"\r",
                                    "\\":"backslash",
                                    "\"":"quote",
                                    "\t":"tab",
                                    "\n":"newline",
                                    "\b":"bell",
                                    "\r":"carriage_return"})";

    jsonland::json_doc jdoc;
    jdoc.parse(parse_me);

    // check the escaped values, check each twice incase there was internal change
    // all permutations of 2 X get_string + 2 X dump are tried
    {
        EXPECT_EQ(jdoc["backslash"].get_string(), "\\");
        EXPECT_EQ(jdoc["backslash"].get_string(), "\\");
        EXPECT_EQ(jdoc["backslash"].dump(), "\"\\\\\"");
        EXPECT_EQ(jdoc["backslash"].dump(), "\"\\\\\"");

        EXPECT_EQ(jdoc["quote"].get_string(), "\"");
        EXPECT_EQ(jdoc["quote"].dump(), "\"\\\"\"");
        EXPECT_EQ(jdoc["quote"].get_string(), "\"");
        EXPECT_EQ(jdoc["quote"].dump(), "\"\\\"\"");

        EXPECT_EQ(jdoc["tab"].dump(), "\"\\t\"");
        EXPECT_EQ(jdoc["tab"].dump(), "\"\\t\"");
        EXPECT_EQ(jdoc["tab"].get_string(), "\t");
        EXPECT_EQ(jdoc["tab"].get_string(), "\t");

        EXPECT_EQ(jdoc["newline"].dump(), "\"\\n\"");
        EXPECT_EQ(jdoc["newline"].get_string(), "\n");
        EXPECT_EQ(jdoc["newline"].dump(), "\"\\n\"");
        EXPECT_EQ(jdoc["newline"].get_string(), "\n");

        EXPECT_EQ(jdoc["bell"].get_string(), "\b");
        EXPECT_EQ(jdoc["bell"].get_string(), "\b");
        EXPECT_EQ(jdoc["bell"].dump(), "\"\\b\"");
        EXPECT_EQ(jdoc["bell"].dump(), "\"\\b\"");

        EXPECT_EQ(jdoc["carriage_return"].dump(), "\"\\r\"");
        EXPECT_EQ(jdoc["carriage_return"].dump(), "\"\\r\"");
        EXPECT_EQ(jdoc["carriage_return"].get_string(), "\r");
        EXPECT_EQ(jdoc["carriage_return"].get_string(), "\r");
    }

    // check the escaped keys, check each twice incase there was internal change
    // all permutations of 2 X get_string + 2 X dump are tried
    {
        EXPECT_EQ(jdoc["\\"].get_string(), "backslash");
        EXPECT_EQ(jdoc["\\"].get_string(), "backslash");
        EXPECT_EQ(jdoc["\\"].dump(), "\"backslash\"");
        EXPECT_EQ(jdoc["\\"].dump(), "\"backslash\"");

        EXPECT_EQ(jdoc["\""].get_string(), "quote");
        EXPECT_EQ(jdoc["\""].dump(), "\"quote\"");
        EXPECT_EQ(jdoc["\""].get_string(), "quote");
        EXPECT_EQ(jdoc["\""].dump(), "\"quote\"");

        EXPECT_EQ(jdoc["\t"].dump(), "\"tab\"");
        EXPECT_EQ(jdoc["\t"].dump(), "\"tab\"");
        EXPECT_EQ(jdoc["\t"].get_string(), "tab");
        EXPECT_EQ(jdoc["\t"].get_string(), "tab");

        EXPECT_EQ(jdoc["\n"].dump(), "\"newline\"");
        EXPECT_EQ(jdoc["\n"].get_string(), "newline");
        EXPECT_EQ(jdoc["\n"].dump(), "\"newline\"");
        EXPECT_EQ(jdoc["\n"].get_string(), "newline");

        EXPECT_EQ(jdoc["\b"].get_string(), "bell");
        EXPECT_EQ(jdoc["\b"].get_string(), "bell");
        EXPECT_EQ(jdoc["\b"].dump(), "\"bell\"");
        EXPECT_EQ(jdoc["\b"].dump(), "\"bell\"");

        EXPECT_EQ(jdoc["\r"].dump(), "\"carriage_return\"");
        EXPECT_EQ(jdoc["\r"].dump(), "\"carriage_return\"");
        EXPECT_EQ(jdoc["\r"].get_string(), "carriage_return");
        EXPECT_EQ(jdoc["\r"].get_string(), "carriage_return");
   }

    std::string dump_me = jdoc.dump();
    // for readability parse_me has new lines and spaces, remove them for comparing
    parse_me.erase(std::remove_if(parse_me.begin(), parse_me.end(), ::isspace), parse_me.end());
    dump_me.erase(std::remove_if(dump_me.begin(), dump_me.end(), ::isspace), dump_me.end());

    EXPECT_EQ(dump_me, parse_me);
}


TEST(EscapeJson, parse_and_get)
{
//    std::string_view parse_me = R"({"backslash":"\\","quote":"\"","tab":"\t","newline":"\n","bell":"\b","carriage_return":"\r","\\":"backslash","\"":"quote","\t":"tab","\n":"newline","\b":"bell","\r":"carriage_return"})";
    std::string_view parse_me = "{\"quote\":\"\\\"\"}";

    jsonland::json_doc jdoc;
    jdoc.parse(parse_me);

    std::string_view sv = jdoc["quote"].get_string();
    EXPECT_EQ(sv.size(), 1);
    EXPECT_EQ(sv[0], '"');
}


TEST(EscapeJson, backslashes)
{
    json_node jo(object_t);

    {
        // from char*
        jo["win_path"] = "c:\\something\\something";
        std::string take_a_dump = jo.dump();
//        std::cout << take_a_dump << std::endl;
        EXPECT_EQ(take_a_dump, "{\"win_path\":\"c:\\\\something\\\\something\"}");
    }
    {
        // from std::string_view
        jo["win_path"] = "c:\\something\\something"sv;
        std::string take_a_dump = jo.dump();
        EXPECT_EQ(take_a_dump, "{\"win_path\":\"c:\\\\something\\\\something\"}");
    }
}

TEST(EscapeJson, escape_and_copy)
{
    auto some_win_path = "c:\\something\\something"sv;

    json_node jo(object_t);
    jo["win_path"sv] = some_win_path;
//    std::cout << jo["win_path"sv].dump() << std::endl;
//    std::cout << jo.dump() << std::endl;
    EXPECT_EQ(jo.dump(), R"({"win_path":"c:\\something\\something"})"sv);
    EXPECT_EQ(jo["win_path"sv].dump(), R"("c:\\something\\something")"sv);
    EXPECT_EQ(jo["win_path"sv].get_string(), R"(c:\something\something)"sv);


//    std::cout << "jo.dump() " << jo.dump() << std::endl;
//    std::cout << "jo[\"win_path\"].dump() " << jo["win_path"sv].dump() << std::endl;
    jo["copy node"sv] = jo["win_path"sv].clone();
//    std::cout << "jo.dump() " << jo.dump() << std::endl;
//    std::cout << "jo[\"copy node\"].dump() " << jo["copy node"sv].dump() << std::endl;


    std::string dumps = jo["copy node"sv].dump();
    EXPECT_EQ(jo["copy node"sv].dump(), R"("c:\\something\\something")"sv);
    EXPECT_EQ(jo["copy node"sv].get_string(), R"(c:\something\something)"sv);
}
