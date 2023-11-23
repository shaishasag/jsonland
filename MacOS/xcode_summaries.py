# provides Summeries for jsonland objects in Xcode debugger - lldb
# to enable this file add a line in ~/.lldbinit
# command script import /path/to/jsonland/MacOS/xcode_summaries.py
# where /path/to should be replaced with the path to jsonland project

import sys
import lldb


def json_node_summery(valobj, internal_dict, options):

    m_value_type = valobj.GetChildMemberWithName('m_value') \
                            .GetChildMemberWithName('m_value') \
                            .GetChildMemberWithName('__data')
    m_value_type.SetFormat(lldb.eFormatCString)
    m_value_value = m_value_type.GetValue()
    if m_value_value:  # the value comes with double quotes
        m_value_value = m_value_value.strip('"')
    else:
        m_value_value = ""


    m_node_type_value = valobj.GetChildMemberWithName('m_node_type').GetValueAsSigned(-1)
    if 0 == m_node_type_value:
        return "uninitialized_t"
    elif 1 == m_node_type_value:
        return "null_t"
    elif 2 == m_node_type_value:  # bool_t
        return f"bool_t: {m_value_value}"
    elif 4 == m_node_type_value:  # number maybe represented as string or in member m_num
        if not m_value_value:
            m_num_type_value = valobj.GetChildMemberWithName('m_num').GetValue()
            return f"number_t: {m_num_type_value}"
        else:
            return f"number_t: {m_value_value}"
    elif 8 == m_node_type_value:
        return f"string_t: '{m_value_value}'"
    elif 16 == m_node_type_value:
        num_values = valobj.EvaluateExpression("m_values.size()").GetValue()
        return f"array_t: size={num_values}"
    elif 32 == m_node_type_value:
        num_values = valobj.EvaluateExpression("m_values.size()").GetValue()
        return f"object_t size={num_values}"
    else:
        return f"unknown type{m_node_type_value}"

def __lldb_init_module(debugger, dict):
    print("loading llvm/Xcode custom debugger summaries for jsonland")
    #debugger.HandleCommand('type summary add jsonland::json_node --summary-string "${var.m_value.m_value}" -w jsonland')
    debugger.HandleCommand(f'type summary add jsonland::json_node  -F "{__name__}.json_node_summery"')
    debugger.HandleCommand('type summary add jsonland::string_or_view --summary-string "${var.m_value}" -w jsonland')
    debugger.HandleCommand('type category enable jsonland')
    
    
