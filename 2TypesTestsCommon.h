#ifndef __2TypesTestsCommon_h__
#define __2TypesTestsCommon_h__

#include "jsonland/json_node.h"
#include "jsonland/JsOn.h"

template <typename T>
class JsonTypedTest : public ::testing::Test {
protected:
    using JsonType = T;
};

using JsonImplementations = ::testing::Types<jsonland::json_node, jsonland::JsOn>;


#endif // __2TypesTestsCommon_h__
