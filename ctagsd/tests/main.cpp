#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "CxxCodeCompletion.hpp"
#include "CxxExpression.hpp"
#include "CxxScannerTokens.h"
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
#include "strings.hpp"
#include "tags_storage_sqlite3.h"
#include "tester.hpp"

#include <iostream>
#include <stdio.h>
#include <unordered_set>
#include <wx/init.h>
#include <wx/log.h>
#include <wxStringHash.h>

using namespace std;
namespace
{
thread_local bool cc_initialised = false;
thread_local bool cc_initialised_successfully = false;
thread_local ITagsStoragePtr lookup_table;
thread_local CxxCodeCompletion::ptr_t completer;

CTagsdSettings settings;

bool is_tag_exists(const wxString& path, const vector<TagEntryPtr>& candidates)
{
    auto where = find_if(candidates.begin(), candidates.end(), [=](TagEntryPtr tag) { return tag->GetPath() == path; });
    return where != candidates.end();
}

#define ENSURE_DB_LOADED()                                                                                          \
    if(!initialize_cc_tests()) {                                                                                    \
        cout << "CC database not loaded. Please set environment variable TAGS_DB that points to `tags.db`" << endl; \
        return true;                                                                                                \
    }

bool initialize_cc_tests()
{
    if(!cc_initialised) {
        cc_initialised = true;
        wxString tags_db;
        if(!wxGetEnv("TAGS_DB", &tags_db)) {
            cc_initialised_successfully = false;
            wxPrintf("Loading TAGS_DB=%s\n", tags_db);
        } else {
            wxFileName fn(tags_db);
            wxFileName fn_settings(fn.GetPath(), "ctagsd.json");
            settings.Load(fn_settings);

            lookup_table = ITagsStoragePtr(new TagsStorageSQLite());
            lookup_table->OpenDatabase(fn);

            completer.reset(new CxxCodeCompletion(lookup_table));
            completer->set_macros_table(settings.GetTokens());
            completer->set_types_table(settings.GetTypes());
            cc_initialised_successfully = true;
        }
    }
    return cc_initialised_successfully;
}

unordered_map<wxString, TagEntryPtr> load_tags_from_file(const wxString& filename)
{
    wxFileName current_file(__FILE__);
    wxFileName test_file(current_file.GetPath(), filename);
    test_file.AppendDir("src");
    test_file.AppendDir("samples");

    wxString fullpath = test_file.GetFullPath();
    auto tags = CTags::Run(fullpath, wxStandardPaths::Get().GetTempDir(), wxEmptyString, settings.GetCodeliteIndexer());
    unordered_map<wxString, TagEntryPtr> tags_map;
    for(const TagEntry& tag : tags) {
        if(tags_map.count(tag.GetPath()))
            continue;
        tags_map.insert({ tag.GetPath(), new TagEntry(tag) });
    }
    return tags_map;
}
} // namespace

/// Helper class for injecting tags loaded from sample files
/// in the completer object
struct SampleFileLoaderLocker {
    wxString filepath;
    SampleFileLoaderLocker(const wxString& filename)
    {
        auto db = load_tags_from_file(filename);
        if(!db.empty()) {
            filepath = db.begin()->second->GetFile();
        }
        completer->test_set_db(db);
    }

    ~SampleFileLoaderLocker() { completer->test_clear_db(); }
};

TEST_FUNC(TestLSPLocation)
{
    ENSURE_DB_LOADED();
    vector<TagEntryPtr> candidates;

    wxString code = "LSP::SymbolInformation si; si.GetLocation().";
    completer->set_text(code, wxEmptyString, wxNOT_FOUND);
    TagEntryPtr resolved = completer->code_complete("si.GetLocation().", { "LSP" });
    if(resolved) {
        completer->get_completions(resolved, wxEmptyString, wxEmptyString, candidates, { "LSP" });
    }
    CHECK_BOOL(resolved);
    CHECK_SIZE(candidates.size(), 12);
    return true;
}

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
    for(const wxStringMap_t::value_type& vt : M) {
        const wxString& raw_string = vt.first;
        const wxString& expected = vt.second;
        CHECK_STRING(helper.get_expression(raw_string, false), expected);
    }
    return true;
}

