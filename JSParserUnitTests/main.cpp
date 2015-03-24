#include <stdio.h>
#include "Tester.h"
#include "JSLookUpTable.h"
#include "JSSourceFile.h"
#include <wx/init.h>
#include <wx/module.h>
#include <wx/filename.h>
#include "JSFunction.h"

// Parse class with 2 properties defined using
// this.<name> = ...
TEST_FUNC(test_parse_simple_class)
{
    JSLookUpTable::Ptr_t lookup(new JSLookUpTable());
    JSSourceFile source(lookup, wxFileName("../TestFiles/simple_class.js"));
    source.Parse();
    
    JSObject::Ptr_t Foo = lookup->FindObject("Foo");
    CHECK_BOOL(Foo);
    CHECK_SIZE(Foo->GetProperties().size(), 2);
    Foo->Print(0);
    return true;
}

// Parse class with 3 properties defined outside of the class
TEST_FUNC(class_with_static_method)
{
    JSLookUpTable::Ptr_t lookup(new JSLookUpTable());
    JSSourceFile source(lookup, wxFileName("../TestFiles/class_with_static_method.js"));
    source.Parse();
    
    JSObject::Ptr_t Foo = lookup->FindObject("Foo");
    CHECK_BOOL(Foo);
    CHECK_SIZE(Foo->GetProperties().size(), 3);
    return true;
}

// Parse global function and check that it exists + check its signature
TEST_FUNC(parse_global_function)
{
    JSLookUpTable::Ptr_t lookup(new JSLookUpTable());
    JSSourceFile source(lookup, wxFileName("../TestFiles/parse_global_function.js"));
    source.Parse();
    
    JSObject::Ptr_t Foo = lookup->FindObject("Foo");
    CHECK_BOOL(Foo);
    CHECK_SIZE(Foo->GetProperties().size(), 0);
    CHECK_SIZE(Foo->As<JSFunction>()->GetVariables().size(), 2);
    return true;
}

// Parse global function and check that it exists + check its signature
TEST_FUNC(parse_local_variables_in_functions)
{
    JSLookUpTable::Ptr_t lookup(new JSLookUpTable());
    JSSourceFile source(lookup, wxFileName("../TestFiles/parse_local_variables_in_functions.js"));
    source.Parse();
    
    // Check that both classes exist
    JSObject::Ptr_t Array = lookup->FindObject("Array");
    CHECK_BOOL(Array);
    
    JSObject::Ptr_t Foo = lookup->FindObject("Foo");
    CHECK_BOOL(Foo);
    
    // Check that Foo has 3 local variables
    CHECK_SIZE(Foo->As<JSFunction>()->GetVariables().size(), 3);
    
    // Check the variables names
    const JSObject::Map_t& vars = Foo->As<JSFunction>()->GetVariables();
    CHECK_SIZE(vars.count("a"), 1);
    CHECK_SIZE(vars.count("b"), 1);
    CHECK_SIZE(vars.count("c"), 1);
    
    // Check the variables types
    CHECK_STRING(vars.find("a")->second->GetType(), "Array");
    CHECK_STRING(vars.find("b")->second->GetType(), "Foo");
    CHECK_STRING(vars.find("c")->second->GetType(), "");
    
    // Check that the variables are not visible in the global scope
    JSObject::Map_t variables = lookup->GetVisibleVariables();
    CHECK_SIZE(variables.size(), 5);
    
    return true;
}

// Parse local variables declare inside a callback function
TEST_FUNC(parse_locals_in_callback_function)
{
    JSLookUpTable::Ptr_t lookup(new JSLookUpTable());
    JSSourceFile source(lookup, wxFileName("../TestFiles/parse_locals_in_callback_function.js"));
    source.Parse();
    
    JSObject::Map_t vars = lookup->GetVisibleVariables();
    CHECK_SIZE(vars.size(), 3);
    
    CHECK_SIZE(vars.count("event"), 1);
    CHECK_SIZE(vars.count("local_var"), 1);
    CHECK_SIZE(vars.count("element"), 1);
    return true;
}

// Test parsing function and attaching the return value from the doc comment
TEST_FUNC(parse_function_doc_return_value)
{
    JSLookUpTable::Ptr_t lookup(new JSLookUpTable());
    JSSourceFile source(lookup, wxFileName("../TestFiles/parse_function_doc_return_value.js"));
    source.Parse();
        
    
    JSObject::Ptr_t Foo = lookup->FindObject("Foo");
    const JSObject::Map_t& variables = Foo->As<JSFunction>()->GetVariables();
    CHECK_BOOL(Foo);
    CHECK_SIZE(Foo->GetProperties().size(), 2);
    CHECK_SIZE(variables.size(), 2);
    CHECK_STRING(Foo->GetType(), "Array");
    CHECK_SIZE(variables.count("name"), 1);
    CHECK_SIZE(variables.count("arrOfNames"), 1);
    
    // Check the function arguments
    CHECK_STRING(variables.find("arrOfNames")->second->GetType(), "Array");
    CHECK_STRING(variables.find("name")->second->GetType(), "string");
    return true;
}


int main(int argc, char** argv)
{
    wxInitialize(argc, argv);
    {
        Tester::Instance()->RunTests();
    }
    wxUninitialize();
    return 0;
}
