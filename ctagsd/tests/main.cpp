#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "Cxx/CxxCodeCompletion.hpp"
#include "Cxx/CxxExpression.hpp"
#include "Cxx/CxxScannerTokens.h"
#include "Cxx/CxxTokenizer.h"
#include "Cxx/CxxVariableScanner.h"
#include "LSPUtils.hpp"
#include "Settings.hpp"
#include "SimpleTokenizer.hpp"
#include "clFilesCollector.h"
#include "ctags_manager.h"
#include "database/tags_storage_sqlite3.h"
#include "fileutils.h"
#include "macros.h"
#include "strings.hpp"

#define DOCTEST_CONFIG_IMPLEMENT
#include <algorithm>
#include <doctest.h>
#include <memory>
#include <wx/init.h>
#include <wx/log.h>
#include <wx/wxcrtvararg.h>

using namespace std;
namespace
{
thread_local bool cc_initialised = false;
thread_local bool cc_initialised_successfully = false;
thread_local ITagsStoragePtr lookup_table;
thread_local CxxCodeCompletion::ptr_t completer;

CTagsdSettings settings;

TagEntryPtr find_tag(const wxString& path, const vector<TagEntryPtr>& candidates)
{
    auto where = find_if(candidates.begin(), candidates.end(), [=](TagEntryPtr tag) { return tag->GetPath() == path; });
    if (where == candidates.end()) {
        return nullptr;
    }
    return *where;
}

TagEntryPtr find_tag_by_name(const wxString& name, const vector<TagEntryPtr>& candidates)
{
    auto where = find_if(candidates.begin(), candidates.end(), [=](TagEntryPtr tag) { return tag->GetName() == name; });
    if (where == candidates.end()) {
        return nullptr;
    }
    return *where;
}

bool is_tag_exists(const wxString& path, const vector<TagEntryPtr>& candidates)
{
    return find_tag(path, candidates) != nullptr;
}

bool is_file_exists(const wxString& suffix, wxString* fullpath)
{
    wxString codelite_src_dir;
    fullpath->clear();

    if (!::wxGetEnv("CODELITE_SRC_DIR", &codelite_src_dir)) {
        return false;
    }

    wxString path = codelite_src_dir + "/" + suffix;
    wxFileName fn(path);
    if (fn.FileExists()) {
        *fullpath = fn.GetFullPath();
        return true;
    }
    return false;
}

#define ENSURE_DB_LOADED()                                                                                \
    if (!initialize_cc_tests()) {                                                                         \
        FAIL("CC database not loaded. Please set environment variable TAGS_DB that points to `tags.db`"); \
    }

bool initialize_cc_tests()
{
    if (!cc_initialised) {
        cc_initialised = true;
        wxString tags_db;
        if (!wxGetEnv("TAGS_DB", &tags_db)) {
            cc_initialised_successfully = false;
            wxPrintf("Loading TAGS_DB=%s\n", tags_db);
        } else {
            wxFileName fn(tags_db);
            wxFileName fn_settings(fn.GetPath(), "ctagsd.json");
            settings.Load(fn_settings);

            lookup_table = ITagsStoragePtr(new TagsStorageSQLite());
            lookup_table->OpenDatabase(fn);

            completer.reset(new CxxCodeCompletion(lookup_table, settings.GetCodeliteIndexer()));
            completer->set_macros_table(settings.GetTokens());
            completer->set_types_table(settings.GetTypes());
            cc_initialised_successfully = true;
        }
    }
    return cc_initialised_successfully;
}

wxString get_sample_file(const wxString& filename)
{
    wxFileName current_file(__FILE__);
    wxFileName test_file(current_file.GetPath(), filename);
    test_file.AppendDir("src");
    test_file.AppendDir("samples");
    return test_file.GetFullPath();
}

} // namespace

TEST_CASE("test_lexing_raw_strings")
{
    wxString fullpath;
    if (is_file_exists("ctagsd/tests/strings.hpp", &fullpath)) {
        size_t raw_strings_counter = 0;
        CxxTokenizer tokenizer;
        CxxLexerToken token;

        wxString content;
        FileUtils::ReadFileContent(fullpath, content);
        tokenizer.Reset(content);

        while (tokenizer.NextToken(token)) {
            switch (token.GetType()) {
            case T_RAW_STRING:
                raw_strings_counter++;
                break;
            default:
                break;
            }
        }
        CHECK_EQ(raw_strings_counter, 24); // we expect 24 raw strings
    }
}

TEST_CASE("test_parsing_of_function_parameter")
{
    {
        const wxString code = "size_t CxxCodeCompletion::get_anonymous_tags(const wxString& name, const wxArrayString& "
                              "kinds, vector<TagEntryPtr>& tags) const {";
        CxxVariableScanner scanner(code, eCxxStandard::kCxx11, {}, false);
        auto vars = scanner.GetVariablesMap();
        CHECK(vars.empty());
    }
    {
        CxxVariableScanner scanner(cc_locals_with_typedef, eCxxStandard::kCxx11, {}, false);
        auto vars = scanner.GetVariablesMap();
        CHECK(!vars.empty());
        CHECK(vars.count("m_options") != 0);
    }
    {
        CxxVariableScanner scanner(cc_test_function_calls_parsing, eCxxStandard::kCxx11, {}, false);
        auto vars = scanner.GetVariablesMap();
        CHECK_EQ(vars.size(), 2);
    }
    {
        CxxVariableScanner scanner(cc_zero_locals, eCxxStandard::kCxx11, {}, false);
        auto vars = scanner.GetVariablesMap();
        CHECK_EQ(vars.size(), 0);
    }
    {
        CxxVariableScanner scanner(cc_one_locals, eCxxStandard::kCxx11, {}, false);
        auto vars = scanner.GetVariablesMap();
        CHECK_EQ(vars.size(), 1);
        REQUIRE(vars.count("one_local"));
        CHECK_EQ(vars["one_local"]->GetTypeAsString(), "wxString");
    }
}