TEST_FUNC(TestSplitArgsNoArgumentName)
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
        CHECK_SIZE(args.size(), 2);
    }
    {
        auto args = helper.split_function_signature(
            signature_2, &return_value, CompletionHelper::STRIP_NO_DEFAULT_VALUES | CompletionHelper::STRIP_NO_NAME);
        CHECK_SIZE(args.size(), 3);
    }

    {
        auto args = helper.split_function_signature(
            signature_3, &return_value, CompletionHelper::STRIP_NO_DEFAULT_VALUES | CompletionHelper::STRIP_NO_NAME);
        CHECK_SIZE(args.size(), 3);
    }
    return true;
}

TEST_FUNC(TestCompletionHelper_truncate_file_to_location)
{
    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content, 4, 6, CompletionHelper::TRUNCATE_EXACT_POS);
    auto lines = ::wxStringTokenize(f, "\n", wxTOKEN_RET_EMPTY_ALL);
    CHECK_BOOL(!lines.empty());
    CHECK_SIZE(lines.size(), 5);
    CHECK_STRING(lines[4].Trim().Trim(false), "V.");
    return true;
}

TEST_FUNC(TestCompletionHelper_truncate_file_to_location_invalid_input)
{
    wxString file_content_empty = "";

    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content_empty, 4, 6, CompletionHelper::TRUNCATE_EXACT_POS);
    auto lines = ::wxStringTokenize(f, "\n", wxTOKEN_RET_EMPTY_ALL);
    CHECK_BOOL(lines.empty());
    CHECK_SIZE(lines.size(), 0);
    return true;
}

TEST_FUNC(TestCompletionHelper_truncate_file_to_location_must_end_with_words)
{
    wxString file_content_z = "std::vector<TagEntryPtr>";

    CompletionHelper helper;
    wxString f = helper.truncate_file_to_location(file_content_z, 0, 15, CompletionHelper::TRUNCATE_COMPLETE_WORDS);
    CHECK_STRING(f, "std::vector<TagEntryPtr");
    return true;
}

TEST_FUNC(test_cxx_code_complete_member_of_parent_class)
{
    ENSURE_DB_LOADED();
    wxString member_of_parent_class_file;
    {
        SampleFileLoaderLocker loader("member_of_parent_class.hpp");
        member_of_parent_class_file = loader.filepath;
    }

    // set our scope at MyPlugin::foo
    completer->set_text(wxEmptyString, member_of_parent_class_file, 8);
    TagEntryPtr resolved = completer->code_complete("m_mgr->", {});
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetPath(), "IManager");
    return true;
}

TEST_FUNC(TestCTagsManager_AutoCandidates)
{
    ENSURE_DB_LOADED();

    vector<TagEntryPtr> candidates;
    wxString fulltext = "wxCodeCompletionBoxManager::Get().";

    auto resolved = completer->code_complete(fulltext, {});
    CHECK_BOOL(resolved);

    completer->get_completions(resolved, wxEmptyString, wxEmptyString, candidates, {});
    CHECK_BOOL(!candidates.empty());
    return true;
}

TEST_FUNC(TestSimeplTokenizer_Comments)
{
    SimpleTokenizer::Token token;
    SimpleTokenizer tokenizer(tokenizer_sample_file_0);
    vector<SimpleTokenizer::Token> tokens;
    while(tokenizer.next_comment(&token)) {
        tokens.push_back(token);
    }
    // we are expecting 2 comments
    CHECK_SIZE(tokens.size(), 4);
    return true;
}

TEST_FUNC(TestSimeplTokenizer_2)
{
    SimpleTokenizer::Token token;
    SimpleTokenizer tokenizer(sample_text_language_h);
    vector<SimpleTokenizer::Token> tokens;
    vector<TokenWrapper> tokens_vec;
    while(tokenizer.next(&token)) {
        tokens.push_back(token);
        TokenWrapper t;
        t.token = token;
        tokens_vec.push_back(t);
    }

    // collect all interesting tokens from the document
    vector<int> encoded_arr;
    LSPUtils::encode_semantic_tokens(tokens_vec, &encoded_arr);
    for(size_t i = 0; i < encoded_arr.size() / 5; ++i) {
        const auto& tok = tokens_vec[i].token;
        wxUnusedVar(tok);
    }
    return true;
}

