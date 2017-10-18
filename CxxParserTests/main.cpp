#include <stdio.h>
#include "tester.h"
#include "fileutils.h"
#include <wx/init.h>
#include <iostream>
#include "ctags_manager.h"
#include "CxxVariableScanner.h"

static wxString InputFile(const wxString& name)
{
    wxFileName fn("../Tests", name);
    wxString content;
    FileUtils::ReadFileContent(fn, content);
    return content;
}

TEST_FUNC(test_cxx_normalize_signature)
{
    wxString buffer = InputFile("test_cxx_normalize_signature.h");
    wxString res =
        TagsManagerST::Get()->NormalizeFunctionSig(buffer, Normalize_Func_Name | Normalize_Func_Default_value);
    return true;
}

TEST_FUNC(test_cxx_local_variables)
{
    wxString buffer = InputFile("test_cxx_local_variables.h");
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t());
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("pstr") == 1);
    CHECK_BOOL(vars.count("name") == 1);
    CHECK_BOOL(vars.count("numbers") == 1);
    CHECK_BOOL(vars.count("numbers2") == 1);
    CHECK_BOOL(vars.count("myIter") == 1);
    return true;
}

TEST_FUNC(test_cxx_class_method_impl)
{
    wxString buffer = InputFile("test_cxx_class_method_impl.h");
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t());
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("MainFrame") == 0);
    return true;
}

TEST_FUNC(test_cxx_c11_template)
{
    wxString buffer = InputFile("test_cxx_c11_template.h");
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t());
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("v") == 1);
    return true;
}


TEST_FUNC(test_cxx_multiple_variables)
{
    wxString buffer = 
        "wxBitmap bmp(1,1);\n"
        "std::vector<int> foo, bar, baz;";
        
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t());
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("foo") == 1);
    CHECK_BOOL(vars.count("bar") == 1);
    CHECK_BOOL(vars.count("baz") == 1);
    return true;
}

int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    Tester::Instance()->RunTests();
    return 0;
}