TEST_CASE("text_cxx_assignment_from_global_method" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString code = "auto arr = ::wxStringTokenize();";
        completer->set_text(code, "", -1);
        auto resolved = completer->code_complete("arr.", {}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxArrayString");
    }
}

TEST_CASE("text_code_completion_manager_cc" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        auto resolved = completer->code_complete("wxCodeCompletionBoxManager::", {}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxCodeCompletionBoxManager");
    }
}

TEST_CASE("text_cxx_cc_with_problematic_typedef" * doctest::may_fail())
{
    ENSURE_DB_LOADED();

    wxString filepath;
    if (is_file_exists("codelite_terminal/main.cpp", &filepath)) {
        {
            completer->set_text(cc_locals_with_typedef, filepath, 88);
            auto resolved = completer->code_complete("m_options.", {}, nullptr);
            CHECK(resolved);
        }
    }
}

TEST_CASE("TestLSPLocation" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    vector<TagEntryPtr> candidates;

    wxString code = "LSP::SymbolInformation si; si.GetLocation().";
    completer->set_text(code, wxEmptyString, wxNOT_FOUND);
    TagEntryPtr resolved = completer->code_complete("si.GetLocation().", {"LSP"});
    if (resolved) {
        completer->get_completions(resolved, wxEmptyString, wxEmptyString, candidates, {"LSP"});
    }
    CHECK(resolved);
    CHECK_EQ(candidates.size(), 18);
}

TEST_CASE("TestCompletionHelper_get_expression")
{
    wxStringMap_t M = {
        {"m_string.", "m_string."},
        {"string name=m_string.", "m_string."},
        {"string name=m_string->", "m_string->"},
        {"get_details().get_name().", "get_details().get_name()."},
        {"foo(){ std::vector<std::string>::", "std::vector<std::string>::"},
        {"auto foo = [=](std::ve", "std::ve"},
        {"string name", "name"},
        {"if(!types.empty() && types.back() == T_IDENTIF", "T_IDENTIF"},
        {"if(!types.empty() && type", "type"},
        {"if(!typ", "typ"},
        {sample_text, "m_string."},
        {"for(const wxString& a: str", "str"},
        {"vector<wxString> split_function_signature", "split_function_signature"},
    };

    CompletionHelper helper;
    for (const wxStringMap_t::value_type& vt : M) {
        const wxString& raw_string = vt.first;
        const wxString& expected = vt.second;
        CHECK_EQ(helper.get_expression(raw_string, false), expected);
    }
}

TEST_CASE("TestSplitArgsNoArgumentName")
{
    wxString signature_1 = "(const LSP::ResponseMessage& response, wxEvtHandler* owner = nullptr)";
    wxString signature_2 =
        "(const LSP::ResponseMessage& response, wxEvtHandler* owner = nullptr, const vector<std::string>& response)";
    wxString signature_3 =
        "(const LSP::ResponseMessage& response, wxEvtHandler* owner = nullptr, vector<std::string>&& response)";
    CompletionHelper helper;

    wxString return_value;
    {
        auto args =
            helper.split_function_signature(signature_1, &return_value, CompletionHelper::STRIP_NO_DEFAULT_VALUES);
        CHECK_EQ(args.size(), 2);
    }
    {
        auto args = helper.split_function_signature(
            signature_2, &return_value, CompletionHelper::STRIP_NO_DEFAULT_VALUES | CompletionHelper::STRIP_NO_NAME);
        CHECK_EQ(args.size(), 3);
    }

    {
        auto args = helper.split_function_signature(
            signature_3, &return_value, CompletionHelper::STRIP_NO_DEFAULT_VALUES | CompletionHelper::STRIP_NO_NAME);
        CHECK_EQ(args.size(), 3);
    }
}

TEST_CASE("TestCompletionHelper_truncate_file_to_location")
{
    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content, 4, 6, CompletionHelper::TRUNCATE_EXACT_POS);
    auto lines = ::wxStringTokenize(f, "\n", wxTOKEN_RET_EMPTY_ALL);
    CHECK(!lines.empty());
    REQUIRE_EQ(lines.size(), 5);
    CHECK_EQ(lines[4].Trim().Trim(false), "V.");
}

TEST_CASE("TestCompletionHelper_truncate_file_to_location_invalid_input")
{
    wxString file_content_empty = "";

    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content_empty, 4, 6, CompletionHelper::TRUNCATE_EXACT_POS);
    auto lines = ::wxStringTokenize(f, "\n", wxTOKEN_RET_EMPTY_ALL);
    CHECK(lines.empty());
    CHECK_EQ(lines.size(), 0);
}

TEST_CASE("TestCompletionHelper_truncate_file_to_location_must_end_with_words")
{
    wxString file_content_z = "std::vector<TagEntryPtr>";

    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content_z, 0, 15, CompletionHelper::TRUNCATE_COMPLETE_WORDS);
    CHECK_EQ(f, "std::vector<TagEntryPtr");
}