TEST_FUNC(TestSimeplTokenizer)
{
    {
        SimpleTokenizer::Token token;
        SimpleTokenizer tokenizer(tokenizer_sample_file_1);
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
    }

    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_2, &return_value);
        CHECK_SIZE(args.size(), 2);
        CHECK_STRING(return_value, "int");
    }
    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_3, &return_value);
        CHECK_SIZE(args.size(), 3);
        CHECK_BOOL(return_value.empty());
    }
    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_4, &return_value);
        CHECK_SIZE(args.size(), 4);
        CHECK_STRING(return_value, "std::vector<int>::iterator*");
    }

    {
        return_value.clear();
        auto args = helper.split_function_signature(signature_5, &return_value);
        CHECK_SIZE(args.size(), 2);
        CHECK_STRING(return_value, "wxString::size_type");
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

TEST_FUNC(test_cxx_expression)
{
    CxxRemainder remainder;
    vector<CxxExpression> exp1 = CxxExpression::from_expression("std::vector<int>::", &remainder);
    CHECK_SIZE(exp1.size(), 2);
    CHECK_EXPECTED(exp1[0].type_name(), "std");
    CHECK_EXPECTED(exp1[0].operand_string(), "::");

    CHECK_EXPECTED(exp1[1].type_name(), "vector");
    CHECK_EXPECTED(exp1[1].operand_string(), "::");
    CHECK_EXPECTED(exp1[1].template_init_list().size(), 1);

    vector<CxxExpression> exp2 = CxxExpression::from_expression("string.AfterFirst('(').", &remainder);
    CHECK_SIZE(exp2.size(), 2);

    vector<CxxExpression> exp3 = CxxExpression::from_expression("string.AfterFirst('(')", &remainder);
    CHECK_SIZE(exp3.size(), 1);
    CHECK_EXPECTED(remainder.filter, "AfterFirst");
    return true;
}

TEST_FUNC(test_cxx_code_completion_both_subscript_and_arrow_operator)
{
    ENSURE_DB_LOADED();
    {
        completer->set_text(cc_subscript_operator_2, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("my_json[1][2].", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "JSONItem");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_anonymous_namespace)
{
    wxString filepath = R"(C:\src\codelite\ctagsd\tests\main.cpp)";
    ENSURE_DB_LOADED();
    {
        if(wxFileExists(filepath)) {
            completer->set_text(wxEmptyString, filepath, wxNOT_FOUND);
            TagEntryPtr resolved = completer->code_complete("completer->", { "std" });
            CHECK_BOOL(resolved);
            CHECK_STRING(resolved->GetPath(), "CxxCodeCompletion");
        }
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_overloaded_function)
{
    ENSURE_DB_LOADED();
    TagEntryPtr resolved = completer->code_complete("wxString::Append", { "std" });
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetPath(), "wxString");

    // vector<TagEntryPtr> tags;
    // completer->get_similar_tags(resolved, tags);
    // CHECK_SIZE(tags.size(), 10);
    return true;
}

TEST_FUNC(test_cxx_code_completion_func_returning_unique_ptr_into_auto)
{
    ENSURE_DB_LOADED();
    {
        SampleFileLoaderLocker loader("sample_function_returning_unique_ptr.hpp");

        wxString code = "auto s = return_unique_ptr_of_string();";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        CxxRemainder remainder;
        TagEntryPtr resolved = completer->code_complete("s->", { "std" }, &remainder);
        CHECK_NOT_NULL(resolved);
        vector<TagEntryPtr> tags;
        completer->get_completions(resolved, remainder.operand_string, remainder.filter, tags, { "std" });
        CHECK_BOOL(is_tag_exists("std::basic_string::append", tags));
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_init_from_ctor)
{
    ENSURE_DB_LOADED();
    {
        wxString code = "JSONItem item = new JSONItem();";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("item->", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "JSONItem");
    }

    {
        wxString code = "auto item = JSONItem();";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("item[0].", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "JSONItem");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_inheritance_children)
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    TagEntryPtr resolved = completer->code_complete("EventNotifier::Get()->", {}, &remainder);
    CHECK_NOT_NULL(resolved);
    vector<TagEntryPtr> candidates;
    completer->get_completions(resolved, wxEmptyString, wxEmptyString, candidates, {});
    CHECK_BOOL(!candidates.empty());
    CHECK_BOOL(is_tag_exists("wxEvtHandler::Bind", candidates));
    return true;
}

TEST_FUNC(test_cxx_code_completion_arrow_operator)
{
    ENSURE_DB_LOADED();
    {
        TagEntryPtr resolved = completer->code_complete("SmartPtr<wxString>->", {}, nullptr);
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    {
        TagEntryPtr resolved = completer->code_complete("SmartPtr<TagEntryPtr>->", {}, nullptr);
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "SmartPtr");
    }
    {
        TagEntryPtr resolved = completer->code_complete("SmartPtr<TagEntryPtr>.Get()->", {}, nullptr);
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "TagEntry");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_subscript_operator)
{
    ENSURE_DB_LOADED();
    {
        wxString code = "vector<TagEntry> tags;";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("tags[0].", { "std" }, nullptr);
        CHECK_NOT_NULL(resolved);
        CHECK_STRING(resolved->GetPath(), "TagEntry");
    }

    {
        wxString code = "vector<vector<wxString>> two_dim_array;";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("two_dim_array[0].", { "std" }, nullptr);
        CHECK_NOT_NULL(resolved);
        CHECK_STRING(resolved->GetPath(), "std::vector");
    }
    {
        wxString code = "vector<vector<wxString>> two_dim_array;";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("two_dim_array[0][0].", { "std" }, nullptr);
        CHECK_NOT_NULL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        wxString code = "vector<vector<wxString>> two_dim_array;";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("two_dim_array[0][0][0].", { "std" }, nullptr);
        CHECK_NOT_NULL(resolved);
        CHECK_STRING_ONE_OF(resolved->GetPath(), "wxUniChar", "wxUniCharRef");
    }
    {
        CxxRemainder remainder;
        auto resolved = completer->code_complete("unordered_map<int, wxString>[0].Tri", { "std" }, &remainder);
        CHECK_NOT_NULL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");

        vector<TagEntryPtr> candidates;
        completer->get_completions(resolved, remainder.operand_string, remainder.filter, candidates, { "std" });
        CHECK_BOOL(!candidates.empty());
        CHECK_BOOL(is_tag_exists("wxString::Trim", candidates));
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_lsp_location_locals)
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    completer->set_text(cc_lsp_location, wxEmptyString, wxNOT_FOUND);
    vector<TagEntryPtr> locals = completer->get_locals(wxEmptyString);
    CHECK_BOOL(!locals.empty());
    CHECK_BOOL(locals.at(0)->GetScope() == "LSP::Location");
    CHECK_BOOL(locals.at(0)->GetName() == "loc");
    return true;
}

TEST_FUNC(test_cxx_code_completion_variable_scanner_ranged_for)
{
    CxxVariableScanner scanner(cc_text_ranged_for, eCxxStandard::kCxx11, {}, false);
    auto vars = scanner.GetVariables();
    CHECK_BOOL(vars.size() == 2);
    CHECK_STRING(vars[0]->GetName(), "array");
    CHECK_STRING(vars[1]->GetName(), "s");
    return true;
}

TEST_FUNC(test_cxx_code_completion_variable_scanner_normal_for)
{
    CxxVariableScanner scanner(cc_text_normal_for, eCxxStandard::kCxx11, {}, false);
    auto vars = scanner.GetVariables();
    CHECK_BOOL(vars.size() == 2);
    CHECK_STRING(vars[0]->GetName(), "array");
    CHECK_STRING(vars[1]->GetName(), "iter");
    return true;
}

TEST_FUNC(test_cxx_code_completion_word_completion)
{
    ENSURE_DB_LOADED();
    {
        CxxRemainder remainder;
        TagEntryPtr resolved = completer->code_complete("wxStr", {}, &remainder);
        CHECK_STRING(remainder.filter, "wxStr");
        vector<TagEntryPtr> candidates;
        completer->get_word_completions(remainder, candidates, { "std" }, {});
        CHECK_BOOL(!candidates.empty());
    }
    {
        CxxRemainder remainder;
        wxString filename = R"(C:\src\codelite\CodeLite\ctags_manager.cpp)";
        completer->set_text(wxEmptyString, filename, 263);
        TagEntryPtr resolved = completer->code_complete("wxStr", {}, &remainder);
        CHECK_STRING(remainder.filter, "wxStr");
        vector<TagEntryPtr> candidates;
        completer->get_word_completions(remainder, candidates, { "std" }, {});
        CHECK_BOOL(!candidates.empty());
        CHECK_BOOL(is_tag_exists("wxString", candidates));
    }
    {
        CxxRemainder remainder;
        TagEntryPtr resolved = completer->code_complete("std::str", {}, &remainder);
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "std");
        CHECK_STRING(remainder.filter, "str");
        vector<TagEntryPtr> candidates;
        completer->get_completions(resolved, remainder.operand_string, remainder.filter, candidates, {});
        CHECK_BOOL(!candidates.empty());

        // check that we found "std::string"
        bool exists = is_tag_exists("std::string", candidates);
        CHECK_BOOL(exists);
    }
    return true;
}

TEST_FUNC(test_cxx_word_completion_inside_scope)
{
    ENSURE_DB_LOADED();
    wxString filename = R"(C:\src\codelite\CodeLite\JSON.cpp)";
    wxString filename_2 = R"(C:\src\codelite\LiteEditor\context_cpp.cpp)";

    if(wxFileExists(filename)) {
        vector<TagEntryPtr> candidates;
        completer->set_text(wxEmptyString, filename, 191);
        CxxRemainder remainder;
        remainder.filter = "addProp";
        size_t count = completer->get_word_completions(remainder, candidates, {}, {});
        CHECK_BOOL(is_tag_exists("JSONItem::addProperty", candidates));
        CHECK_BOOL(count > 0);
    }

    if(wxFileExists(filename_2)) {
        {
            vector<TagEntryPtr> candidates;
            completer->set_text(wxEmptyString, filename_2, 200);
            CxxRemainder remainder;
            remainder.filter = "GetCt";
            size_t count = completer->get_word_completions(remainder, candidates, {}, {});
            CHECK_BOOL(count > 0);
            CHECK_BOOL(is_tag_exists("ContextBase::GetCtrl", candidates));
        }

        {
            completer->set_text(wxEmptyString, filename_2, 200);
            auto resolved = completer->code_complete("GetCtrl().", {}, nullptr);
            CHECK_NOT_NULL(resolved);
            CHECK_STRING(resolved->GetPath(), "clEditor");
        }
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_macros)
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    vector<TagEntryPtr> candidates;
    completer->word_complete(wxEmptyString, wxNOT_FOUND, "CHECK_STRING_ONE", wxEmptyString, {}, false, candidates);
    CHECK_SIZE(candidates.size(), 1);
    CHECK_STRING(candidates[0]->GetName(), "CHECK_STRING_ONE_OF");
    return true;
}

TEST_FUNC(test_cxx_code_completion_class_enum)
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    vector<TagEntryPtr> candidates;
    TagEntryPtr resolved = completer->code_complete("eCxxStandard::", {});
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetPath(), "eCxxStandard");
    CHECK_STRING(resolved->GetKind(), "cenum");

    // find the enumerators
    vector<TagEntryPtr> tags;
    completer->get_completions(resolved, wxEmptyString, wxEmptyString, tags, {});
    CHECK_SIZE(tags.size(), 2);
    return true;
}

TEST_FUNC(test_cxx_code_completion_this_and_global_scope)
{
    ENSURE_DB_LOADED();
    // use a line inside CxxCodeCompletion file
    wxString filename = R"(C:\src\codelite\CodeLite\ctags_manager.cpp)";
    wxString filename_2 = R"(C:\src\codelite\CodeLite\CxxCodeCompletion.cpp)";
    TagEntryPtr resolved;
    if(wxFileExists(filename)) {
        completer->set_text(wxEmptyString, filename, 149);
        resolved = completer->code_complete("this->", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "TagsManager");
    }

    if(wxFileExists(filename_2)) {
        completer->set_text(wxEmptyString, filename_2, 60);
        resolved = completer->code_complete("m_template_manager->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "TemplateManager");
    }

    resolved = completer->code_complete("::", {});
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetPath(), "<global>");

    CxxRemainder remainder;
    resolved = completer->code_complete("wxS", {}, &remainder);
    CHECK_STRING(remainder.filter, "wxS");
    return true;
}

TEST_FUNC(test_cxx_code_completion_vector_of_pairs)
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;

    completer->set_text(wxEmptyString, wxEmptyString, wxNOT_FOUND);
    auto resolved = completer->code_complete("vector<pair<wxString, wxString>>.at(0).", { "std" }, &remainder);
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetPath(), "std::pair");
    return true;
}

