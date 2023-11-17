#include <filesystem>
#include <fstream>
#include <iostream>

#include "jsonland/json_node.h"


jsonland::json_node create_array()
{
    // create an object
    jsonland::json_node jObj1(jsonland::object_t);
    jObj1["one"] = 1;
    jObj1["two"] = "two";
 
    // create another object
    jsonland::json_node jObj2(jsonland::object_t);
    jObj2["three"] = 3;
    jObj2["four"] = "quatro";

    // creat an array from the two objects
    jsonland::json_node jArray(jsonland::array_t);
    jArray.push_back(jObj1);
    jArray.push_back(jObj2);
    
    // add somethings to the array
    jArray.push_back("just a string");
    jArray.push_back(123456);
    jArray.push_back(true);
    jArray.push_back(nullptr);

    return jArray;
}

void recursive_dive(const jsonland::json_node& jNode, uint32_t indent=0)
{
    if (jNode.is_array())
    {
        //std::fill_n(std::ostream_iterator<char>(std::cout), 4*indent, ' ');
        std::cout << "array:" << std::endl;
        ++indent;
        for (auto& array_item : jNode)
        {
            std::fill_n(std::ostream_iterator<char>(std::cout), 4*indent, ' ');
            std::cout << "- ";
            recursive_dive(array_item, indent);
            std::cout << "\n";
        }
        --indent;
    }
    else if (jNode.is_object())
    {
        //std::fill_n(std::ostream_iterator<char>(std::cout), 4*indent, ' ');
        std::cout << "object:" << std::endl;
        ++indent;
        for (auto& obj_item : jNode)
        {
            std::fill_n(std::ostream_iterator<char>(std::cout), 4*indent, ' ');
            std::cout << obj_item.key();
            //recursive_dive(jNode[obj_item.key()], indent);
            std::cout << "\n";
        }
        --indent;
    }
    else if (jNode.is_string())
    {
        std::cout << jNode.as_string();
    }
    else if (jNode.is_num())
    {
        std::cout << jNode.as_int<int64_t>();
    }
    else if (jNode.is_bool())
    {
        std::cout << jNode.as_bool();
    }
    else if (jNode.is_null())
    {
        std::cout << "NULL";
    }
}
struct sep_1000 : std::numpunct<char>
{
    char do_thousands_sep()   const { return ','; }  // separate with spaces
    std::string do_grouping() const { return "\3"; } // groups of 1 digit
};
 

void parse_and_report_file(std::filesystem::path& in_file)
{
    std::cout.imbue(std::locale(std::cout.getloc(), new sep_1000));
    std::cout << "parsing " << in_file << std::endl;

    std::ifstream ifs(in_file);
    std::string contents((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    
    auto before = std::chrono::steady_clock::now();
    jsonland::json_doc jdoc;
    int err = jdoc.parse_insitu(contents);
    auto after = std::chrono::steady_clock::now();
    if (0 == err)
    {
        //j_example_1.dump(std::cout);
        auto duration = std::chrono::duration<double, std::milli>(after - before).count();
        uint64_t byte_per_mili = double(contents.size()) / double(duration);
        std::cout << "    " << "parsing of " << contents.size() << " bytes " << std::fixed << duration << "ms;";
        std::cout << " " << byte_per_mili << " bytes per millisecond" << std::endl;
        size_t memory_consumption = jdoc.memory_consumption();
        uint64_t mem_ratio = double(memory_consumption) / double(contents.size());
        std::cout << "    " << "memory consumption " << memory_consumption << " bytes;  " << "1:" << mem_ratio << "\n";
    }
    else
    {
        std::cout << "    " << "parsing error: " << err << std::endl;
    }
}

int main(int argc, const char * argv[])
{
//    auto jArr = create_array();
//    jArr.dump(std::cout);
//    std::cout << std::endl;
//    jArr[0]["two"].dump(std::cout);
//    std::cout << std::endl;

    std::filesystem::path example_folder = std::filesystem::path(__FILE__);
    std::vector<std::filesystem::path> path_vec;
    path_vec.push_back(std::filesystem::path(__FILE__).parent_path().append("example_1.json"));
    path_vec.push_back(std::filesystem::path(__FILE__).parent_path().append("example_2.json"));
    path_vec.push_back(std::filesystem::path(__FILE__).parent_path().append("example_3.json"));

    for (auto& path : path_vec)
    {
        parse_and_report_file(path);
    }
    
    std::cout << std::endl;
    std::cout << "sizeof(json_node): " << sizeof(jsonland::json_node) << std::endl;
    std::cout << "    sizeof(json_node.m_node_type): " << sizeof(jsonland::json_node::m_node_type) << std::endl;
    std::cout << "    sizeof(json_node.m_str_v): " << sizeof(jsonland::json_node::m_value) << std::endl;

    std::cout << "    sizeof(json_node.m_num): " << sizeof(jsonland::json_node::m_num) << std::endl;
    std::cout << "    sizeof(json_node.m_values): " << sizeof(jsonland::json_node::m_values) << std::endl;
    std::cout << "    sizeof(json_node.m_obj_key_to_index): " << sizeof(jsonland::json_node::m_obj_key_to_index) << std::endl;
    std::cout << "    sizeof(json_node.m_key): " << sizeof(jsonland::json_node::m_key) << std::endl;

    return 0;
}
