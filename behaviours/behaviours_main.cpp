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

std::string RapidJsonStringify(rapidjson::Value& rapid_val)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    rapid_val.Accept(writer);
    return sb.GetString();
}
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


void string_as_array(int argc, char* argv[])
{
    std::cout << "--- " << "!string_as_array" << std::endl;
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
        std::cout << "jsonland::json_node before: (" << jsonland::value_type_name(str_land_j.value_type()) << "): " << str_land_j.dump() << std::endl;
        str_land_j.push_back(123);
        std::cout << "jsonland::json_node after: (" << jsonland::value_type_name(str_land_j.value_type()) << "): " << str_land_j.dump() << std::endl;
    } catch (std::exception& ex) {
        std::cout << "jsonland exception: " << ex.what() << std::endl;
    }
    
    std::cout << "..." << std::endl;
}

void as_string()
{
    std::cout << "--- " << "!as_string" << std::endl;
    {   // get the value as string for any type : jsonland
        jsonland::json_node s1("I am a string");
        jsonland::json_node n1(1234567);
        jsonland::json_node b1(true);
        jsonland::json_node p1(nullptr);
        
        std::cout << "jsonland: " << jsonland::value_type_name(s1.value_type()) << " node: " << s1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(n1.value_type()) << " node: " << n1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(b1.value_type()) << " node: " << b1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(p1.value_type()) << " node: " << p1.dump() << std::endl;
    }
    
    std::cout << std::endl;
    
    {   // get the value as string for any type : nlohmann
        nlohmann::json s1("I am a string");
        nlohmann::json n1(1234567);
        nlohmann::json b1(true);
        nlohmann::json p1(nullptr);
        
        std::cout << "nlohmann: " << NlohmannTypeName(s1.type()) << " node: " << s1.dump() << std::endl;
        std::cout << "nlohmann: " << NlohmannTypeName(n1.type()) << " node: " << n1.dump() << std::endl;
        std::cout << "nlohmann: " << NlohmannTypeName(b1.type()) << " node: " << b1.dump() << std::endl;
        std::cout << "nlohmann: " << NlohmannTypeName(p1.type()) << " node: " << p1.dump() << std::endl;
    }
    
    std::cout << std::endl;
    
    {   // get the value as string for any type : rapidjson
        rapidjson::Value s1("I am a string");
        rapidjson::Value n1(1234567);
        rapidjson::Value b1(true);
        rapidjson::Value p1(rapidjson::kNullType);
        
        std::cout << "rapidjson: " << RapidJsonTypeName(s1.GetType()) << " node: " << RapidJsonStringify(s1) << std::endl;
        std::cout << "rapidjson: " << RapidJsonTypeName(n1.GetType()) << " node: " << RapidJsonStringify(n1) << std::endl;
        std::cout << "rapidjson: " << RapidJsonTypeName(b1.GetType()) << " node: " << RapidJsonStringify(b1) << std::endl;
        std::cout << "rapidjson: " << RapidJsonTypeName(p1.GetType()) << " node: " << RapidJsonStringify(p1) << std::endl;
    }
    
    std::cout << "..." << std::endl;
}

void operator_equal()
{
    std::cout << "--- " << "!operator_equal" << std::endl;
    // what is you use operator= on a non object
    {
        jsonland::json_node s1("I am a string");
        auto s2 = s1["pilpel"];
        jsonland::json_node n1(jsonland::null_t);
        auto n2 = n1["pilpel"];

        std::cout << "jsonland: " << jsonland::value_type_name(s1.value_type()) << " node: " << s1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(s2.value_type()) << " node: " << s2.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(n1.value_type()) << " node: " << n1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(n2.value_type()) << " node: " << n2.dump() << std::endl;
    }
    
    std::cout << std::endl;
    
    {
        try
        {
            nlohmann::json s1("I am a string");
            auto& s2 = s1["pilpel"];
            std::cout << "nlohmann: " << NlohmannTypeName(s1.type()) << " node: " << s1.dump() << std::endl;
            std::cout << "nlohmann: " << NlohmannTypeName(s2.type()) << " node: " << s2.dump() << std::endl;
        }
        catch(nlohmann::detail::type_error& ex)
        {
            std::cout << "nlohmann: " << "throws calling operator[] on string value" << std::endl;
        }
    }
 
    nlohmann::json s1;
    auto& s2 = s1["pilpel"];
    std::cout << "nlohmann: " << NlohmannTypeName(s1.type()) << " node: " << s1.dump() << std::endl;
    std::cout << "nlohmann: " << NlohmannTypeName(s2.type()) << " node: " << s2.dump() << std::endl;

    std::cout << std::endl;
    
    {   // get the value as string for any type : rapidjson
        rapidjson::Value s1("I am a string");
        auto& s2 = s1["pilpel"];
        std::cout << "rapidjson: " << RapidJsonTypeName(s1.GetType()) << " node: " << RapidJsonStringify(s1) << std::endl;
        std::cout << "rapidjson: " << RapidJsonTypeName(s2.GetType()) << " node: " << RapidJsonStringify(s2) << std::endl;

        rapidjson::Value o1(rapidjson::kObjectType);
        auto& o2 = o1["mamaliga"];
        std::cout << "rapidjson: " << RapidJsonTypeName(o1.GetType()) << " node: " << RapidJsonStringify(o1) << std::endl;
        std::cout << "rapidjson: " << RapidJsonTypeName(o2.GetType()) << " node: " << RapidJsonStringify(o2) << std::endl;
    }
    
    std::cout << "..." << std::endl;
}

int main(int argc, char* argv[])
{
    as_string();
    operator_equal();
}