TEST_CASE("test_from_expression_c_cast")
{
    {
        vector<CxxExpression> expr = CxxExpression::from_expression("(wxFontMapper::Get())->", nullptr);
        REQUIRE_EQ(expr.size(), 2);
        CHECK_EQ(expr[0].type_name(), "wxFontMapper");
        CHECK_EQ(expr[1].type_name(), "Get");
    }

    {
        vector<CxxExpression> expr = CxxExpression::from_expression("((wxString*)something)->Append().", nullptr);
        REQUIRE_EQ(expr.size(), 2);
        CHECK_EQ(expr[0].type_name(), "wxString");
        CHECK_EQ(expr[1].type_name(), "Append");
    }
}

TEST_CASE("test_cxx_code_complete_macro" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        TagEntryPtr resolved = completer->code_complete("wxTheApp->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxApp");
    }
    {
        TagEntryPtr resolved = completer->code_complete("wxTheClipboard->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxClipboard");
    }
}

TEST_CASE("test_cxx_code_complete_member_of_parent_class" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    wxString member_of_parent_class_file;
    member_of_parent_class_file = get_sample_file("member_of_parent_class.hpp");

    // set our scope at MyPlugin::foo
    completer->set_text(wxEmptyString, member_of_parent_class_file, 8);
    TagEntryPtr resolved = completer->code_complete("m_mgr->", {});
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "IManager");
}

TEST_CASE("TestCTagsManager_AutoCandidates" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        vector<TagEntryPtr> candidates;
        wxString fulltext = "wxCodeCompletionBoxManager::Get().";

        auto resolved = completer->code_complete(fulltext, {});
        CHECK(resolved);

        completer->get_completions(resolved, wxEmptyString, wxEmptyString, candidates, {});
        CHECK(!candidates.empty());
    }
    {
        vector<TagEntryPtr> candidates;
        CxxRemainder remainder;
        auto resolved = completer->code_complete("StringUtils::StripTerminalColouring", {}, &remainder);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "StringUtils");
        CHECK_EQ(remainder.filter, "StripTerminalColouring");
        completer->get_completions(resolved, remainder.operand_string, remainder.filter, candidates, {});
        CHECK(!candidates.empty());
        CHECK_EQ(candidates.size(), 2);
    }
}

TEST_CASE("TestSimpleTokenizer_Comments")
{
    SimpleTokenizer::Token token;
    SimpleTokenizer tokenizer(tokenizer_sample_file_0);
    vector<SimpleTokenizer::Token> tokens;
    while (tokenizer.next_comment(&token)) {
        tokens.push_back(token);
    }
    // we are expecting 2 comments
    CHECK_EQ(tokens.size(), 4);
}

TEST_CASE("TestSimpleTokenizer_2")
{
    SimpleTokenizer::Token token;
    SimpleTokenizer tokenizer(sample_text_language_h);
    vector<SimpleTokenizer::Token> tokens;
    vector<TokenWrapper> tokens_vec;
    while (tokenizer.next(&token)) {
        tokens.push_back(token);
        TokenWrapper t;
        t.token = token;
        tokens_vec.push_back(t);
    }

    // collect all interesting tokens from the document
    vector<int> encoded_arr;
    LSPUtils::encode_semantic_tokens(tokens_vec, &encoded_arr);
    for (size_t i = 0; i < encoded_arr.size() / 5; ++i) {
        const auto& tok = tokens_vec[i].token;
        wxUnusedVar(tok);
    }
}

TEST_CASE("TestSimpleTokenizer")
{
    SimpleTokenizer::Token token;
    SimpleTokenizer tokenizer(tokenizer_sample_file_1);
    vector<SimpleTokenizer::Token> tokens;
    while (tokenizer.next(&token)) {
        tokens.push_back(token);
    }
    CHECK_EQ(tokens.size(), 15);
}

TEST_CASE("TestSplitArgs")
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
        CHECK_EQ(args.size(), 2);
        CHECK(return_value.empty());
    }

    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_2, &return_value);
        CHECK_EQ(args.size(), 2);
        CHECK_EQ(return_value, "int");
    }
    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_3, &return_value);
        CHECK_EQ(args.size(), 3);
        CHECK(return_value.empty());
    }
    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_4, &return_value);
        CHECK_EQ(args.size(), 4);
        CHECK_EQ(return_value, "std::vector<int>::iterator*");
    }

    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_5, &return_value);
        CHECK_EQ(args.size(), 2);
        CHECK_EQ(return_value, "wxString::size_type");
    }
}

TEST_CASE("test_symlink_is_scandir")
{
    clFilesScanner scanner;
    wxArrayString files;
    scanner.Scan("/tmp/eran", files, "*.cpp", "", "");
}

TEST_CASE("test_cxx_expression")
{
    CxxRemainder remainder;
    vector<CxxExpression> exp1 = CxxExpression::from_expression("std::vector<int>::", &remainder);
    REQUIRE_EQ(exp1.size(), 2);
    CHECK_EQ(exp1[0].type_name(), "std");
    CHECK_EQ(exp1[0].operand_string(), "::");

    CHECK_EQ(exp1[1].type_name(), "vector");
    CHECK_EQ(exp1[1].operand_string(), "::");
    CHECK_EQ(exp1[1].template_init_list().size(), 1);

    vector<CxxExpression> exp2 = CxxExpression::from_expression("string.AfterFirst('(').", &remainder);
    CHECK_EQ(exp2.size(), 2);

    vector<CxxExpression> exp3 = CxxExpression::from_expression("string.AfterFirst('(')", &remainder);
    CHECK_EQ(exp3.size(), 1);
    CHECK_EQ(remainder.filter, "AfterFirst");
}

