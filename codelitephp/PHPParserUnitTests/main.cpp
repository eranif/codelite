#include <stdio.h>
#include "php_parser_api.h"
#include <tester.h>
#include "php_storage.h"
#include <globals.h>
#include "php_code_completion.h"
#include "php_workspace.h"

// --------------------------------------------------------
// Helpers
// --------------------------------------------------------
wxString GetFileContent(const wxString &filename) {
    wxString content;
    wxFFile fp(filename, "r");
    if ( fp.IsOpened() ) {
        fp.ReadAll( &content );
    }
    return content;
}

TEST_FUNC(TestGlobalFuncLocals)
{
    PHPParser.parseFile(true, wxT("../Tests/test_global_func_locals.php"), PHPGlobals::Type_Normal);
    CHECK_BOOL_INT(PHPParser.getLocals().size() == 8, PHPParser.getLocals().size());
    CHECK_BOOL(PHPParser.getCurrentScope() == wxT("\\MyNS"));
    CHECK_BOOL(PHPParser.getCurrentFunction() == wxT("foo"));
    CHECK_BOOL(PHPParser.getLocals().at(wxT("$v1")).getName() == wxT("$v1"));
    CHECK_BOOL(PHPParser.getLocals().at(wxT("$v2")).getName() == wxT("$v2"));
    CHECK_BOOL(PHPParser.getLocals().at(wxT("$v3")).getName() == wxT("$v3"));
    CHECK_BOOL(PHPParser.getLocals().at(wxT("$v4")).getName() == wxT("$v4"));
    CHECK_BOOL(PHPParser.getLocals().at(wxT("$v5")).getName() == wxT("$v5"));

    // check that type was copied from the global variable
    CHECK_BOOL(PHPParser.getLocals().at(wxT("$v1")).getType() == wxT("DatabaseManager"));
    CHECK_BOOL(PHPParser.getLocals().at(wxT("$v3")).getType() == wxT("SQLite"));
    //PHPParser.print();
    return true;
}

TEST_FUNC(ExprChain_Simple)
{
    PHPParser.parseFile(true, wxT("../Tests/test_chain.php"), PHPGlobals::Type_Expr);
    CHECK_BOOL_INT( PHPParser.getCurchain().size() == 4, PHPParser.getCurchain().size());
    return true;
}

TEST_FUNC(ExprChain_Self)
{
    PHPParser.parseFile(true, wxT("../Tests/test_chain1.php"), PHPGlobals::Type_Expr);
    CHECK_BOOL_INT( PHPParser.getCurchain().size() == 3, PHPParser.getCurchain().size());
    return true;
}

TEST_FUNC(ExprChain_StaticMethod)
{
    PHPParser.parseFile(true, wxT("../Tests/test_chain_static_method.php"), PHPGlobals::Type_Expr);
    CHECK_BOOL_INT( PHPParser.getCurchain().size() == 3, PHPParser.getCurchain().size());
    return true;
}

TEST_FUNC(ExprChain_StaticVariable)
{
    PHPParser.parseFile(true, wxT("../Tests/test_chain_static_variable.php"), PHPGlobals::Type_Expr);
    CHECK_BOOL_INT( PHPParser.getCurchain().size() == 5, PHPParser.getCurchain().size());
    return true;
}

TEST_FUNC(ExprChain_EndsWithObjOperator)
{
    PHPParser.parseFile(true, wxT("../Tests/test_chain_ends_with_obj_operators.php"), PHPGlobals::Type_Expr);
    //CHECK_BOOL_INT( PHPParser.getCurchain().size() == 5, PHPParser.getCurchain().size());
    PHPEntry &last = PHPParser.getCurchain().back();
    // test to see that the last entry in the chain ends with "->"
    CHECK_BOOL_INT(last.getTerminatingOperator() == T_OBJECT_OPERATOR, last.getTerminatingOperator());
    return true;
}

TEST_FUNC(Test_Init_Class_Member_In_Method)
{
    PHPParser.parseFile(true, wxT("../Tests/test_init_member.php"), PHPGlobals::Type_Normal);
    std::vector<PHPEntry*>& classes = PHPParser.getClasses();
    CHECK_BOOL(classes.at(0)->getName() == wxT("MyClas"));

    classes.at(0)->print();
    CHECK_BOOL(classes.at(0)->findChild(wxT("my_member"), PHP_Kind_Member) != NULL);

    PHPEntry *entry = classes.at(0)->findChild(wxT("my_member"), PHP_Kind_Member);
    CHECK_BOOL(entry->getType() == wxT("SQLite"));
    return true;
}

