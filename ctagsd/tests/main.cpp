#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "CxxTokenizer.h"
#include "CxxVariableScanner.h"
#include "LSPUtils.hpp"
#include "Settings.hpp"
#include "SimpleTokenizer.hpp"
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
            tod.SetTypes(map_to_wxString(settings.GetTokens()));

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
    };

    CompletionHelper helper;
    for(const auto& vt : M) {
        const wxString& raw_string = vt.first;
        const wxString& expected = vt.second;
        CHECK_STRING(helper.get_expression(raw_string), expected);
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
    wxString f = helper.truncate_file_to_location(file_content, 4, 6);
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
    wxString f = helper.truncate_file_to_location(file_content, 4, 6);
    auto lines = ::wxStringTokenize(f, "\n", wxTOKEN_RET_EMPTY_ALL);
    CHECK_BOOL(lines.empty());
    CHECK_SIZE(lines.size(), 0);
    return true;
}

TEST_FUNC(TestCTagsManager_AutoCandidates_unique_ptr)
{
    if(!initialize_cc_tests()) {
        cout << "CC database not loaded. Please set environment variable TAGS_DB that points to `tags.db`" << endl;
        return true;
    }
    vector<TagEntryPtr> candidates;
    wxString fulltext = "std::unique_ptr<std::string> mystr; mystr->";

    clTempFile tmpfile("cpp");
    tmpfile.Write(fulltext);
    TagsManagerST::Get()->AutoCompleteCandidates(tmpfile.GetFullPath(), 1, "mystr->", fulltext, candidates);
    CHECK_BOOL(!candidates.empty());
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
#if 0
    {
        SimpleTokenizer::Token token;
        wxString file_content;
        FileUtils::ReadFileContent(wxFileName("C:\\src\\codelite\\codelite_terminal\\wxTerminalColourHandler.cpp"),
                                   file_content);
        SimpleTokenizer tokenizer(file_content);
        vector<SimpleTokenizer::Token> tokens;
        vector<TokenWrapper> tokens_vec;

        while(tokenizer.next(&token)) {
            tokens_vec.push_back({ token, eTokenType::TYPE_CLASS });
        }

        vector<int> encoding;
        LSPUtils::encode_semantic_tokens(tokens_vec, &encoding);
    }
#endif
    return true;
}

int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    wxLogNull NOLOG;
    Tester::Instance()->RunTests();
    return 0;
}