TEST_CASE("test_cxx_code_completion_both_subscript_and_arrow_operator" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        completer->set_text(cc_subscript_operator_2, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("my_json[1][2].", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "JSONItem");
    }
}

TEST_CASE("test_cxx_code_completion_anonymous_namespace" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString filepath;
        if (is_file_exists("ctagsd/tests/main.cpp", &filepath)) {
            completer->set_text(wxEmptyString, filepath, wxNOT_FOUND);
            TagEntryPtr resolved = completer->code_complete("completer->", {"std"});
            REQUIRE(resolved);
            CHECK_EQ(resolved->GetPath(), "CxxCodeCompletion");
        }
    }

    {
        // simple case
        completer->set_text("MyAnonStruct anon_struct;", get_sample_file("anonymous_class.hpp"), -1);
        auto resolved = completer->code_complete("anon_struct.", {}, nullptr);
        CHECK(resolved);
        vector<TagEntryPtr> tags;
        completer->get_completions(resolved, "->", "", tags, {});
        CHECK_EQ(tags.size(), 2);
    }

    {
        // test anonymous class within a unique pointer
        completer->set_text("unique_ptr<MyAnonStruct> P;", get_sample_file("anonymous_class.hpp"), -1);
        auto resolved = completer->code_complete("P->", {"std"}, nullptr);
        CHECK(resolved);
        vector<TagEntryPtr> tags;
        completer->get_completions(resolved, "->", "", tags, {});
        CHECK_EQ(tags.size(), 2);
    }

    {
        // test anonymous class within a unique pointer
        vector<TagEntryPtr> tags;
        auto filepath = get_sample_file("anonymous_class.hpp");
        completer->word_complete(filepath, -1, "MyAnonSt", "", {}, false, tags, {});
        CHECK(!tags.empty());
    }
}

TEST_CASE("test_cxx_code_completion_static_member" * doctest::may_fail())
{
    wxString filepath;
    ENSURE_DB_LOADED();
    {
        if (is_file_exists("LiteEditor/frame.cpp", &filepath)) {
            completer->set_text(wxEmptyString, filepath, 1180);
            TagEntryPtr resolved = completer->code_complete("m_theFrame->", {});
            REQUIRE(resolved);
            CHECK_EQ(resolved->GetPath(), "clMainFrame");
        }
    }
}

TEST_CASE("test_cxx_code_completion_wxwindow" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    vector<TagEntryPtr> candidates;
    auto resolved = completer->code_complete("EventNotifier::Get()->TopFrame()->", {});
    CHECK(resolved);
    completer->get_completions(resolved, "->", "GetEventHandler", candidates, {}, 1);
    CHECK(!candidates.empty());
    CHECK(is_tag_exists("wxWindowBase::GetEventHandler", candidates));
}

TEST_CASE("test_cxx_code_completion_wxwindow_get_event_handler" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    vector<TagEntryPtr> candidates;
    auto resolved = completer->code_complete("EventNotifier::Get()->TopFrame()->GetEventHandler()->", {});
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "wxEvtHandler");
}

TEST_CASE("test_cxx_code_completion_overloaded_function" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    TagEntryPtr resolved = completer->code_complete("wxString::Append", {"std"});
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "wxString");

    // vector<TagEntryPtr> tags;
    // completer->get_similar_tags(resolved, tags);
    // CHECK_EQ(tags.size(), 10);
}

TEST_CASE("test_cxx_code_completion_func_returning_unique_ptr_into_auto" * doctest::may_fail())
{
    ENSURE_DB_LOADED();

    wxString code = "auto s = return_unique_ptr_of_string();";
    completer->set_text(code, wxEmptyString, wxNOT_FOUND);
    CxxRemainder remainder;
    TagEntryPtr resolved = completer->code_complete("s->", {"std"}, &remainder);
    CHECK(resolved);
    vector<TagEntryPtr> tags;
    completer->get_completions(resolved, remainder.operand_string, remainder.filter, tags, {"std"});
    CHECK(is_tag_exists("std::basic_string::append", tags));
}

TEST_CASE("test_cxx_code_completion_template_inheritance" * doctest::may_fail())
{
    ENSURE_DB_LOADED();

    CxxRemainder remainder;
    TagEntryPtr resolved = completer->code_complete("CxxUTDialog::GetEventHandler()->", {}, &remainder);
    CHECK(resolved);
}

TEST_CASE("test_cxx_code_completion_init_from_ctor" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString code = "JSONItem item = new JSONItem();";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("item->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "JSONItem");
    }

    {
        wxString code = "auto item = JSONItem();";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("item[0].", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "JSONItem");
    }
}

TEST_CASE("test_cxx_code_completion_inheritance_children" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    TagEntryPtr resolved = completer->code_complete("EventNotifier::Get()->", {}, &remainder);
    CHECK(resolved);
    vector<TagEntryPtr> candidates;
    completer->get_completions(resolved, wxEmptyString, wxEmptyString, candidates, {});
    CHECK(!candidates.empty());
    CHECK(is_tag_exists("wxEvtHandler::Bind", candidates));
}

