# Jsonland

C++17 JSON Parser/Builder

## Overview
Jsonland is a JSON parser and builder written in C++17. It is designed to be fast, efficient, and easy to use, focusing on minimal memory allocation and straightforward API.

## Getting Started
- Add `jsonland/json_node.cpp` to your project.
- Ensure your compiler's search paths include the `jsonland` folder.
- Use `#include "jsonland/json_node.h"` in your code.

### Prerequisites
- Any C++17 or above compliant compiler.

## Design Goals
- **Fast Parsing**: Minimal allocation and copying during parsing.
- **Clear API**: Intuitive and programmer-friendly API for JSON manipulation.
- **Efficient JSON Creation**: Easy JSON creation in code with minimal allocation and copying.
- **Debugger-Friendly**: Easy inspection of JSON objects in debuggers.
- **Compact Code**: Small code footprint, using simple and clear C++17 code without specialized allocators or macros.
- **Stable Order**: JSON objects maintain the order of insertion during iteration.
- **Just-In-Time Conversions**: Number-to-string and string-to-number conversions are done only when needed.

## Examples
Here are some basic examples of using Jsonland:

### Parsing JSON
```cpp
#include "jsonland/json_node.h"

int main() {
    jsonland::json_doc myJson;
    myJson.parse(R"({"name": "Jsonland", "version": 1.0})");

    std::string name = myJson["name"].get_value();
    double version = myJson["version"].get_value();
}



## Tests

Test were implemented with googletest

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```



## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Shai Shasag**

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the BSD-3-Clause license - see the [LICENSE](LICENSE) file for details

## Acknowledgments

