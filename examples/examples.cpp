
#include "json_node.h"

int main()
{
    // Create an instance of json_node
    jsonland::json_node person;

    // Add string value
    person["name"] = "Jane Doe";

    // Add integer value
    person["age"] = 30;

    // Create an array of hobbies
    jsonland::json_node hobbies;
    hobbies.push_back("Reading");
    hobbies.push_back("Hiking");
    hobbies.push_back("Coding");
    
    // Add array to the person object
    person["hobbies"] = hobbies;

    // access some values
    std::string some_values_str = "My name is ";
    some_values_str += person["name"].get_string();
    some_values_str += ", my first hobby is ";
    some_values_str += person["hobbies"][0].get_string();
    std::cout << some_values_str << std::endl;

    // Serialize the whole JSON object to string
    std::string json_string = person.dump();

    // Output the JSON string
    std::cout << json_string << std::endl;

    return 0;
}
