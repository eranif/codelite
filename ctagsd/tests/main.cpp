#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "CxxTokenizer.h"
#include "CxxVariableScanner.h"
#include "LSPUtils.hpp"
#include "Settings.hpp"
#include "SimpleTokenizer.hpp"
#include "clFilesCollector.h"
#include "clTempFile.hpp"
#include "ctags_manager.h"
#include "fileutils.h"
#include "macros.h"
#include "tester.hpp"
#include <iostream>
#include <stdio.h>
#include <wx/init.h>
#include <wx/log.h>
#include <wxStringHash.h>

using namespace std;
namespace
{
ostream& operator<<(ostream& stream, const vector<wxString>& arrstr)
{
    wxString s;
    s << "[";
    for(const auto& str : arrstr) {
        s << str << ",";
    }
    if(!s.empty()) {
        s.RemoveLast();
    }
    s << "]";
    stream << s;
    return stream;
}

ostream& operator<<(ostream& stream, const pair<vector<SimpleTokenizer::Token>, wxString>& pr)
{
    wxString s;
    SimpleTokenizer helper({});
    const vector<SimpleTokenizer::Token>& tokens = pr.first;
    const wxString& source_string = pr.second;
    for(const SimpleTokenizer::Token& token : tokens) {
        wxString as_str = token.to_string(source_string);
        helper.strip_comment(as_str);
        s << "Line: " << token.line() << "\n"
          << "Comment:\n"
          << as_str << "\n------------\n";
    }
    stream << s << endl;
    return stream;
}

const wxString sample_cxx_file = R"(
class MyClass {
    std::string m_name;
    
public:
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
};

int main(int argc, char** argv)
{
    std::string name = "hello";
    MyClass cls, cls2;
    cls.SetName(name);
    return 0;
}
)";

const wxString sample_text = R"(
#ifdef __WXMSW__
#endif
m_string.)";

thread_local bool cc_initialised = false;
thread_local bool cc_initialised_successfully = false;
CTagsdSettings settings;

wxString map_to_wxString(const wxStringMap_t& m)
{
    wxString s;
    for(const auto& vt : m) {
        s << vt.first;
        if(!vt.second.empty()) {
            s << "=" << vt.second;
        }
        s << "\n";
    }
    return s;
}

bool initialize_cc_tests()
{
    if(!cc_initialised) {
        cc_initialised = true;
        wxString tags_db;
        if(!wxGetEnv("TAGS_DB", &tags_db)) {
            cc_initialised_successfully = false;
        } else {
            wxFileName fn(tags_db);
            TagsManagerST::Get()->CloseDatabase();

            wxFileName fn_settings(fn.GetPath(), "settings.json");
            settings.Load(fn_settings);

            TagsOptionsData tod;
            tod.SetTypes(map_to_wxString(settings.GetTypes()));
            tod.SetTokens(map_to_wxString(settings.GetTokens()));

            TagsManagerST::Get()->OpenDatabase(fn.GetFullPath());
            TagsManagerST::Get()->SetCtagsOptions(tod);
            cc_initialised_successfully = true;
        }
    }
    return cc_initialised_successfully;
}
} // namespace

TEST_FUNC(TestCompletionHelper_get_expression)
{
    wxStringMap_t M = {
        { "m_string.", "m_string." },
        { "string name=m_string.", "m_string." },
        { "string name=m_string->", "m_string->" },
        { "get_details().get_name().", "get_details().get_name()." },
        { "foo(){ std::vector<std::string>::", "std::vector<std::string>::" },
        { "auto foo = [=](std::ve", "std::ve" },
        { "string name", "name" },
        { "if(!types.empty() && types.back() == T_IDENTIF", "T_IDENTIF" },
        { "if(!types.empty() && type", "type" },
        { "if(!typ", "typ" },
        { sample_text, "m_string." },
        { "for(const wxString& a: str", "str" },
        { "vector<wxString> split_function_signature", "split_function_signature" },
    };

    CompletionHelper helper;
    for(const auto& vt : M) {
        const wxString& raw_string = vt.first;
        const wxString& expected = vt.second;
        CHECK_STRING(helper.get_expression(raw_string, false), expected);
    }
    return true;
}

TEST_FUNC(TestCompletionHelper_truncate_file_to_location)
{
    wxString file_content =
        R"(
using namespace std;
void foo() {
    vector<string> V;
    V.empty();
    // other content goes here
    // bla bla
)";
    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content, 4, 6, false);
    auto lines = ::wxStringTokenize(f, "\n", wxTOKEN_RET_EMPTY_ALL);
    CHECK_BOOL(!lines.empty());
    CHECK_SIZE(lines.size(), 5);
    CHECK_STRING(lines[4].Trim().Trim(false), "V.");
    return true;
}

TEST_FUNC(TestCompletionHelper_truncate_file_to_location_invalid_input)
{
    wxString file_content = "";

    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content, 4, 6, false);
    auto lines = ::wxStringTokenize(f, "\n", wxTOKEN_RET_EMPTY_ALL);
    CHECK_BOOL(lines.empty());
    CHECK_SIZE(lines.size(), 0);
    return true;
}

TEST_FUNC(TestCompletionHelper_truncate_file_to_location_must_end_with_words)
{
    wxString file_content = "std::vector<TagEntryPtr>";

    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content, 0, 15, true);
    CHECK_STRING(f, "std::vector<TagEntryPtr");
    return true;
}

