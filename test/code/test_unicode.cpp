#include "gtest/gtest.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "nlohmann/json.hpp"
#include "jsonland/json_node.h"

using namespace jsonland;

TEST(Unicode, compare_escape_parse)
{
    const char* json_text = R"({"\uD83C\uDFE1": "house", "post-office" : "\uD83C\uDFE3", "anabanana":"\ud83c\udf4c\t\ud83c\udf4d"})";

    rapidjson::Document document;
    document.Parse(json_text);
    if (document.IsObject())
    {
        for (rapidjson::Value::ConstMemberIterator iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter){
            std::cout << "Rapidjson: " << iter->name.GetString() << " => " <<  iter->value.GetString() << "\n";
        }

    }

    nlohmann::json nj = nlohmann::json::parse(json_text);
    for (nlohmann::json::iterator it = nj.begin(); it != nj.end(); ++it)
    {
        std::cout << "Nlohmann: " << it.key() << " => " <<  it.value() << "\n";
    }


    jsonland::json_doc landoc;
    int parse_error = landoc.parse(json_text);
    for (auto& node : landoc)
    {
        std::cout << "jsonland: " << node.key() << " => " <<  node.as_string() << "\n";
    }
}

TEST(Unicode, compare_unescape_parse)
{
    const char* json_text = R"({"🏡": "house", "post-office" : "🏣", "anabanana":"🍌🍍"})";

    rapidjson::Document document;
    document.Parse(json_text);
    if (document.IsObject())
    {
        for (rapidjson::Value::ConstMemberIterator iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter){
            std::cout << "Rapidjson: " << iter->name.GetString() << " => " <<  iter->value.GetString() << "\n";
        }

    }

    nlohmann::json nj = nlohmann::json::parse(json_text);
    for (nlohmann::json::iterator it = nj.begin(); it != nj.end(); ++it)
    {
        std::cout << "Nlohmann: " << it.key() << " => " <<  it.value() << "\n";
    }


    jsonland::json_doc landoc;
    int parse_error = landoc.parse(json_text);
    std::cout << "jsonland: " << "house" << " => " <<  landoc["house"].as_string() << "\n";
    std::cout << "jsonland: " << "post-office" << " => " <<  landoc["post-office"].as_string() << "\n";
}


TEST(Unicode, compare_unescaped_initialization)
{
    rapidjson::Document rapido(rapidjson::kObjectType);
    rapido.AddMember("🏡", "house", rapido.GetAllocator());
    rapido.AddMember("post-office", "🏣", rapido.GetAllocator());
    rapido.AddMember("Tab", "T'\t'b", rapido.GetAllocator());
    rapido.AddMember("T'\t'b", "Tab", rapido.GetAllocator());
    if (rapido.IsObject())
    {
        for (rapidjson::Value::ConstMemberIterator iter = rapido.MemberBegin(); iter != rapido.MemberEnd(); ++iter){
            std::cout << "Rapidjson: " << iter->name.GetString() << " => " <<  iter->value.GetString() << "\n";
        }
    }

    std::cout << "---";
    
    nlohmann::json nj = nlohmann::json::object();
    nj["🏡"] = "house";
    nj["post-office"] = "🏣";
    nj["Tab"] = "T'\t'b";
    nj["T'\t'b"] = "Tab";
    for (nlohmann::json::iterator it = nj.begin(); it != nj.end(); ++it)
    {
        std::cout << "Nlohmann: " << it.key() << " => " <<  it.value() << "\n";
    }

    std::cout << "---";
    
    jsonland::json_doc landoc(jsonland::node_type::_object);
    landoc["🏡"] = "🏣";
    landoc["🏡"] = "house";
    landoc["post-office"] = "🏣";
    landoc["Tab"] = "T'\t'b";
    landoc["T'\t'b"] = "Tab";

    for (auto& node : landoc)
    {
        std::cout << "jsonland: " << node.key() << " => " <<  node.as_string() << "\n";
    }
}
