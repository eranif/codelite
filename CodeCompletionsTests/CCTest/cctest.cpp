
#define USE_CLI_EVENTS 1

#include "tester.h"
#include <algorithm>
#include <cpptoken.h>
#include <cppwordscanner.h>
#include <memory>
#include <parse_thread.h>
#include <refactorengine.h>
#include <stringsearcher.h>
#include <vector>
#include <wx/event.h>
#include <wx/ffile.h>
#include <wx/init.h>   //wxInitializer
#include <wx/string.h> //wxString
#include <wx/tokenzr.h>

// CodeLite includes
#include <CxxVariableScanner.h>
#include <ctags_manager.h>
#include <wx/crt.h>

#ifdef __WXGTK__
#   define WX_STRING_MEMBERS_COUNT 446
#   define CLASS_WITH_MEMBERS_COUNT 31
#else
#   define WX_STRING_MEMBERS_COUNT 453
#   define CLASS_WITH_MEMBERS_COUNT 31
#endif

wxString LoadFile(const wxString& filename)
{
    wxFFile f(filename.c_str(), wxT("r"));
    if(f.IsOpened()) {
        wxString content;
        f.ReadAll(&content);
        return content;
    }
    return wxEmptyString;
}

class EventHandler : public wxEvtHandler
{
public:
    void OnParsingDone(wxCommandEvent& e);
    void OnParsingDoneProg(wxCommandEvent& e);
    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(EventHandler, wxEvtHandler)
EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_PROGRESS, EventHandler::OnParsingDoneProg)
EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_PROGRESS, EventHandler::OnParsingDone)
END_EVENT_TABLE()

void EventHandler::OnParsingDone(wxCommandEvent& e) { wxPrintf(wxT("Parsing completed\n")); }

void EventHandler::OnParsingDoneProg(wxCommandEvent& e) { wxPrintf(wxT("[%%%d] completed\n"), e.GetInt()); }

/////////////////////////////////////////////////////////////////////////////
// Code Completion test cases
/////////////////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
#define CXX_TEST_DIR "../tests/"
#endif

static wxString GetTestsDir()
{
    wxString dir;
    dir << CXX_TEST_DIR;
    return dir;
}

TEST_FUNC(testMacros)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "simple_tests.h"), 1,
                                                 wxT("wxTheClipboard->"), LoadFile(GetTestsDir() + "simple_tests.h"),
                                                 tags);
#ifdef __WXMSW__
    CHECK_SIZE(tags.size(), 65);
#else
    CHECK_SIZE(tags.size(), 55);
#endif

    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "simple_tests.h"), 1, wxT("GET_APP->"),
                                                 LoadFile(GetTestsDir() + "simple_tests.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "simple_tests.h"), 1, wxT("EG(name)."),
                                                 LoadFile(GetTestsDir() + "simple_tests.h"), tags);
#ifdef __WXMSW__
    CHECK_SIZE(tags.size(), 195);
#else
    CHECK_SIZE(tags.size(), 155);
#endif
    return true;
}

TEST_FUNC(testUsingNamespace)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "using_namespace.h"), 3, wxT("v.at(0)."),
                                                 LoadFile(GetTestsDir() + "using_namespace.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testTypedefs)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "typedef_tests.h"), 0,
                                                 wxT("myStack.at(0)."), LoadFile(GetTestsDir() + "typedef_tests.h"),
                                                 tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testStlMapLeditor)
{
    // test map template with basic types
    // std::map<wxString, int> mm;
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "stl_map_static.h"), 2,
                                                 wxT("ms_bookmarkShapes.find(0)->second."),
                                                 LoadFile(GetTestsDir() + "stl_map_static.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);

    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "stl_map_static.h"), 2,
                                                 wxT("ms_bookmarkShapes.find(wxT(\"value\"))->second."),
                                                 LoadFile(GetTestsDir() + "stl_map_static.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);

    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "stl_map_static.h"), 2,
                                                 wxT("ms_bookmarkShapes.find(wxT(\"value\"))->first."),
                                                 LoadFile(GetTestsDir() + "stl_map_static.h"), tags);
    CHECK_SIZE(tags.size(), 0);

    return true;
}

TEST_FUNC(testMapWithBasicTypes)
{
    // test map template with basic types
    // std::map<wxString, int> mm;
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "simple_tests.h"), 2,
                                                 wxT("mm.find()->first."), LoadFile(GetTestsDir() + "simple_tests.h"),
                                                 tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);

    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "simple_tests.h"), 2,
                                                 wxT("mm.find()->second."), LoadFile(GetTestsDir() + "simple_tests.h"),
                                                 tags);
    CHECK_SIZE(tags.size(), 0);
    return true;
}