TEST_CASE("test_cxx_code_completion_arrow_operator" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        TagEntryPtr resolved = completer->code_complete("SmartPtr<wxString>->", {}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
    {
        TagEntryPtr resolved = completer->code_complete("SmartPtr<TagEntryPtr>->", {}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "SmartPtr");
    }
    {
        TagEntryPtr resolved = completer->code_complete("SmartPtr<TagEntryPtr>.Get()->", {}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "TagEntry");
    }
}

TEST_CASE("test_cxx_code_completion_subscript_operator" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString code = "vector<TagEntry> tags;";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("tags[0].", {"std"}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "TagEntry");
    }

    {
        wxString code = "vector<vector<wxString>> two_dim_array;";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("two_dim_array[0].", {"std"}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "std::vector");
    }
    {
        wxString code = "vector<vector<wxString>> two_dim_array;";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("two_dim_array[0][0].", {"std"}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        wxString code = "vector<vector<wxString>> two_dim_array;";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("two_dim_array[0][0][0].", {"std"}, nullptr);
        REQUIRE(resolved);
        CHECK((resolved->GetPath() == "wxUniChar" || resolved->GetPath() == "wxUniCharRef"));
    }
    {
        CxxRemainder remainder;
        auto resolved = completer->code_complete("unordered_map<int, wxString>[0].Tri", {"std"}, &remainder);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");

        vector<TagEntryPtr> candidates;
        completer->get_completions(resolved, remainder.operand_string, remainder.filter, candidates, {"std"});
        CHECK(!candidates.empty());
        CHECK(is_tag_exists("wxString::Trim", candidates));
    }
}

#if 1 // FAILED
TEST_CASE("test_ctags_locals" * doctest::may_fail())
{
    wxString file = get_sample_file("locals.hpp");
    wxString content;
    FileUtils::ReadFileContent(file, content);
    vector<TagEntryPtr> tags;
    CTags::ParseLocals({}, content, settings.GetCodeliteIndexer(), settings.GetMacroTable(), tags);
    CHECK(!tags.empty());

    TagEntryPtr tag_auto_var = find_tag("MyClass::Foo::auto_var", tags);
    REQUIRE(tag_auto_var);
    CHECK(tag_auto_var->is_auto());
    CHECK_EQ(tag_auto_var->get_assignment(), "V[0]");

    TagEntryPtr tag_item = find_tag("MyClass::Foo::item", tags);
    REQUIRE(tag_item);
    CHECK(tag_item->is_auto());
    CHECK_EQ(tag_item->get_assignment(), "V.begin()");
}
#endif

TEST_CASE("test_cxx_code_completion_lsp_location_locals" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    completer->set_text(cc_lsp_location, wxEmptyString, wxNOT_FOUND);
    vector<TagEntryPtr> locals = completer->get_locals(wxEmptyString);
    CHECK(!locals.empty());

    auto tag = find_tag_by_name("loc", locals);
    REQUIRE(tag);
    CHECK(tag->GetName() == "loc");
    CHECK(tag->GetScope() == "LSP::Location");
}

TEST_CASE("test_cxx_code_completion_variable_scanner_ranged_for")
{
    CxxVariableScanner scanner(cc_text_ranged_for, eCxxStandard::kCxx11, {}, false);
    auto vars = scanner.GetVariables();
    REQUIRE(vars.size() == 2);
    CHECK_EQ(vars[0]->GetName(), "array");
    CHECK_EQ(vars[1]->GetName(), "s");
}

TEST_CASE("test_cxx_code_completion_variable_scanner_normal_for")
{
    CxxVariableScanner scanner(cc_text_normal_for, eCxxStandard::kCxx11, {}, false);
    auto vars = scanner.GetVariables();
    REQUIRE(vars.size() == 2);
    CHECK_EQ(vars[0]->GetName(), "array");
    CHECK_EQ(vars[1]->GetName(), "iter");
}

TEST_CASE("test_cxx_code_completion_word_completion_externvar" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        CxxRemainder remainder;
        remainder.filter = "wxEmptyStr";
        remainder.operand_string = "";
        vector<TagEntryPtr> candidates;
        completer->get_word_completions(remainder, candidates, {"std"}, {});
        CHECK(!candidates.empty());
        CHECK(is_tag_exists("wxEmptyString", candidates));
    }
    {
        CxxRemainder remainder;
        remainder.filter = "wxDefaultSi";
        remainder.operand_string = "";
        vector<TagEntryPtr> candidates;
        completer->get_word_completions(remainder, candidates, {"std"}, {});
        CHECK(!candidates.empty());
        CHECK(is_tag_exists("wxDefaultSize", candidates));
    }
    {
        auto resolved = completer->code_complete("wxDefaultSize.", {}, nullptr);
        CHECK(resolved);
    }
}

TEST_CASE("test_cxx_code_completion_word_completion" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        CxxRemainder remainder;
        TagEntryPtr resolved = completer->code_complete("wxStr", {}, &remainder);
        CHECK_EQ(remainder.filter, "wxStr");
        vector<TagEntryPtr> candidates;
        completer->get_word_completions(remainder, candidates, {"std"}, {});
        CHECK(!candidates.empty());
    }
    {
        CxxRemainder remainder;
        wxString filename = R"(C:\src\codelite\CodeLite\ctags_manager.cpp)";
        completer->set_text(wxEmptyString, filename, 263);
        TagEntryPtr resolved = completer->code_complete("wxStr", {}, &remainder);
        CHECK_EQ(remainder.filter, "wxStr");
        vector<TagEntryPtr> candidates;
        completer->get_word_completions(remainder, candidates, {"std"}, {});
        CHECK(!candidates.empty());
        CHECK(is_tag_exists("wxString", candidates));
    }
    {
        CxxRemainder remainder;
        TagEntryPtr resolved = completer->code_complete("std::str", {}, &remainder);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "std");
        CHECK_EQ(remainder.filter, "str");
        vector<TagEntryPtr> candidates;
        completer->get_completions(resolved, remainder.operand_string, remainder.filter, candidates, {});
        CHECK(!candidates.empty());

        // check that we found "std::string"
        bool exists = is_tag_exists("std::string", candidates);
        CHECK(exists);
    }
}

