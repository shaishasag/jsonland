#include "gtest/gtest.h"
#include "jsonland/json_node.h"
#include <iostream>

using namespace jsonland;

TEST(Escape, parse_and_dump)
{
    std::string_view parse_me = R"({"backslash":"\\","quote":"\"","tab":"\t","newline":"\n","bell":"\b","carriage_return":"\r","\\":"backslash","\"":"quote","\t":"tab","\n":"newline","\b":"bell","\r":"carriage_return"})";

    jsonland::json_doc jdoc;
    jdoc.parse(parse_me);
    std::string dump_me = jdoc.dump();

    EXPECT_EQ(dump_me, parse_me);
}


TEST(Escape, parse_and_get)
{
//    std::string_view parse_me = R"({"backslash":"\\","quote":"\"","tab":"\t","newline":"\n","bell":"\b","carriage_return":"\r","\\":"backslash","\"":"quote","\t":"tab","\n":"newline","\b":"bell","\r":"carriage_return"})";
    std::string_view parse_me = "{\"quote\":\"\\\"\"}";

    jsonland::json_doc jdoc;
    jdoc.parse(parse_me);

    std::string_view sv = jdoc["quote"].get_string();
    EXPECT_EQ(sv.size(), 2);
    EXPECT_EQ(sv, "\\\""sv);
}


TEST(Escape, backslashes)
{
    json_node jo(object_t);

    {
        // from char*
        jo["win_path"] = "c:\\something\\something";
        std::string take_a_dump = jo.dump();
        std::cout << take_a_dump << std::endl;
        EXPECT_EQ(take_a_dump, "{\"win_path\":\"c:\\\\something\\\\something\"}");
    }
    {
        // from std::string_view
        jo["win_path"] = "c:\\something\\something"sv;
        std::string take_a_dump = jo.dump();
        std::cout << take_a_dump << std::endl;
        EXPECT_EQ(take_a_dump, "{\"win_path\":\"c:\\\\something\\\\something\"}");
    }
}