TEST_FUNC(testTtp)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "smart_ptr_of_template.h"), 3,
                                                 wxT("ttp->GetRoot()->GetData()."),
                                                 LoadFile(GetTestsDir() + "smart_ptr_of_template.h"), tags);
    CHECK_SIZE(tags.size(), 88);

    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "smart_ptr_of_template.h"), 3,
                                                 wxT("ttp->GetRoot()->GetKey()."),
                                                 LoadFile(GetTestsDir() + "smart_ptr_of_template.h"), tags);
    CHECK_SIZE(tags.size(), WX_STRING_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testTempalteInheritance)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "simple_tests.h"), 2,
                                                 wxT("ContextManager::Get()->"),
                                                 LoadFile(GetTestsDir() + "simple_tests.h"), tags);
    CHECK_SIZE(tags.size(), 7);
    return true;
}

TEST_FUNC(testThis)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "this_ptr.h"), 4, wxT("this->"),
                                                 LoadFile(GetTestsDir() + "this_ptr.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testStdVectorOfTagEntryPtr)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "std_vec_tag_entry_ptr.h"), 3,
                                                 wxT("tags.at(0)->"),
                                                 LoadFile(GetTestsDir() + "std_vec_tag_entry_ptr.h"), tags);
    CHECK_SIZE(tags.size(), 88);

    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "std_vec_tag_entry_ptr.h"), 3,
                                                 wxT("tags.at(0)."),
                                                 LoadFile(GetTestsDir() + "std_vec_tag_entry_ptr.h"), tags);
    CHECK_SIZE(tags.size(), 10);
    return true;
}

TEST_FUNC(testIterators)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "iterators.h"), 3, wxT("mapIter->first."),
                                                 LoadFile(GetTestsDir() + "iterators.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);

    tags.clear();
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "iterators.h"), 3, wxT("vecIter->"),
                                                 LoadFile(GetTestsDir() + "iterators.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testTemplateTypedef)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_template_typedef.h"), 11,
                                                 wxT("Foo_t::Get()->"),
                                                 LoadFile(GetTestsDir() + "test_template_typedef.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testNamespace)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_namespace.h"), 1, wxT("flex::"),
                                                 LoadFile(GetTestsDir() + "test_namespace.h"), tags);
    CHECK_SIZE(tags.size(), 35);
    return true;
}

TEST_FUNC(testLocalArgument)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_local_var.h"), 2, wxT("path."),
                                                 LoadFile(GetTestsDir() + "test_local_var.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testBoostForeach)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "boost_foreach.h"), 3, wxT("str."),
                                                 LoadFile(GetTestsDir() + "boost_foreach.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testStdAutoPtr)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_auto_ptr.h"), 4, wxT("p->"),
                                                 LoadFile(GetTestsDir() + "test_auto_ptr.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testVectorOfStdString_OperatorMeruba)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_vector_std_string.h"), 6,
                                                 wxT("v[0]->"), LoadFile(GetTestsDir() + "test_vector_std_string.h"),
                                                 tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testWxOrderedMap)
{
    TagEntryPtrVector_t tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_wx_ordered_map.h"), 2,
                                                 wxT("om.begin()->first"),
                                                 LoadFile(GetTestsDir() + "test_wx_ordered_map.h"), tags);
    CHECK_SIZE(tags.size(), WX_STRING_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testWxArrayString_OperatorMeruba)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_wxarraystr.h"), 7, wxT("myArr[]."),
                                                 LoadFile(GetTestsDir() + "test_wxarraystr.h"), tags);
    CHECK_SIZE(tags.size(), WX_STRING_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testStdSharedPtr)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_shared_ptr.h"), 10, wxT("p->"),
                                                 LoadFile(GetTestsDir() + "test_shared_ptr.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testVectorOfStdString)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_vector_std_string.h"), 6,
                                                 wxT("v.at(0)->"), LoadFile(GetTestsDir() + "test_vector_std_string.h"),
                                                 tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testGotoDeclInsideNamespace)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->FindImplDecl(wxFileName(GetTestsDir() + "test_goto_decl_inside_ns.h"), 11, wxT("Tada"),
                                       wxT("Tada"), LoadFile(GetTestsDir() + "test_goto_decl_inside_ns.h"), tags);

    CHECK_SIZE(tags.size(), 1);
    CHECK_STRING(tags.at(0)->GetName().mb_str(wxConvUTF8).data(), "Tada");
    return true;
}
#if 0
TEST_FUNC(testGotoDeclOfFuncArgUsingTheMethodScope)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->FindImplDecl(wxFileName(GetTestsDir() + "test_goto_decl_of_func_arg.h"), 13, wxT("Tada"),
                                       wxT("Tada"), LoadFile(GetTestsDir() + "test_goto_decl_of_func_arg.h"), tags);

    CHECK_SIZE(tags.size(), 1);
    CHECK_STRING(tags.at(0)->GetName().mb_str(wxConvUTF8).data(), "Tada");
    return true;
}
#endif

