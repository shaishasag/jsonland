#define NDEBUG

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"
#include "nlohmann/json.hpp"
#include "jsonland/json_node.h"

static const char* RapidJsonTypeName(rapidjson::Type in_type)
{
    const char* retVal = "Unknown";
    switch (in_type)
    {
        case rapidjson::kNullType: retVal = "kNullType"; break;
        case rapidjson::kFalseType: retVal = "kFalseType"; break;
        case rapidjson::kTrueType: retVal = "kTrueType"; break;
        case rapidjson::kObjectType: retVal = "kObjectType"; break;
        case rapidjson::kArrayType: retVal = "kArrayType"; break;
        case rapidjson::kStringType: retVal = "kStringType"; break;
        case rapidjson::kNumberType: retVal = "kNumberType"; break;
    }
    return retVal;
}
static const char* NlohmannTypeName(nlohmann::detail::value_t in_type)
{
    const char* retVal = "Unknown";
    switch (in_type)
    {
        case nlohmann::detail::value_t::null: retVal = "null"; break;
        case nlohmann::detail::value_t::object: retVal = "object"; break;
        case nlohmann::detail::value_t::array: retVal = "array"; break;
        case nlohmann::detail::value_t::string: retVal = "string"; break;
        case nlohmann::detail::value_t::boolean: retVal = "boolean"; break;
        case nlohmann::detail::value_t::number_integer: retVal = "number_integer"; break;
        case nlohmann::detail::value_t::number_unsigned: retVal = "number_unsigned"; break;
        case nlohmann::detail::value_t::number_float: retVal = "number_float"; break;
        case nlohmann::detail::value_t::binary: retVal = "binary"; break;
        case nlohmann::detail::value_t::discarded: retVal = "discarded"; break;
    }
    return retVal;
}


int main(int argc, char* argv[])
{
    // what happens when creating a string node and treating it like an array?
    
    
    
    try { // nlohmann throws unless object is an array or null
        nlohmann::json str_nloh_j("nlohmann json string");
        std::cout << "nlohmann::json before: (" << NlohmannTypeName(str_nloh_j.type()) << "): " << str_nloh_j.dump() << std::endl;
        str_nloh_j.push_back(123);
        std::cout << "nlohmann::json after: (" << NlohmannTypeName(str_nloh_j.type()) << "): " << str_nloh_j.dump() << std::endl;
    } catch (nlohmann::detail::exception& ex) {
        std::cout << "nlohmann::json exception: " << ex.what() << std::endl;
    }
    
    try { // rapid asserts unless object is an array
        rapidjson::Document doc;
        rapidjson::Value str_rapid_j("rapid json string");
        std::cout << "rapid before (" << RapidJsonTypeName(str_rapid_j.GetType()) << "): '" << str_rapid_j.GetString() << "'" << std::endl;
        // if NDEBUG is not defined: this will assert and crash
        // if NDEBUG is defined: string value will be erased but type wil remain kStringType
        str_rapid_j.PushBack(rapidjson::Value(123), doc.GetAllocator());
        std::cout << "rapid after (" << RapidJsonTypeName(str_rapid_j.GetType()) << "): '" << str_rapid_j.GetString() << "'" << std::endl;
    }
    catch (std::exception& ex) {
        std::cout << "rapid exception: " << ex.what() << std::endl;
    }
    
    try { // nlohmann throws unless object is an array or null
        jsonland::json_node str_land_j("nlohmann json string");
        std::cout << "jsonland::json_node before: (" << node_type_name(str_land_j.type()) << "): " << str_land_j.dump() << std::endl;
        str_land_j.push_back(123);
        std::cout << "jsonland::json_node after: (" << node_type_name(str_land_j.type()) << "): " << str_land_j.dump() << std::endl;
    } catch (std::exception& ex) {
        std::cout << "jsonland exception: " << ex.what() << std::endl;
    }

}