TEST_FUNC(TestCTagsManager_AutoCandidates)
{
    if(!initialize_cc_tests()) {
        cout << "CC database not loaded. Please set environment variable TAGS_DB that points to `tags.db`" << endl;
        return true;
    }

    vector<TagEntryPtr> candidates;
    wxString fulltext = "wxCodeCompletionBoxManager::Get().";

    clTempFile tmpfile("cpp");
    tmpfile.Write(fulltext);
    TagsManagerST::Get()->WordCompletionCandidates(tmpfile.GetFullPath(), 1, "wxCodeCompletionBoxManager::Get().",
                                                   fulltext, "ShowCompletionBox", candidates);
    CHECK_BOOL(!candidates.empty());

    return true;
}

TEST_FUNC(TestCTagsManager_AutoCandidates_unique_ptr)
{
    if(!initialize_cc_tests()) {
        cout << "CC database not loaded. Please set environment variable TAGS_DB that points to `tags.db`" << endl;
        return true;
    }

    vector<TagEntryPtr> candidates;
    wxString fulltext = "std::unique_ptr<std::string>&& ptr; ptr->";

    clTempFile tmpfile("cpp");
    tmpfile.Write(fulltext);
    TagsManagerST::Get()->AutoCompleteCandidates(tmpfile.GetFullPath(), 1, "ptr->",
                                                   fulltext, candidates);
    CHECK_BOOL(!candidates.empty());
    return true;
}

TEST_FUNC(TestSimeplTokenizer_Comments)
{
    const wxString tokenizer_sample_file = R"(#include <unistd.h>
/** comment **/
int main(int argc, char** argv)
{
    /**
        multi line comment
        
    **/
    std::string name = "hello\nworld";
    MyClass cls, 1cls2;
    cls.SetName(name);
    return 0;
}
//last line comment
//)";
    SimpleTokenizer::Token token;
    SimpleTokenizer tokenizer(tokenizer_sample_file);
    vector<SimpleTokenizer::Token> tokens;
    while(tokenizer.next_comment(&token)) {
        tokens.push_back(token);
    }
    // we are expecting 2 comments
    CHECK_SIZE(tokens.size(), 4);
    cout << make_pair(tokens, tokenizer_sample_file) << endl;
    return true;
}

TEST_FUNC(TestSimeplTokenizer_Comments_From_File)
{
    wxString content;
    if(!FileUtils::ReadFileContent(wxFileName("/home/eran/devl/codelite/Plugin/project.h"), content)) {
        return true;
    }

    SimpleTokenizer::Token token;
    SimpleTokenizer tokenizer(content);
    vector<SimpleTokenizer::Token> tokens;
    while(tokenizer.next_comment(&token)) {
        tokens.push_back(token);
    }
    // we are expecting 2 comments
    cout << make_pair(tokens, content) << endl;
    return true;
}

TEST_FUNC(TestSimeplTokenizer)
{
    const wxString tokenizer_sample_file = R"(#include <unistd.h>
/** comment **/
int main(int argc, char** argv)
{
    /**
        multi line comment
        
    **/
    std::string name = "hello\nworld";
    MyClass cls, 1cls2;
    cls.SetName(name);
    return 0;
}
)";

    const wxString tokenizer_sample_file_2 = R"(#include "TextView.h"
#include "wxTerminalColourHandler.h"
#include "wxTerminalCtrl.h"

#ifdef __WXMSW__
#include "wx/msw/wrapwin.h" // includes windows.h
#endif
#include <fileutils.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

wxTerminalColourHandler::wxTerminalColourHandler()
{
    // we use the Ubuntu colour scheme
    // Text colours
    m_colours.insert({ 30, wxColour(1, 1, 1) });
)";
    {
        SimpleTokenizer::Token token;
        SimpleTokenizer tokenizer(tokenizer_sample_file);
        vector<SimpleTokenizer::Token> tokens;
        while(tokenizer.next(&token)) {
            tokens.push_back(token);
        }
        CHECK_SIZE(tokens.size(), 15);
    }
    return true;
}

TEST_FUNC(TestSplitArgs)
{
    const wxString signature_1 = "(const LSP::ResponseMessage& response, wxEvtHandler* owner)";
    const wxString signature_2 = "(int argc, char** argv) -> int";
    const wxString signature_3 = "void : (const wxString& name, std::function<void(const JSONItem& item)> "
                                 "deserialiser_func, const wxFileName& configFile = {})";
    const wxString signature_4 = "(const wxString& name, std::function<void(const JSONItem& item)> "
                                 "deserialiser_func, const wxFileName& configFile = {}, std::string value = "
                                 "\"default_string\") -> std::vector<int>::iterator*";
    const wxString signature_5 = "(unique_ptr<JSON>&& msg, Channel& channel) -> wxString::size_type";
    CompletionHelper helper;
    wxString return_value;

    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_1, &return_value);
        CHECK_SIZE(args.size(), 2);
        CHECK_BOOL(return_value.empty());
        cout << args << endl;
    }

    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_2, &return_value);
        CHECK_SIZE(args.size(), 2);
        CHECK_STRING(return_value, "int");
        cout << args << endl;
    }
    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_3, &return_value);
        CHECK_SIZE(args.size(), 3);
        CHECK_BOOL(return_value.empty());
        cout << args << endl;
    }
    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_4, &return_value);
        CHECK_SIZE(args.size(), 4);
        CHECK_STRING(return_value, "std::vector<int>::iterator*");
        cout << args << endl;
    }

    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_5, &return_value);
        CHECK_SIZE(args.size(), 2);
        CHECK_STRING(return_value, "wxString::size_type");
        cout << args << endl;
    }

    return true;
}

TEST_FUNC(test_symlink_is_scandir)
{
    clFilesScanner scanner;
    wxArrayString files;
    scanner.Scan("/tmp/eran", files, "*.cpp", "", "");
    return true;
}

int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    wxLogNull NOLOG;
    Tester::Instance()->RunTests();
    return 0;
}
