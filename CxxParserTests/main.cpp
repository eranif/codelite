#include "CxxTokenizer.h"
#include "CxxVariableScanner.h"
#include "ctags_manager.h"
#include "fileutils.h"
#include "tester.h"
#include <iostream>
#include <stdio.h>
#include <wx/init.h>
#include <wx/log.h>

TEST_FUNC(test_cxx_normalize_signature)
{
    wxString buffer = "const std::map<int, int>& m = std::map<int, int>(), int number = -1, const "
                      "std::vector<std::pair<int, int> >& v";
    wxString res =
        TagsManagerST::Get()->NormalizeFunctionSig(buffer, Normalize_Func_Name | Normalize_Func_Default_value);
    return true;
}

TEST_FUNC(test_cxx_local_variables)
{
    wxString buffer = "struct wxString *pstr; const std::string& name;"
                      "std::vector<int> numbers = {1,2,3};"
                      "std::vector<int> numbers2 {1,2,3};"
                      "std::vector<std::pair<int, int>>::iterator myIter;";
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
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
    wxString buffer = "void MainFrame::OnFoo() {}";
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("MainFrame") == 0);
    return true;
}

TEST_FUNC(test_array_variables)
{
    wxString buffer = "wxString arr[10];";
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("arr") == 1);
    return true;
}

TEST_FUNC(test_cxx_c11_template)
{
    wxString buffer = "std::vector<std::pair<int, int>> v;";
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("v") == 1);
    return true;
}

TEST_FUNC(test_cxx_multiple_variables)
{
    wxString buffer = "wxBitmap bmp(1,1);\n"
                      "std::vector<int> foo, bar, baz;";

    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("foo") == 1);
    CHECK_BOOL(vars.count("bar") == 1);
    CHECK_BOOL(vars.count("baz") == 1);
    return true;
}

TEST_FUNC(test_cxx_decltype_template_variable)
{
    wxString buffer =
        "auto comp = [&](std::shared_ptr<int> a, std::shared_ptr<int> b) { return (*a) < (*b);}\n"
        "std::priority_queue<std::shared_ptr<int>, std::vector<std::shared_ptr<int>>,  decltype(comp)> queue(comp);";

    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("queue") == 1);
    CxxVariable::Ptr_t var = vars["queue"];
    //    std::cout << "Type:" << var->GetTypeAsCxxString() << std::endl;
    return true;
}

TEST_FUNC(test_cxx_locals_in_for_loop)
{
    wxString buffer = "for(int i=0; i<100; ++i) {\n"
                      "   wxString sql;";

    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("sql") == 1);
    return true;
}

TEST_FUNC(test_cxx_lambda_args)
{
    wxString buffer = "std::for_each(a.begin(), b.end(), [&](const wxString& lambdaArg){\n";

    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("lambdaArg") == 1);
    CHECK_WXSTRING(vars["lambdaArg"]->GetName(), "lambdaArg");
    return true;
}

TEST_FUNC(test_cxx_lambda_locals)
{
    wxString buffer = "std::for_each(a.begin(), b.end(), [&](const wxString& lambdaArg){\n"
                      "wxString myStr;";

    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("myStr") == 1);
    CHECK_WXSTRING(vars["myStr"]->GetName(), "myStr");
    return true;
}

TEST_FUNC(test_optimize_scope)
{
    wxString buffer = "#define IS_DETACHED(name) (detachedPanes.Index(name) != wxNOT_FOUND) ? true : false\n"
                      "void MyClass::FooBar(){";

    CxxTokenizer tokenizer;
    wxString visibleScope = tokenizer.GetVisibleScope(buffer);
    visibleScope.Trim().Trim(false);
    CHECK_WXSTRING(visibleScope, "void MyClass :: FooBar(){");
    return true;
}

TEST_FUNC(test_locals_inside_for_inside_lambda)
{
    wxString buffer = "std :: for_each(a.begin(), b.end(), [&]( wxAuiToolBar * tb) {"
                      "    for(size_t i = 0; i < tb->GetToolCount(); ++i) {"
                      "        wxAuiToolBarItem* tbItem = nullptr;"
                      "        tbItem->";
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("tbItem") == 1);
    return true;
}

TEST_FUNC(test_locals_inside_while)
{
    wxString buffer = "std::string mystr;"
                      " while(";
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("mystr") == 1);
    return true;
}

TEST_FUNC(test_angel_script_locals)
{
    wxString buffer = "MyType@ var;";
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("var") == 1);
    return true;
}

TEST_FUNC(test_ranged_forloop)
{
    wxString buffer = "for(const wxString& str : myArr) {";
    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_BOOL(vars.count("str") == 1);
    return true;
}

TEST_FUNC(test_local_in_std_for_each)
{
    wxString buffer = "std::for_each(a.begin(), a.end(), [&](const std::string& str) {"
                      "        const wxString& confname = conf->GetName();"
                      "        for(const wxString& filename : excludeFiles) {"
                      "            clProjectFile::Ptr_t file = proj->GetFile(filename);";

    CxxVariableScanner scanner(buffer, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_SIZE(vars.size(), 4);
    CHECK_BOOL(vars.count("file") == 1);
    CHECK_BOOL(vars.count("confname") == 1);
    return true;
}

int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    wxLogNull NOLOG;
    Tester::Instance()->RunTests();
    //    fgetc(stdin);
    return 0;
}
