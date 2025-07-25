#ifndef __2TypesTestsCommon_h__
#define __2TypesTestsCommon_h__

#include "jsonland/json_node.h"
#include "jsonland/JsOn.h"

template <typename T>
class JsonTypedTest : public ::testing::Test {
protected:
    using JsonType = T;
};

using json_node_andJsOn = ::testing::Types<jsonland::JsOn, jsonland::json_node>;
using json_doc_andJsOn = ::testing::Types<jsonland::JsOn, jsonland::json_doc, jsonland::JsOn>;
using just_json_doc = ::testing::Types<jsonland::json_doc>;
using just_json_node = ::testing::Types<jsonland::json_node>;
using just_JsOn = ::testing::Types<jsonland::JsOn>;

#endif // __2TypesTestsCommon_h__
