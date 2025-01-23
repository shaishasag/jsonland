#include "gtest/gtest.h"
#include <string_view>
#include <iostream>
#include <fstream>

using namespace std::literals;

#include "jsonland/json_node.h"

static void trim_trailing_whitespace(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static bool compare_files(const std::filesystem::path& lhp, const std::filesystem::path& rhp)
{
    std::ifstream lhf(lhp);
    std::ifstream rhf(rhp);

    std::string lhc{std::istreambuf_iterator<char>(lhf), {}};
    trim_trailing_whitespace(lhc);

    std::string rhc{std::istreambuf_iterator<char>(rhf), {}};
    trim_trailing_whitespace(rhc);

    if (lhc.size() != rhc.size())
    {
        std::cout << "different sizes " << lhc.size() <<  " != "  << rhc.size() << std::endl;
    }
    else
    {
        int line = 1;
        int char_in_line = 1;
        for (int i = 0; i < lhc.size(); ++i)
        {
            if (lhc[i] != rhc[i])
            {
                std::cout << "different at line " << line <<  ":"  << char_in_line << std::endl;
            }
            if (lhc[i] == '\n')
            {
                ++line;
                char_in_line = 1;
            }
            else
            {
                ++char_in_line;
            }
        }
    }
    return lhc == rhc;
}

// parse a test file with all kinds of special chars, escapes and unicode
// dump in 2 styles: tight and pretty, compare against reference files
TEST(ReadWriteCompare, file)
{
    std::filesystem::path test_files_folder = std::filesystem::path(__FILE__).parent_path().parent_path();
    std::filesystem::path file_to_read = test_files_folder / "test.source.json"sv;
    std::filesystem::path reference_tight_file_path =  test_files_folder / "test.reference.tight.json"sv;
    std::filesystem::path reference_pretty_file_path = file_to_read;
    std::filesystem::path output_tight_file_path = test_files_folder / "test.output.tight.json"sv;
    std::filesystem::path output_pretty_file_path = test_files_folder / "test.output.pretty.json"sv;

    jsonland::json_doc jdoc;
    std::ifstream ifs(file_to_read);
    std::string contents((std::istreambuf_iterator<char>(ifs)), {});
    int parse_result = jdoc.parse_insitu(contents);
    ASSERT_EQ(parse_result, 0) << jdoc.parse_error_message();

    {
        std::ofstream out_tight(output_tight_file_path);
        out_tight << jdoc.dump(jsonland::dump_style::tight);
        std::ofstream out_pretty(output_pretty_file_path);
        out_pretty << jdoc.dump(jsonland::dump_style::pretty);
    }

    EXPECT_TRUE(compare_files(reference_tight_file_path, output_tight_file_path)) << "dump(tight) output!=reference";
    EXPECT_TRUE(compare_files(reference_pretty_file_path, output_pretty_file_path)) << "dump(pretty) output!=reference";
}

// access the keys with escaped char programatically
TEST(ReadWriteCompare, access_weird_keys)
{
    std::filesystem::path test_files_folder = std::filesystem::path(__FILE__).parent_path().parent_path();
    std::filesystem::path file_to_read = test_files_folder / "test.source.json"sv;
    jsonland::json_doc jdoc;
    std::ifstream ifs(file_to_read);
    std::string contents((std::istreambuf_iterator<char>(ifs)), {});
    int parse_result = jdoc.parse_insitu(contents);
    ASSERT_EQ(parse_result, 0) << jdoc.parse_error_message();

    auto& escaped_keys_j = jdoc["object_keys_variations"]["escaped_characters"];

    ASSERT_EQ(escaped_keys_j["\"key_with_double_quotes\""].get_string(), "value"sv);
    ASSERT_EQ(escaped_keys_j["key_with\\backslash"].get_string(), "value"sv);
    ASSERT_EQ(escaped_keys_j["key_with/slash"].get_string(), "value"sv);
    ASSERT_EQ(escaped_keys_j["key_with\bbackspace"].get_string(), "value"sv);
    ASSERT_EQ(escaped_keys_j["key_with\fFormfeed"].get_string(), "value"sv);
    ASSERT_EQ(escaped_keys_j["key_with\nnewline"].get_string(), "value"sv);
    ASSERT_EQ(escaped_keys_j["key_with\rcarriage_return"].get_string(), "value"sv);
    ASSERT_EQ(escaped_keys_j["key_with\ttab"].get_string(), "value"sv);

    auto& unicode_keys_j = jdoc["object_keys_variations"]["unicode_keys"];
    ASSERT_EQ(unicode_keys_j["中文"].get_string(), "value"sv);
    ASSERT_EQ(unicode_keys_j["Русский"].get_string(), "value"sv);
    ASSERT_EQ(unicode_keys_j["हिन्दी"].get_string(), "value"sv);
    ASSERT_EQ(unicode_keys_j["العربية"].get_string(), "value"sv);
    ASSERT_EQ(unicode_keys_j["日本語"].get_string(), "value"sv);
    ASSERT_EQ(unicode_keys_j["한국어"].get_string(), "value"sv);
    ASSERT_EQ(unicode_keys_j["😀😂🥺👍"].get_string(), "value"sv);

    auto& whitespace_keys_j = jdoc["object_keys_variations"]["whitespace_keys"];
    ASSERT_EQ(whitespace_keys_j[" key_with_space"].get_string(), "value"sv);
    ASSERT_EQ(whitespace_keys_j["\tkey_with_tab"].get_string(), "value"sv);
    ASSERT_EQ(whitespace_keys_j["\nkey_with_newline"].get_string(), "value"sv);
}

TEST(ReadWriteCompare, ownership_after_parse)
{
    std::filesystem::path test_files_folder = std::filesystem::path(__FILE__).parent_path().parent_path();
    std::filesystem::path file_to_read = test_files_folder / "test.source.json"sv;
    jsonland::json_doc jdoc;
    std::ifstream ifs(file_to_read);
    std::string contents((std::istreambuf_iterator<char>(ifs)), {});
    int parse_result = jdoc.parse_insitu(contents);
    ASSERT_EQ(parse_result, 0) << jdoc.parse_error_message();

    ASSERT_TRUE(jdoc.refers_to_external_memory());
    ASSERT_FALSE(jdoc.is_full_owner());

    jdoc["somebody"] = "somewhere";                 // when assiging aditional value to parsed json
    ASSERT_TRUE(jdoc.refers_to_external_memory());  // jdoc still refers to external memory
    ASSERT_FALSE(jdoc.is_full_owner());             // jdoc still is not a full owner

    jsonland::json_node jnode = jdoc.clone();
    ASSERT_FALSE(jnode.refers_to_external_memory());
    ASSERT_TRUE(jnode.is_full_owner());

    jnode["somewhere"] = "somebody";                  // when assiging aditional value to cloned json
    ASSERT_FALSE(jnode.refers_to_external_memory());  // jdoc still does not refers to external memory
    ASSERT_TRUE(jnode.is_full_owner());               // jdoc still is a full owner
}
