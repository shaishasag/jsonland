# Jsonland

C++20 JSON Parser/Builder

## Overview
Jsonland is a JSON parser and builder written in C++20. It is designed to be fast, efficient, and easy to use, focusing on minimal memory allocation and straightforward API.

## Getting Started
- Add `jsonland/json_node.cpp` to your project.
- Ensure your compiler's search paths include the `jsonland` folder.
- Use `#include "jsonland/json_node.h"` in your code.

### Prerequisites
- Any C++20 or above compliant compiler.

## Design Goals
- **Fast Parsing**: Minimal allocation and copying during parsing.
- **Clear API**: Intuitive and programmer-friendly API for JSON manipulation.
- **Efficient JSON Creation**: Easy JSON creation in code with minimal allocation and copying.
- **Debugger-Friendly**: Easy inspection of JSON objects in debuggers.
- **Compact Code**: Small code footprint, using simple and clear C++20 code without specialized allocators or macros.
- **Stable Order**: JSON objects maintain the order of insertion during iteration.
- **Just-In-Time Conversions**: Number-to-string and string-to-number conversions are done only when needed.

## Examples
Here are some basic examples of using Jsonland:

### Parsing JSON
```cpp
#include "jsonland/json_node.h"

int main() {
    // parse a json string
    jsonland::json_doc myJson;
    myJson.parse(R"({"name": "Jsonland", "version": 1.0})");

    // get some values
    std::string_view name = myJson["name"].get_string();
    double version = myJson["version"].get_double();
    
    // parsed json can be extended:
    // erase a value
    myJson.erase("version");
    
    // add array and push some values
    auto& sub_array = myJson.append_array("keywords");
    sub_array.push_back("json", "parse", "dump");
    
    // add object and push some values
    auto& sub_object = myJson.append_object("map");
    sub_object["json"] = true;
    sub_object["parse"] = 1.0f;
    sub_object["dump"] = 1234;
    
    // convert myJson to string and output to stdout
    std::cout << myJson.dump() << std::endl;
    // outputs: {"name":"Jsonland","keywords":["json","parse","dump"],"map":{"json":true,"parse":1.0,"dump":1234}}
}
```

### Printing JSON
jsonland::json_node can be printed to a string using the dump() function.
By default dump() outputs the json as compact as possible, without any spaces and new lines.

Printing can be done in "pretty" style where each value is in its own line and nested values are indented by passing the style parameter to dump():
```cpp
dump(jsonland::dump_style::pretty);
```

```cpp
#include "jsonland/json_node.h"

int main() {
    // parse a json string
    jsonland::json_doc myJson;
    myJson.parse(R"({"name": "Jsonland", "version": 1.0})");
    
    std::string tight_dump = myJson.dump();
    // tight_dump == {"name":"Jsonland","version":1.0}
    
    std::string pretty_dump = myJson.dump(jsonland::dump_style::pretty);
    // pretty_dump ==
    // {
    //     "name": "Jsonland",
    //     "version": 1.0
    // }
}
```

### Creating JSON programmatically

```cpp
    // Create an instance of json_node
    jsonland::json_node person;

    // Add string value
    person["name"] = "Jane Doe";

    // Add integer value
    person["age"] = 30;

    // Add array of hobbies to the person object
    jsonland::json_node& hobbies = person.append_array("hobbies");
    hobbies.push_back("Reading");
    hobbies.push_back("Hiking");
    hobbies.push_back("Coding");

    // access some values
    std::string some_values_str = "My name is ";
    some_values_str += person["name"].get_string();
    some_values_str += ", my first hobby is ";
    some_values_str += person["hobbies"][0].get_string();
    std::cout << some_values_str << std::endl;
    // outputs: My name is Jane Doe, my first hobby is Reading

    // Serialize the whole JSON object to string
    std::string json_string = person.dump();

    // Output the JSON string
    std::cout << json_string << std::endl;
    // outputs: {"name":"Jane Doe","age":30,"hobbies":["Reading","Hiking","Coding"]}
```

## Tests

Test were implemented with googletest.


## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

* **Shai Shasag**

## License

This project is licensed under the BSD-3-Clause license - see the [LICENSE](LICENSE) file for details

