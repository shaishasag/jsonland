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
#include "fstring.h"

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


static void string_as_array()
{
    std::cout << "--- " << "!string_as_array" << std::endl;
    // what happens when creating a string node and treating it like an array?
    try
    {   // nlohmann throws unless object is an array or null
        nlohmann::json str_nloh_j("nlohmann json string");
        std::cout << "nlohmann::json before: (" << NlohmannTypeName(str_nloh_j.type()) << "): " << str_nloh_j.dump() << std::endl;
        str_nloh_j.push_back(123);  // push_back to a string
        std::cout << "nlohmann::json after: (" << NlohmannTypeName(str_nloh_j.type()) << "): " << str_nloh_j.dump() << std::endl;
    } catch (nlohmann::detail::exception& ex) {
        std::cout << "nlohmann::json exception: " << ex.what() << std::endl;
    }
    
    try
    { // rapid asserts unless object is an array
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
    
    try { // jsonland throws unless object is an array or null
        jsonland::json_node str_land_j("jsonland json string");
        std::cout << "jsonland::json_node before: (" << jsonland::value_type_name(str_land_j.get_value_type()) << "): " << str_land_j.dump() << std::endl;
        str_land_j.push_back(123);
        std::cout << "jsonland::json_node after: (" << jsonland::value_type_name(str_land_j.get_value_type()) << "): " << str_land_j.dump() << std::endl;
    } catch (std::exception& ex) {
        std::cout << "jsonland exception: " << ex.what() << std::endl;
    }
    
    std::cout << "..." << std::endl;
}

static void as_string()
{
    std::cout << "--- " << "!as_string" << std::endl;
    {   // get the value as string for any type : jsonland
        jsonland::json_node s1("I am a string");
        jsonland::json_node n1(1234567);
        jsonland::json_node b1(true);
        jsonland::json_node p1(nullptr);
        
        std::cout << "jsonland: " << jsonland::value_type_name(s1.get_value_type()) << " node: " << s1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(n1.get_value_type()) << " node: " << n1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(b1.get_value_type()) << " node: " << b1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(p1.get_value_type()) << " node: " << p1.dump() << std::endl;
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

static void operator_equal()
{
    std::cout << "--- " << "!operator_equal" << std::endl;
    // what is you use operator= on a non object
    {
        jsonland::json_node s1("I am a string");
        auto s2 = s1["pilpel"].clone();
        jsonland::json_node n1(jsonland::null_t);
        auto n2 = jsonland::json_node(n1["pilpel"]);

        std::cout << "jsonland: " << jsonland::value_type_name(s1.get_value_type()) << " node: " << s1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(s2.get_value_type()) << " node: " << s2.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(n1.get_value_type()) << " node: " << n1.dump() << std::endl;
        std::cout << "jsonland: " << jsonland::value_type_name(n2.get_value_type()) << " node: " << n2.dump() << std::endl;
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

static void preserve_ordering()
{
    std::cout << "--- " << "!preserve_ordering" << std::endl;
    jsonland::json_node J(jsonland::object_t);
    J["1st"] = "a";
    J["2nd"] = "b";
    J["3rd"] = "c";

    for (auto& iter : J)
    {
        std::cout << iter.key() << ": " << iter.dump() << std::endl;
    }
}

static void nlohmann_weird_insertions()
{
    nlohmann::json jsonData;

    jsonData["mixInfoVersion"] = "1.2.3.4";

    jsonData["id"] = 182364;
    jsonData["trackName"] = "Lady Stardust";
    for (int32_t preset = 0; preset < 2; ++preset)
    {
        jsonData["Presets"][preset]["what"] = preset;
        jsonData["Presets"][preset]["PresetName"] = "band";
        jsonData["Presets"][preset]["PresetGuid"] = "was";
        jsonData["Presets"][preset]["isLicensedPresets"] = "all";
        jsonData["Presets"][preset]["PresetLocation"] = "together";
    }
    
    std::cout << jsonData.dump() << std::endl;
}

class b
{
public:
    template <typename TValue>
    b(const TValue i)
    {
        if constexpr (std::floating_point<TValue> || std::is_integral_v<TValue>) {
            m_value.printf(i);
        }
        else if constexpr (std::convertible_to<TValue, std::string_view>) {
            m_value = i;
        }
    }

    b& operator+=(const b& in_b)
    {
        m_sub_values.push_back(in_b);
        return *this;
    }

    b& operator+=(b&& in_b)
    {
        m_sub_values.push_back(std::move(in_b));
        return *this;
    }

    std::vector<b> m_sub_values;
    fstr::fstr31    m_value;
};

std::ostream& operator<<(std::ostream& os, const b& bob)
{
    os << bob.m_value.c_str();
    
    if (!bob.m_sub_values.empty())
    {
        os << ": [";
        os << bob.m_sub_values.front();
        
        for (auto sub_bob = bob.m_sub_values.begin()+1; sub_bob != bob.m_sub_values.end(); ++sub_bob)
        {
            os << ", " << *sub_bob;
        }
        
        os << "]";
    }
    
    return os;
}

static void compare_escaping_behaviour(std::string_view in_path)
{
    std::filesystem::path file_to_read = std::filesystem::path(in_path);
    {
        std::ifstream ifs(file_to_read);
        nlohmann::json jf = nlohmann::json::parse(ifs);
        jf["CP"] = jf["PF"];
        std::filesystem::path out_file = file_to_read.parent_path().append("nlohmann_out.json");
        std::ofstream ofs(out_file);
        ofs << jf;
        ofs << "\n...\n";
        ofs << jf["PF"].dump();
        ofs << "\n";
        ofs << jf["AD"].dump();
        ofs << "\n...\n";
        ofs << jf["PF"].get<std::string_view>();
        ofs << "\n";
        ofs << jf["AD"].get<std::string_view>();
    }

    {
        std::ifstream ifs(file_to_read);
        jsonland::json_doc jf;
        std::string iss((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());
        jf.parse(iss);
        jf["CP"] = jf["PF"].clone();

        std::filesystem::path out_file = file_to_read.parent_path().append("jsonland_out.json");
        std::ofstream ofs(out_file);
        ofs << jf;
        ofs << "\n...\n";
        ofs << jf["PF"].dump();
        ofs << "\n";
        ofs << jf["AD"].dump();
        ofs << "\n...\n";
        ofs << jf["PF"].get_string();
        ofs << "\n";
        ofs << jf["AD"].get_string();
    }
}

int main(int argc, char* argv[])
{
    compare_escaping_behaviour(argv[1]);
}
