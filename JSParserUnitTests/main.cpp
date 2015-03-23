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

int main(int argc, char** argv)
{
    wxInitialize(argc, argv);
    {
        Tester::Instance()->RunTests();
    }
    wxUninitialize();
    return 0;
}