TEST_FUNC(Test_Parsing_Include_Statement)
{
#ifdef _WIN32
    wxFileName expectedResult, fn;
    expectedResult = wxFileName(wxGetCwd(), wxT("file.php"));
    fn = PHPParser.parseIncludeStatement(wxT("include getcwd() . '/file.php';"),    wxT("../Tests/test_include_statement_parsing.php"));

    CHECK_BOOL(expectedResult.GetFullPath() == fn.GetFullPath());

    fn = PHPParser.parseIncludeStatement(wxT("include MY_BASE_DIR . '/file.php';"),    wxT("../Tests/test_include_statement_parsing.php"));
    CHECK_BOOL(expectedResult.GetFullPath() == fn.GetFullPath());

    // use the directory name of __FILE__ (in our case ../Tests/)
    fn = PHPParser.parseIncludeStatement(wxT("include dirname(__FILE__) . '/file.php';"),    wxT("../Tests/test_include_statement_parsing.php"));
    CHECK_BOOL(fn.GetFullPath() == wxT("..\\Tests\\file.php"));

    // Similar to __DIR__ = dirname(__FILE__)
    fn = PHPParser.parseIncludeStatement(wxT("include __DIR__ . '/file.php';"),    wxT("../Tests/test_include_statement_parsing.php"));
    wxPrintf(wxT("%s\n"), fn.GetFullPath().c_str());
    CHECK_BOOL(fn.GetFullPath() == wxT("..\\Tests\\file.php"));

    fn = PHPParser.parseIncludeStatement(wxT("include __DIR__ . DIRECTORY_SEPARATOR . 'file.php';"),    wxT("../Tests/test_include_statement_parsing.php"));
    wxPrintf(wxT("%s\n"), fn.GetFullPath().c_str());
    CHECK_BOOL(fn.GetFullPath() == wxT("..\\Tests\\file.php"));
#endif
    return true;
}

TEST_FUNC(Test_Global_Function_Argumetns)
{
    PHPParser.parseFile(true, wxT("../Tests/test_global_function_arguments.php"), PHPGlobals::Type_Normal);
    CHECK_BOOL(PHPParser.getCurrentEntry().getArguments().size() == 2);
    return true;
}

TEST_FUNC(Test_Assignment_With_Function)
{
    PHPParser.parseFile(true, wxT("../Tests/test_assignment_with_function.php"), PHPGlobals::Type_Normal);
    PHPGlobals::EntryMap_t locals = PHPParser.getLocals();
    CHECK_BOOL(locals.count("$a") == 1);
    PHPEntry &local = locals.find("$a")->second;
    CHECK_BOOL(local.getType() == "$c->get");
    return true;
}

TEST_FUNC(Test_Assignment_With_Global_Function)
{
    PHPParser.parseFile(true, wxT("../Tests/test_assign_variable_from_global_function.php"), PHPGlobals::Type_Normal);
    PHPGlobals::EntryMap_t locals = PHPParser.getLocals();
    CHECK_BOOL(locals.count("$a") == 1);
    PHPEntry &local = locals.find("$a")->second;
    CHECK_BOOL(local.getType() == "bar");
    return true;
}

TEST_FUNC(Test_Simple_Class_Members_No_Namespace)
{
    PHPEntry::Vector_t matches;
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_simple_class_members_in_global_namespace.php"), "$cls->", matches);
    CHECK_BOOL(matches.size() ==  3);
    return true;
}

TEST_FUNC(Test_Simple_Class_Members_In_Namespace)
{
    PHPEntry::Vector_t matches;
    
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_simple_class_in_namespace.php"), "$clsA->", matches);
    CHECK_BOOL(matches.size() ==  0);
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_simple_class_in_namespace.php"), "$clsB->", matches);
    CHECK_BOOL(matches.size() ==  3);
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_simple_class_in_namespace.php"), "$clsC->", matches);
    CHECK_BOOL(matches.size() ==  3);
    return true;
}