TEST_FUNC(testScopeResolving1)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_scope_resolving_1.h"), 4, wxT("cls."),
                                                 LoadFile(GetTestsDir() + "test_scope_resolving_1.h"), tags);
    CHECK_SIZE(tags.size(), 1);
    return true;
}

TEST_FUNC(testTypedefIteratorInsideClass)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(
        wxFileName(GetTestsDir() + "test_typedef_iterator_inside_class.h"), 2, wxT("__iter->second->"),
        LoadFile(GetTestsDir() + "test_typedef_iterator_inside_class.h"), tags);
    CHECK_SIZE(tags.size(), 31);
    return true;
}

TEST_FUNC(testStrcutDelcratorInFuncArgument)
{
    std::vector<TagEntryPtr> tags;
    wxFileName fn(GetTestsDir() + "/../../SampleWorkspace/header.h");
    fn.Normalize();
    TagsManagerST::Get()->AutoCompleteCandidates(fn, 77, wxT("pString->"), LoadFile(fn.GetFullPath()), tags);
    CHECK_SIZE(tags.size(), WX_STRING_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testAutoSimple)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_auto_simple.h"), 2,
                                                 wxT("autoPtr->second->"),
                                                 LoadFile(GetTestsDir() + "test_auto_simple.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testBoostSharedPtr)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "boost_shared_ptr.h"), 2, wxT("s->"),
                                                 LoadFile(GetTestsDir() + "boost_shared_ptr.h"), tags);
    CHECK_SIZE(tags.size(), CLASS_WITH_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testVariablesParserSimple)
{
    wxString content = LoadFile(GetTestsDir() + "test_parse_variables_simple.h");
    CxxVariableScanner scanner(content, eCxxStandard::kCxx11, wxStringTable_t(), false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_SIZE(vars.size(), 1); // a,b,c,d,e,f,g,str,stringMap,nNumber
    CHECK_CONDITION(vars.count("delims"), "variable 'delims' was not found");
    return true;
}

TEST_FUNC(testVariablesParserCxx11)
{
    wxString content = LoadFile(GetTestsDir() + "test_parse_variables_cxx11.h");
    wxString funcSignature = "(int32_t const arg1, int32_t const arg2)";

    CxxVariable::Map_t vars;
    {
        CxxVariableScanner scanner(content, eCxxStandard::kCxx11, wxStringTable_t(), false);
        vars = scanner.GetVariablesMap();
    }

    {
        CxxVariableScanner scanner(funcSignature, eCxxStandard::kCxx11, wxStringTable_t(), true);
        CxxVariable::Map_t M = scanner.GetVariablesMap();
        vars.insert(M.begin(), M.end());
    }

    CHECK_SIZE(vars.size(), 4);
    CHECK_CONDITION(vars.count("var1"), "variable 'var1' was not found");
    CHECK_CONDITION(vars.count("var2"), "variable 'var2' was not found");
    CHECK_CONDITION(vars.count("arg1"), "variable 'arg1' was not found");
    CHECK_CONDITION(vars.count("arg2"), "variable 'arg2' was not found");
    return true;
}

TEST_FUNC(testVariablesParserClassMembers)
{
    wxString content = LoadFile(GetTestsDir() + "test_parse_variables_class_members.h");

    // Ignore table: WXDLLEXPORT => ""
    wxStringTable_t t{ { "WXDLLEXPORT", "" } };
    CxxVariableScanner scanner(content, eCxxStandard::kCxx11, t, false);
    CxxVariable::Map_t vars = scanner.GetVariablesMap();
    CHECK_SIZE(vars.size(), 2);
    return true;
}

TEST_FUNC(test_cxx_nested_template)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_cxx_nested_template.h"), 2, "myVec.",
                                                 LoadFile(GetTestsDir() + "test_cxx_nested_template.h"), tags);
    std::vector<TagEntryPtr>::iterator iter =
        std::find_if(tags.begin(), tags.end(), [&](TagEntryPtr tag) { return tag->GetName() == "front"; });
    CHECK_CONDITION((iter != tags.end()), "Could not find front()");
    return true;
}

TEST_FUNC(testLambdaArgs)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_lambda_args.h"), 3, wxT("str."),
                                                 LoadFile(GetTestsDir() + "test_lambda_args.h"), tags);
    CHECK_SIZE(tags.size(), WX_STRING_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testDeclTypeInTemplate)
{
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(GetTestsDir() + "test_decl_type.h"), 2, wxT("q."),
                                                 LoadFile(GetTestsDir() + "test_decl_type.h"), tags);
