# provides Summeries for jsonland objects in Xcode debugger - lldb
# to enable this file add a line in ~/.lldbinit:
# command script import /path/to/jsonland/MacOS/xcode_summaries.py
# where /path/to should be replaced with the path to jsonland project

import sys
import lldb
import ctypes

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

def char_pointer_to_string(char_pointer, num_chars): 
    if num_chars == 0:
        return "" 
    error = lldb.SBError()  
    address = char_pointer.GetValueAsUnsigned(error)  
    process = char_pointer.GetProcess()  
  
    if error.Fail() or not process.IsValid():  
        return "FAILED 1"  
  
    data = process.ReadMemory(address, num_chars, error)  
    if error.Fail():  
        return "FAILED 2"  
  
    # Assuming ASCII encoding, change it to the appropriate encoding if needed  
    string = data.decode('ascii')  
    return string
     
def string_or_view_summery(valobj, internal_dict, options):
    string_SBValue = valobj.EvaluateExpression("data()")
    string_size = valobj.EvaluateExpression("size()").GetValueAsUnsigned()
    string = char_pointer_to_string(string_SBValue, string_size)
    string = f"'{string}'"
    return string



def __lldb_init_module(debugger, dict):
    print("loading llvm/Xcode custom debugger summaries for jsonland")
    debugger.HandleCommand(f'type summary add jsonland::json_node  -F "{__name__}.json_node_summery"')
    debugger.HandleCommand(f'type summary add jsonland::string_or_view  -F "{__name__}.string_or_view_summery"')
    debugger.HandleCommand('type category enable jsonland')
    
    