TEST_CASE("test_cxx_word_completion_inside_scope" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    wxString filename;
    wxString filename_2;

    if (is_file_exists("CodeLite/JSON.cpp", &filename)) {
        vector<TagEntryPtr> candidates;
        completer->set_text(wxEmptyString, filename, 191);
        CxxRemainder remainder;
        remainder.filter = "addProp";
        size_t count = completer->get_word_completions(remainder, candidates, {}, {});
        CHECK(is_tag_exists("JSONItem::addProperty", candidates));
        CHECK(count > 0);
    }

    if (is_file_exists("LiteEditor/context_cpp.cpp", &filename_2)) {
        {
            vector<TagEntryPtr> candidates;
            completer->set_text(wxEmptyString, filename_2, 200);
            CxxRemainder remainder;
            remainder.filter = "GetCt";
            size_t count = completer->get_word_completions(remainder, candidates, {}, {});
            CHECK(count > 0);
            CHECK(is_tag_exists("ContextBase::GetCtrl", candidates));
        }

        {
            completer->set_text(wxEmptyString, filename_2, 200);
            auto resolved = completer->code_complete("GetCtrl().", {}, nullptr);
            REQUIRE(resolved);
            CHECK_EQ(resolved->GetPath(), "clEditor");
        }
    }
}

TEST_CASE("test_cxx_code_completion_macros" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    {
        vector<TagEntryPtr> candidates;
        completer->word_complete(wxEmptyString, wxNOT_FOUND, "CHECK_EQ_ONE", wxEmptyString, {}, false, candidates);
        REQUIRE_EQ(candidates.size(), 1);
        CHECK_EQ(candidates[0]->GetName(), "CHECK_EQ_ONE_OF");
    }
    {
        vector<TagEntryPtr> candidates;
        completer->word_complete(wxEmptyString, wxNOT_FOUND, "CHECK_BOO", wxEmptyString, {}, false, candidates);
        CHECK(!candidates.empty());
        CHECK(is_tag_exists("CHECK", candidates));
    }
}

TEST_CASE("test_cxx_code_completion_c_style_enum" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    vector<TagEntryPtr> candidates;
    completer->word_complete(wxEmptyString, wxNOT_FOUND, "wxTOKEN_ST", wxEmptyString, {}, false, candidates);
    REQUIRE_EQ(candidates.size(), 1);
    CHECK_EQ(candidates[0]->GetName(), "wxTOKEN_STRTOK");
}

TEST_CASE("test_cxx_code_completion_casting" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    TagEntryPtr resolved = completer->code_complete("dynamic_cast<wxString*>(something)->", {});
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "wxString");
}

TEST_CASE("test_cxx_code_completion_class_enum" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    vector<TagEntryPtr> candidates;
    TagEntryPtr resolved = completer->code_complete("eCxxStandard::", {});
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "eCxxStandard");
    CHECK_EQ(resolved->GetKind(), "enum");

    // find the enumerators
    vector<TagEntryPtr> tags;
    completer->get_completions(resolved, wxEmptyString, wxEmptyString, tags, {});
    CHECK_EQ(tags.size(), 2);
}

TEST_CASE("test_cxx_code_completion_this_and_global_scope" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    // use a line inside CxxCodeCompletion file
    wxString filename;
    wxString filename_2;
    TagEntryPtr resolved;
    if (is_file_exists("CodeLite/ctags_manager.cpp", &filename)) {
        completer->set_text(wxEmptyString, filename, 149);
        resolved = completer->code_complete("this->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "TagsManager");
    }

    if (is_file_exists("CodeLite/CxxCodeCompletion.cpp", &filename_2)) {
        completer->set_text(wxEmptyString, filename_2, 78);
        resolved = completer->code_complete("m_template_manager->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "TemplateManager");
    }

    resolved = completer->code_complete("::", {});
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "<global>");

    CxxRemainder remainder;
    resolved = completer->code_complete("wxS", {}, &remainder);
    CHECK_EQ(remainder.filter, "wxS");
}

TEST_CASE("test_cxx_code_completion_vector_of_pairs" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;

    completer->set_text(wxEmptyString, wxEmptyString, wxNOT_FOUND);
    auto resolved = completer->code_complete("vector<pair<wxString, wxString>>.at(0).", {"std"}, &remainder);
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "std::pair");
}

TEST_CASE("test_cxx_code_completion_pointer_type_in_template" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;

    wxString code = "vector<ITest*> m_tests;";
    completer->set_text(code, wxEmptyString, wxNOT_FOUND);
    auto resolved = completer->code_complete("m_tests.at(i)->tes", {"std"}, &remainder);
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "ITest");
    CHECK_EQ(remainder.filter, "tes");
}

TEST_CASE("test_cxx_code_completion_invalid_completions" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    vector<TagEntryPtr> candidates;
    completer->word_complete(
        wxEmptyString, wxNOT_FOUND, "does_not_exist->tes", wxEmptyString, {"std"}, false, candidates);
    CHECK_EQ(candidates.size(), 0);
}

TEST_CASE("test_cxx_code_completion_word_complete_cenum" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    vector<TagEntryPtr> candidates;
    completer->word_complete(wxEmptyString, wxNOT_FOUND, "eAsc", wxEmptyString, {}, false, candidates);
    CHECK(!candidates.empty());
}