#ifdef __WXMSW__
    CHECK_SIZE(tags.size(), 20);
#else
    CHECK_SIZE(tags.size(), 14);
#endif
    return true;
}

TEST_FUNC(testWxArrayStringWitArrayInt)
{
    wxString buffer = "wxArrayInt choices;\n"
                      "wxArrayString lexers;\n"
                      "for(size_t i = 0; i < choices.GetCount(); ++i) {\n"
                      "    lexers.Item(choices.Item(i)).";
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName("dummy.h"), 4, "lexers.Item(choices.Item(i)).", buffer,
                                                 tags);
    CHECK_SIZE(tags.size(), WX_STRING_MEMBERS_COUNT);
    return true;
}

TEST_FUNC(testUnorderedMapIterator)
{
    wxString buffer = "std::unordered_map<int, wxString> M;\n"
                      "M[0].";
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->AutoCompleteCandidates(wxFileName("dummy.h"), 4, "M[0].", buffer, tags);
    CHECK_SIZE(tags.size(), WX_STRING_MEMBERS_COUNT);
    return true;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void testCC()
{
    // Load the tags database that is used during the test.
    std::cout << "Tests Dir is set to: " << GetTestsDir() << std::endl;

    wxFileName fn(GetTestsDir() + "../../SampleWorkspace/.codelite/SampleWorkspace.tags");
    TagsManagerST::Get()->OpenDatabase(fn);

    TagsOptionsData tod;
    TagsManagerST::Get()->SetCtagsOptions(tod);
    TagsManagerST::Get()->GetDatabase()->SetSingleSearchLimit(
        5000); // For the testing to pass we need to set a huge number of matches
    // Execute the tests
    Tester::Instance()->RunTests();

    Tester::Release();
    TagsManagerST::Free();
    LanguageST::Free();
}
#if 0
void InitialiseTags()
{
    // load the workspace file list
    wxArrayString inclPath;
    wxArrayString exclPath;
    std::vector<wxFileName> files;

    wxFileName fn(wxT("../workspace_file.list"));
    wxString content = LoadFile(fn.GetFullPath());

    wxArrayString lines = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.GetCount(); i++) {
        wxString fname = lines.Item(i).Trim().Trim(false);
        if(fname.IsEmpty()) continue;
        files.push_back(lines.Item(i));
    }

    inclPath.Add(wxT("/usr/include/c++/4.4"));
    inclPath.Add(wxT("/usr/include/c++/4.4/x86_64-linux-gnu"));
    inclPath.Add(wxT("/usr/include/c++/4.4/backward"));
    inclPath.Add(wxT("/usr/local/include"));
    inclPath.Add(wxT("/usr/lib/gcc/x86_64-linux-gnu/4.4.3/include"));
    inclPath.Add(wxT("/usr/lib/gcc/x86_64-linux-gnu/4.4.3/include-fixed"));
    inclPath.Add(wxT("/usr/include"));
    inclPath.Add(wxT("/usr/include/qt4/QtCore"));
    inclPath.Add(wxT("/usr/include/qt4/QtGui"));
    inclPath.Add(wxT("/usr/include/qt4/QtXml"));
    inclPath.Add(wxT("/home/eran/wx29/include/wx-2.9"));

    // Set the search paths and start the parser thread
    ParseThreadST::Get()->SetSearchPaths(inclPath, exclPath);
    ParseThreadST::Get()->SetNotifyWindow(NULL);

    ParseThreadST::Get()->Start();

    // Perform a full retagging
    TagsManagerST::Get()->SetCodeLiteIndexerPath(wxT("/usr/bin"));
    TagsManagerST::Get()->StartCodeLiteIndexer();
    TagsManagerST::Get()->OpenDatabase(wxFileName(wxT("test.tags")));
    TagsManagerST::Get()->RetagFiles(files, TagsManager::Retag_Full);

    char line[100];
    gets(line);

    ParseThreadST::Get()->Stop();
    ParseThreadST::Free();
    TagsManagerST::Free();
    LanguageST::Free();
}
#endif

/**
 * @brief call the test framework
 */
int main(int argc, char** argv)
{
    // Initialize the wxWidgets library
    wxInitializer initializer;
// #ifdef __WXMSW__
//     TagsManagerST::Get()->SetCodeLiteIndexerPath("C:/Program Files/CodeLite/codelite_indexer.exe");
//     TagsManagerST::Get()->StartCodeLiteIndexer();
//     TagEntryPtrVector_t tags = TagsManagerST::Get()->ParseBuffer("enum class FooShort : short {"
//                                                                  "    short_apple,"
//                                                                  "    short_banana,"
//                                                                  "    short_orange,"
//                                                                  "};");
// #endif

    testCC();
    TagsManagerST::Free();
    return 0;
}