TEST_FUNC(test_cxx_code_completion_pointer_type_in_template)
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;

    wxString code = "vector<ITest*> m_tests;";
    completer->set_text(code, wxEmptyString, wxNOT_FOUND);
    auto resolved = completer->code_complete("m_tests.at(i)->tes", { "std" }, &remainder);
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetPath(), "ITest");
    CHECK_STRING(remainder.filter, "tes");
    return true;
}

TEST_FUNC(test_cxx_code_completion_invalid_completions)
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    vector<TagEntryPtr> candidates;
    completer->word_complete(wxEmptyString, wxNOT_FOUND, "does_not_exist->tes", wxEmptyString, { "std" }, false,
                             candidates);
    CHECK_SIZE(candidates.size(), 0);
    return true;
}

TEST_FUNC(test_cxx_code_completion_word_complete_cenum)
{
    ENSURE_DB_LOADED();
    CxxRemainder remainder;
    vector<TagEntryPtr> candidates;
    completer->word_complete(wxEmptyString, wxNOT_FOUND, "eAsc", wxEmptyString, {}, false, candidates);
    CHECK_BOOL(!candidates.empty());
    return true;
}

TEST_FUNC(test_cxx_code_completion_global_method)
{
    ENSURE_DB_LOADED();
    {
        auto resolved = completer->code_complete("clGetManager()->", {}, nullptr);
        CHECK_NOT_NULL(resolved);
        CHECK_STRING(resolved->GetPath(), "IManager");
    }
    {
        // try again, this time with the global namespace prefix
        auto resolved = completer->code_complete("::clGetManager()->", {}, nullptr);
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "IManager");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_member_variable_in_scope)
{
    ENSURE_DB_LOADED();
    // use a line inside CxxCodeCompletion file
    wxString filename = R"(C:\src\codelite\CodeLite\ctags_manager.cpp)";
    if(!wxFileExists(filename)) {
        return true;
    }
    {
        completer->set_text(wxEmptyString, filename, 210); // inside TagsManager::TreeFromTags
        auto resolved = completer->code_complete("m_watch.", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxStopWatch");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_list_locals)
{
    ENSURE_DB_LOADED();
    {
        completer->set_text(tokenizer_sample_file_0, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_SIZE(locals.size(), 0);
    }
    {
        // test function args from a const method
        completer->set_text(cc_text_func_const, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_SIZE(locals.size(), 2);
    }

    {
        completer->set_text(tokenizer_sample_file_2, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_SIZE(locals.size(), 0);
    }

    {
        completer->set_text(file_content, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_SIZE(locals.size(), 1);
    }

    {
        completer->set_text(cc_text_auto_chained, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_SIZE(locals.size(), 2);
        CHECK_STRING(locals[0]->GetName(), "str");
        CHECK_STRING(locals[1]->GetName(), "arr");
    }

    {
        completer->set_text(big_file, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        // we expect 4 variables
        CHECK_SIZE(locals.size(), 4);
    }

    {
        completer->set_text(cc_lamda_text, wxEmptyString, wxNOT_FOUND);
        auto locals = completer->get_locals(wxEmptyString);
        CHECK_SIZE(locals.size(), 6);
    }

    return true;
}

TEST_FUNC(test_cxx_code_completion_function_arguments)
{
    ENSURE_DB_LOADED();
    {
        completer->set_text(cc_text_function_args_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    {
        completer->set_text(cc_text_function_args_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("json.", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "std::shared_ptr");
    }

    {
        completer->set_text(cc_text_function_args_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("json->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "JSON");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_rvalue_reference)
{
    ENSURE_DB_LOADED();
    {
        wxString text = "TagEntry&& entry;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("entry.m_path.", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_member_variable)
{
    ENSURE_DB_LOADED();
    {
        wxString text = "TagEntry entry;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("entry.m_path.", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_template_function)
{
    ENSURE_DB_LOADED();
    SampleFileLoaderLocker loader("template_function.hpp");
    {
        TagEntryPtr resolved = completer->code_complete("get_as_type<wxString>()->", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        wxString code = "auto resolved = get_as_type<wxString>();";
        completer->set_text(code, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("resolved.", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    return true;
}

TEST_FUNC(test_cxx_code_completion_template_std_set)
{
    ENSURE_DB_LOADED();
    {
        wxString text = "set<wxString> S;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("S.begin()->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        TagEntryPtr resolved = completer->code_complete("wxStringSet_t::value_type::", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_template_std_multiset)
{
    ENSURE_DB_LOADED();
    {
        wxString text = "multiset<wxString> S;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("S.begin()->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        TagEntryPtr resolved = completer->code_complete("multiset<wxString>::value_type::", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_template_std_unordered_multiset)
{
    ENSURE_DB_LOADED();
    {
        wxString text = "unordered_multiset<wxString> S;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("S.begin()->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        TagEntryPtr resolved = completer->code_complete("unordered_multiset<wxString>::value_type::", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_template_std_map)
{
    ENSURE_DB_LOADED();
    {
        wxString text = "map<wxString> M;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("M.begin()->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "std::pair");
    }

    {
        TagEntryPtr resolved = completer->code_complete("wxStringMap_t::value_type.", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "std::pair");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion_typedef_using)
{
    ENSURE_DB_LOADED();
    wxString text = "ClangFormatMap compiledMap;";
    completer->set_text(text, wxEmptyString, wxNOT_FOUND);
    TagEntryPtr resolved = completer->code_complete("ClangFormatMap.begin()->first.", { "FormatOptions" });
    CHECK_BOOL(resolved);
    CHECK_STRING(resolved->GetPath(), "wxString");
    return true;
}

TEST_FUNC(test_cxx_code_completion_template)
{
    ENSURE_DB_LOADED();

    if(false) {
        wxString text = "cJSON* json;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("json->", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "cJSON");
        vector<TagEntryPtr> tags;
        completer->get_completions(resolved, wxEmptyString, wxEmptyString, tags, {});
        CHECK_BOOL(is_tag_exists("valuestring", tags));
    }

    {
        wxString text = "wxVector<wxString> V;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("V.at(0).", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    {
        // template inheritance in typedef
        // typedef Singleton<Manager> ManagerST;
        completer->reset();
        TagEntryPtr resolved = completer->code_complete("ManagerST::Get()->", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "Manager");
    }

    {
        // template inheritance
        // class ContextManager : public Singleton<ContextManager> {...}
        // ContextManager::Get()->
        completer->reset();
        TagEntryPtr resolved = completer->code_complete("ContextManager::Get()->", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "ContextManager");
    }

    {
        wxString text = "map<wxString, wxArrayString> M;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("M.at(str).", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxArrayString");
    }

    {
        wxString text = "shared_ptr<wxString> P;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("P.", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "std::shared_ptr");
    }

    {
        wxString text = "shared_ptr<wxString> P;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("P->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    {
        wxString text = "unique_ptr<wxString> P;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("P->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }
    {
        completer->set_text(cc_text_ProtocolHandler, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("msg->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "JSON");
    }
    {
        completer->set_text("CxxVariable::Map_t varsMap;", wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("varsMap.begin()->first.", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text("CxxVariable::Map_t varsMap;", wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("varsMap.begin()->second->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "CxxVariable");
    }

    {
        completer->set_text("CxxVariable::Map_t varsMap;", wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("varsMap.begin()->second.", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "SmartPtr");
    }

    {
        completer->set_text("unordered_set<CxxVariable> varsSet;", wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("varsSet.begin()->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "CxxVariable");
    }

    {
        TagEntryPtr resolved =
            completer->code_complete("map<wxString, CxxVariable::Ptr_t>::begin()->first.", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        TagEntryPtr resolved =
            completer->code_complete("map<wxString, CxxVariable::Ptr_t>::begin()->second.", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "SmartPtr");
    }
    {
        TagEntryPtr resolved =
            completer->code_complete("map<wxString, CxxVariable::Ptr_t>::begin()->second->", { "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "CxxVariable");
    }
    return true;
}

TEST_FUNC(test_cxx_code_completion)
{
    ENSURE_DB_LOADED();

    {
        wxString text = "std::string str;";
        completer->set_text(text, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "std::basic_string");
    }

    {
        completer->set_text(cc_text_auto_chained, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_auto_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("wxString::", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.AfterFirst('(').", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_simple, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("str.AfterFirst('(').BeforeFirst().", {});
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "wxString");
    }

    {
        completer->set_text(cc_text_lsp_event, wxEmptyString, wxNOT_FOUND);
        TagEntryPtr resolved = completer->code_complete("event.GetLocation().GetRange().GetStart().", { "LSP", "std" });
        CHECK_BOOL(resolved);
        CHECK_STRING(resolved->GetPath(), "LSP::Position");
    }
    return true;
}

int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    wxLogNull NOLOG;
    Tester::Instance()->RunTests();
    return 0;
}
