# Jsonland

C++17 json parser/builder

## Getting Started

- Add the file jsonland/json_node.cpp to your project.
- Make sure the compiler search paths include the jsonland folder.
- ```#include "jsonland/json_node.h"```

### Prerequisites

Any C++17 or above compiler.


## Design goals
- parsing should be fast with minimal allocation and copying.
- API to access parsed json should be clear and easy to work with.
- Creating json in code should be easy with minimal allocation and copying.
- View of json objects in debugger should be easily to understood.
- Small code footprint
- Special efficiency for parsing small (up to 1k) jsons.
- Simple and clear C++17 code, no fancy stuff like specialized allocators or macros.
- Stable order: iteration on a json object should return element in the same order they were inserted.
- Convert numbers to string and string to numbers only when needed (JIT).

##Examples



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