TEST_CASE("test_cxx_code_completion_global_method" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        auto resolved = completer->code_complete("clGetManager()->", {}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "IManager");
    }
    {
        // try again, this time with the global namespace prefix
        auto resolved = completer->code_complete("::clGetManager()->", {}, nullptr);
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "IManager");
    }
}

TEST_CASE("test_cxx_code_completion_after_adding_func_impl" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    // use a line inside CxxCodeCompletion file
    wxString filename;
    if (!is_file_exists("Plugin/clFileSystemWorkspace.cpp", &filename)) {
        return;
    }
    {
        completer->set_text(wxEmptyString, filename, 1117); // inside clFileSystemWorkspace::ReloadWorkspace
        auto resolved = completer->code_complete("this->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "clFileSystemWorkspace");

        vector<TagEntryPtr> candidates;
        completer->get_completions(resolved, wxEmptyString, wxEmptyString, candidates, {});
        CHECK(!candidates.empty());
        CHECK_EQ(candidates.size(), 194);
    }
}

TEST_CASE("test_cxx_code_completion_member_variable_in_scope" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    // use a line inside CxxCodeCompletion file
    wxString filename;
    if (!is_file_exists("CodeLite/ctags_manager.cpp", &filename)) {
        return;
    }
    {
        completer->set_text(wxEmptyString, filename, 210); // inside TagsManager::TreeFromTags
        auto resolved = completer->code_complete("m_watch.", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxStopWatch");
    }
}

TEST_CASE("test_cxx_code_completion_list_locals" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        completer->set_text(tokenizer_sample_file_0, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_EQ(locals.size(), 0);
    }
    {
        completer->set_text(tokenizer_sample_file_2, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_EQ(locals.size(), 0);
    }

    {
        completer->set_text(file_content, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_EQ(locals.size(), 1);
    }

    {
        completer->set_text(cc_text_auto_chained, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        REQUIRE_EQ(locals.size(), 2);
        CHECK_EQ(locals[0]->GetName(), "str");
        CHECK_EQ(locals[1]->GetName(), "arr");
    }

    {
        completer->set_text(big_file, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        // we expect 2 locals
        CHECK_EQ(locals.size(), 2);
    }

    {
        completer->set_text(cc_lamda_text, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_EQ(locals.size(), 2);
    }
}

TEST_CASE("test_cxx_code_completion_full_ns_path" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    auto resolved = completer->code_complete("LSP::Params::Ptr_t->", {}, nullptr);
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "LSP::Params");
}

TEST_CASE("test_cxx_code_completion_function_arguments" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    wxString filepath;
    wxString filepath2;
    if (is_file_exists("ctagsd/lib/ProtocolHandler.cpp", &filepath)) {
        {
            completer->set_text(wxEmptyString,
                                filepath,
                                430); // ProtocolHandler::on_initialize(unique_ptr<JSON>&& msg, Channel::ptr_t channel)
            TagEntryPtr resolved = completer->code_complete("msg->", {"std"});
            REQUIRE(resolved);
            CHECK_EQ(resolved->GetPath(), "JSON");
        }
        {
            completer->set_text(wxEmptyString,
                                filepath,
                                430); // ProtocolHandler::on_initialize(unique_ptr<JSON>&& msg, Channel::ptr_t channel)
            TagEntryPtr resolved = completer->code_complete("channel->", {"std"});
            REQUIRE(resolved);
            CHECK_EQ(resolved->GetPath(), "Channel");
        }
        {
            vector<TagEntryPtr> candidates;
            completer->set_text(wxEmptyString,
                                filepath,
                                430); // ProtocolHandler::on_initialize(unique_ptr<JSON>&& msg, Channel::ptr_t channel)
            size_t count =
                completer->word_complete(filepath, 430, "channel", wxEmptyString, {"std"}, true, candidates, {});
            CHECK(count > 0);
            CHECK(is_tag_exists("ProtocolHandler::on_initialize::channel", candidates));
        }
    }
    if (is_file_exists("Plugin/navigationmanager.cpp", &filepath2)) {
        {
            vector<TagEntryPtr> candidates;
            size_t count = completer->word_complete(filepath2, 86, "editor", wxEmptyString, {}, true, candidates, {});
            REQUIRE_EQ(count, 1);
            CHECK_EQ(candidates[0]->GetTypename(), "IEditor *");
        }
    }
}

TEST_CASE("test_cxx_code_completion_rvalue_reference" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString text = "TagEntry&& entry;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("entry.m_path.", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
}

TEST_CASE("test_cxx_code_completion_member_variable" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString text = "TagEntry entry;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("entry.m_path.", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
}

TEST_CASE("test_cxx_code_completion_template_function" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        TagEntryPtr resolved = completer->code_complete("get_as_type<wxString>()->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        wxString code = "auto resolved = get_as_type<wxString>();";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("resolved.", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
}

TEST_CASE("test_cxx_code_completion_template_std_set" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString text = "set<wxString> S;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("S.begin()->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        TagEntryPtr resolved = completer->code_complete("wxStringSet_t::value_type::", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
}

TEST_CASE("test_cxx_code_completion_template_std_multiset" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString text = "multiset<wxString> S;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("S.begin()->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        TagEntryPtr resolved = completer->code_complete("multiset<wxString>::value_type::", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
}

TEST_CASE("test_cxx_code_completion_template_std_unordered_multiset" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString text = "unordered_multiset<wxString> S;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("S.begin()->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        TagEntryPtr resolved = completer->code_complete("unordered_multiset<wxString>::value_type::", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
}

TEST_CASE("test_cxx_code_completion_template_std_map" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    {
        wxString text = "map<wxString> M;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("M.begin()->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "std::pair");
    }

    {
        TagEntryPtr resolved = completer->code_complete("wxStringMap_t::value_type.", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "std::pair");
    }
}

TEST_CASE("test_cxx_code_completion_typedef_using" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
    wxString text = "ClangFormatMap compiledMap;";
    completer->set_text(text, wxEmptyString, wxNOT_FOUND);
    TagEntryPtr resolved = completer->code_complete("ClangFormatMap.begin()->first.", {"FormatOptions"});
    REQUIRE(resolved);
    CHECK_EQ(resolved->GetPath(), "wxString");
}

TEST_CASE("test_cxx_code_completion_template" * doctest::may_fail())
{
    ENSURE_DB_LOADED();
#if 0
    {
        wxString text = "cJSON* json;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("json->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "cJSON");
        vector<TagEntryPtr> tags;
        completer->get_completions(resolved, wxEmptyString, wxEmptyString, tags, {});
        CHECK(is_tag_exists("valuestring", tags));
    }
#endif

    {
        wxString text = "wxVector<wxString> V;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("V.at(0).", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
    {
        // template inheritance in typedef
        // using ManagerST = Singleton<Manager>;
        completer->reset();
        TagEntryPtr resolved = completer->code_complete("ManagerST::Get()->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "Manager");
    }

    {
        // template inheritance
        // class ContextManager : public Singleton<ContextManager> {...}
        // ContextManager::Get()->
        completer->reset();
        TagEntryPtr resolved = completer->code_complete("ContextManager::Get()->", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "ContextManager");
    }

    {
        wxString text = "map<wxString, wxArrayString> M;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("M.at(str).", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxArrayString");
    }

    {
        wxString text = "shared_ptr<wxString> P;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("P.", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "std::shared_ptr");
    }

    {
        wxString text = "shared_ptr<wxString> P;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("P->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
    {
        wxString text = "unique_ptr<wxString> P;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("P->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }
    {
        completer->set_text("CxxVariable::Map_t varsMap;", wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("varsMap.begin()->first.", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text("CxxVariable::Map_t varsMap;", wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("varsMap.begin()->second->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "CxxVariable");
    }

    {
        completer->set_text("CxxVariable::Map_t varsMap;", wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("varsMap.begin()->second.", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "SmartPtr");
    }

    {
        completer->set_text("unordered_set<CxxVariable> varsSet;", wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("varsSet.begin()->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "CxxVariable");
    }

    {
        TagEntryPtr resolved = completer->code_complete("map<wxString, CxxVariable::Ptr_t>::begin()->first.", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        TagEntryPtr resolved = completer->code_complete("map<wxString, CxxVariable::Ptr_t>::begin()->second.", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "SmartPtr");
    }
    {
        TagEntryPtr resolved =
            completer->code_complete("map<wxString, CxxVariable::Ptr_t>::begin()->second->", {"std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "CxxVariable");
    }
}

TEST_CASE("test_cxx_code_completion" * doctest::may_fail())
{
    ENSURE_DB_LOADED();

    {
        wxString text = "std::string str;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "std::basic_string");
    }

    {
        completer->set_text(cc_text_auto_chained, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_auto_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("wxString::", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.AfterFirst('(').", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.AfterFirst('(').BeforeFirst().", {});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_lsp_event, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("event.GetLocation().GetRange().GetStart().", {"LSP", "std"});
        REQUIRE(resolved);
        CHECK_EQ(resolved->GetPath(), "LSP::Position");
    }
}

int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    wxLogNull NOLOG;
#if 0
    clFileExtensionMatcher ext_matcher{"*.rs"};
    // clPathExcluder path_excluder{ "*/build" };
    clPathExcluder path_excluder{wxEmptyString};

    wxArrayString files;
    wxStringSet_t visited_dirs;
    wxStringSet_t scannedFiles;
    wxArrayString rootDirs;
    rootDirs.Add("/home/ANT.AMAZON.COM/eifrah/workplace/ElastiCacheProxyRust/src/ElastiCacheProxyRust");
    for (size_t i = 0; i < rootDirs.size(); ++i) {
        // collect only unique files that are matching the pattern
        auto on_file = [&](const wxArrayString& paths) {
            for (const auto& fullpath : paths) {
                if (scannedFiles.insert(fullpath).second && ext_matcher.matches(fullpath)) {
                    files.Add(fullpath);
                }
            }
        };

        // do not traverse into excluded directories or directories that
        // we already visited
        auto on_folder = [&](const wxString& fullpath) -> bool {
            if (!visited_dirs.insert(fullpath).second) {
                return false;
            }
            return !path_excluder.is_exclude_path(fullpath);
        };

        // make sure it's really a dir (not a fifo, etc.)
        clFilesScanner scanner;
        scanner.ScanWithCallbacks(rootDirs.Item(i),
                                  on_folder,
                                  on_file,
                                  clFilesScanner::SF_DONT_FOLLOW_SYMLINKS | clFilesScanner::SF_EXCLUDE_HIDDEN_DIRS);
    }

    cout << "found: " << files.size() << " files" << endl;
    return 0;
#endif

    // ensure that the user data dir exists
    wxFileName::Mkdir(clStandardPaths::Get().GetUserDataDir(), wxPosixPermissions::wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    return doctest::Context(argc, argv).run();
}
