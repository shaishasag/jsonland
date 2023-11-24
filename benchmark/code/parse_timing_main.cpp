#include <filesystem>
#include <fstream>
#include <iostream>

#include "jsonland/json_node.h"


jsonland::json_node create_array()
{
    jsonland::json_node a_num_from_num(17);
    jsonland::json_node a_num_from_string("300.123", jsonland::number_t);

    jsonland::json_node jStr1("mamaluka", jsonland::string_t);
    
    // create an object
    jsonland::json_node jObj1(jsonland::object_t);
    jsonland::json_node a_false(false);
    jsonland::json_node a_true(true);
    jsonland::json_node a_string("stringify");
    jObj1["a_num_from_num"] = a_num_from_num;
    jObj1["a_num_from_string"] = a_num_from_string;
    jObj1["a_false"] = a_false;
    jObj1["a_true"] = a_true;
    jObj1["a_string"] = a_string;
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
 

void parse_and_report_file(std::filesystem::path& in_file, bool in_situ=true)
{
    std::cout.imbue(std::locale(std::cout.getloc(), new sep_1000));
    std::cout << "parsing " << in_file << std::endl;

    std::ifstream ifs(in_file);
    std::string contents((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    
    auto before = std::chrono::steady_clock::now();
    jsonland::json_doc jdoc;
    int err = 0;
    std::string parse_type;
    if (in_situ) {
        parse_type = "insitue";
        err = jdoc.parse_insitu(contents);
    }
    else {
        parse_type = "copy";
        err = jdoc.parse(contents);
    }
    auto after = std::chrono::steady_clock::now();
    if (0 == err)
    {
        //j_example_1.dump(std::cout);
        auto duration = std::chrono::duration<double, std::milli>(after - before).count();
        uint64_t byte_per_mili = double(contents.size()) / double(duration);
        std::cout << "    " << "parsing " << "(" << parse_type << ") of " << contents.size() << " bytes " << std::fixed << duration << "ms;";
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

static void report_size_of()
{
    std::cout << "sizeof(string_or_view): " << sizeof(jsonland::string_or_view) << std::endl;
    std::cout << "sizeof(json_node): " << sizeof(jsonland::json_node) << std::endl;
    std::cout << "    sizeof(json_node.m_node_type): " << sizeof(jsonland::json_node::m_node_type) << std::endl;
    std::cout << "    sizeof(json_node.m_value): " << sizeof(jsonland::json_node::m_value) << std::endl;
    
    std::cout << "    sizeof(json_node.m_num): " << sizeof(jsonland::json_node::m_num) << std::endl;
    std::cout << "    sizeof(json_node.m_values): " << sizeof(jsonland::json_node::m_values) << std::endl;
    std::cout << "    sizeof(json_node.m_obj_key_to_index): " << sizeof(jsonland::json_node::m_obj_key_to_index) << std::endl;
    std::cout << "    sizeof(json_node.m_key): " << sizeof(jsonland::json_node::m_key) << std::endl;
}

int parse_and_report_files()
{
    std::filesystem::path file_in_folder = std::filesystem::path(__FILE__);
    //file_in_folder.remove_filename(); std::cout << file_in_folder << std::endl;
    file_in_folder = file_in_folder.parent_path();
    file_in_folder = file_in_folder.parent_path();
    file_in_folder.append("files");
    file_in_folder.append("example_1.json");
    std::vector<std::filesystem::path> path_vec;
    path_vec.push_back(file_in_folder);
    path_vec.push_back(file_in_folder.replace_filename("example_2.json"));
    path_vec.push_back(file_in_folder.replace_filename("example_3.json"));

    for (auto& path : path_vec)
    {
        parse_and_report_file(path, true);
    }
    

    return 0;
}

int main(int argc, const char * argv[])
{
    parse_and_report_files();
    report_size_of();

//    auto jArr = create_array();
//    jArr.dump(std::cout);
//    std::cout << std::endl;
//    jArr[0]["two"].dump(std::cout);
//    std::cout << std::endl;
}