TEST_FUNC(Test_Complete_Namespace_Children)
{
    PHPEntry::Vector_t matches;

    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_complete_namespace_children.php"), "\\NS_Test1\\", matches);
    CHECK_SIZE(matches.size(),1); // single child NS_Sub_1
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_complete_namespace_children.php"), "\\NS_Test1\\NS_Sub_1\\", matches);
    CHECK_SIZE(matches.size(), 1); // single child NS_Sub_2
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_complete_namespace_children.php"), "\\NS_Test1\\NS_Sub_1\\NS_Sub_2\\", matches);
    CHECK_SIZE(matches.size(), 1); // single child ClassInNS
 
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_complete_namespace_children.php"), "\\NS_", matches);
    CHECK_SIZE(matches.size(), 1); // single child ClassInNS
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_complete_namespace_children.php"), "$_ClassInNS->", matches);
    CHECK_SIZE(matches.size(), 2); // 2 matches foo() and bar()
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_complete_namespace_children.php"), "$_ClassInNSFullPath->", matches);
    CHECK_SIZE(matches.size(), 2); // 2 matches foo() and bar()
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_complete_namespace_children.php"), "$dbhPDO_Bad->", matches);
    CHECK_SIZE(matches.size(), 0); // 0 matches, since PDO is not in namespace NS_Test1\NS_Sub_1\NS_Sub_2
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_complete_namespace_children.php"), "$dbhPDO->", matches);
    CHECK_SIZE(matches.size(), 108); // 108 entries for PDO
    return true;
}

TEST_FUNC(Test_Parent_Keyword_Completion)
{
    PHPEntry::Vector_t matches;
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_parent_keyword.php"), "parent::", matches);
    CHECK_SIZE(matches.size(), 2);

    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_parent_keyword_inherits_with_full_path.php"), "parent::", matches);
    CHECK_SIZE(matches.size(), 17);
    return true;
}

TEST_FUNC(Test_Using_Namespace_Keyword)
{
    PHPEntry::Vector_t matches;
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_using_namesapce.php"), "$a->", matches);
    CHECK_SIZE(matches.size(), 3);
    
    matches.clear();
    PHPCodeCompletion::Instance()->TestSuggestMembers(GetFileContent("../Tests/test_use_in_inheritance.php"), "$a->", matches);
    CHECK_SIZE(matches.size(), 3);
    return true;
}

// test that anonymous function arguments do not mix with their parent function (if any)
TEST_FUNC(Test_Anonymous_Function)
{
    PHPParser.parseFile(true, "../Tests/test_anonymous_function.php", PHPGlobals::Type_Normal);
    CHECK_SIZE(PHPParser.getClasses().size(), 1);
    
    PHPEntry *cart = PHPParser.getClasses().at(0);
    CHECK_WXSTRING(cart->getName(), "Cart");
    
    PHPEntry *func = PHPParser.getClasses().at(0)->findChild("getTotal", PHP_Kind_Function);
    CHECK_BOOL(func);
    
    // Check the number of arguments
    CHECK_SIZE(func->getArguments().size(), 1);
    return true;
}

TEST_FUNC(Test_Function_Body_With_Keywords)
{
    PHPParser.parseFile(true, "../Tests/test_function_body_with_keywords.php", PHPGlobals::Type_Normal, true);
    CHECK_SIZE(PHPParser.getClasses().size(), 1);
    
    PHPEntry *cart = PHPParser.getClasses().at(0);
    CHECK_WXSTRING(cart->getName(), "MyClass2124");
    
    PHPEntry *func = cart->findChild("foo", PHP_Kind_Function);
    CHECK_BOOL(func);
    return true;
}

int main(int argc, char **argv)
{
    wxInitialize(argc, argv);
    PHPCodeCompletion::Instance()->SetUnitTestsMode(true);
    PHPStorage::Instance()->SetLimit( 200 );
    
    // In order for code completin to work we must have a workspace opened
    PHPWorkspace::Get()->Open("../Tests/Test.phpwsp");
    //PHPStorage::Instance()->OpenWorksace(wxFileName("../Tests/Test.phptags"), true, true);
    Tester::Instance()->RunTests();
    //PHPParser.parseFile(wxT("../Tests/generate_function.php"), PHPGlobals::Type_Normal);
    //PHPParser.print();
    return 0;
}
