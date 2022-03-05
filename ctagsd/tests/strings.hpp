#include <wx/string.h>
const wxString big_file = R"(
#include "CxxScannerTokens.h"
#include "CxxVariableScanner.h"
#include "file_logger.h"

#include <algorithm>
#include <unordered_set>

CxxVariableScanner::CxxVariableScanner(const wxString& buffer, eCxxStandard standard, const wxStringTable_t& macros,
                                       bool isFuncSignature)
    : m_scanner(NULL)
    , m_buffer(buffer)
    , m_eof(false)
    , m_parenthesisDepth(0)
    , m_standard(standard)
    , m_macros(macros)
    , m_isFuncSignature(isFuncSignature)
{
    if(!m_buffer.IsEmpty() && m_buffer[0] == '(') {
        m_buffer.Remove(0, 1);
    }
    m_nativeTypes.insert(T_AUTO);
    m_nativeTypes.insert(T_BOOL);
    m_nativeTypes.insert(T_CHAR);
    m_nativeTypes.insert(T_CHAR16_T);
    m_nativeTypes.insert(T_CHAR32_T);
    m_nativeTypes.insert(T_DOUBLE);
    m_nativeTypes.insert(T_FLOAT);
    m_nativeTypes.insert(T_INT);
    m_nativeTypes.insert(T_LONG);
    m_nativeTypes.insert(T_SHORT);
    m_nativeTypes.insert(T_SIGNED);
    m_nativeTypes.insert(T_UNSIGNED);
    m_nativeTypes.insert(T_VOID);
    m_nativeTypes.insert(T_WCHAR_T);
}

CxxVariableScanner::~CxxVariableScanner() {}

CxxVariable::Vec_t CxxVariableScanner::GetVariables(bool sort)
{
    wxString strippedBuffer;
    OptimizeBuffer(m_buffer, strippedBuffer);
    CxxVariable::Vec_t vars = DoGetVariables(strippedBuffer, sort);
    if(sort) {
        std::sort(vars.begin(), vars.end(),
                  [&](CxxVariable::Ptr_t a, CxxVariable::Ptr_t b) { return a->GetName() < b->GetName(); });
    }
    return vars;
}

bool CxxVariableScanner::ReadType(CxxVariable::LexerToken::Vec_t& vartype, bool& isAuto)
{
    isAuto = false;
    int depth = 0;
    CxxLexerToken token;
    while(GetNextToken(token)) {
        if(depth == 0) {
            if(vartype.empty()) {
                // a type can start the following tokens
                switch(token.GetType()) {
                case T_AUTO:
                    isAuto = true;
                // fall
                case T_CLASS:
                case T_STRUCT:
                case T_IDENTIFIER:
                case T_DOUBLE_COLONS:
                case T_BOOL:
                case T_CHAR:
                case T_CHAR16_T:
                case T_CHAR32_T:
                case T_CONST:
                case T_CONSTEXPR:
                case T_DOUBLE:
                case T_FLOAT:
                case T_INT:
                case T_LONG:
                case T_MUTABLE:
                case T_REGISTER:
                case T_SHORT:
                case T_SIGNED:
                case T_STATIC:
                case T_UNSIGNED:
                case T_VOLATILE:
                case T_VOID:
                case T_USING:
                case T_WCHAR_T: {
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    break;
                }
                default:
                    // Not a type definition
                    return false;
                }
            } else {
                const CxxVariable::LexerToken& lastToken = vartype.back();
                switch(token.GetType()) {
                case T_IDENTIFIER: {
                    if(TypeHasIdentifier(vartype) && (vartype.back().type != T_DOUBLE_COLONS)) {
                        // We already found the identifier for this type, its probably part of the name
                        UngetToken(token);
                        return true;
                    } else if(HasNativeTypeInList(vartype) && (vartype.back().type != T_DOUBLE_COLONS)) {
                        UngetToken(token);
                        return true;
                    }
                    // Found an identifier
                    // We consider this part of the type only if the previous token was "::" or
                    // if it was T_CONST
                    switch(lastToken.type) {
                    case T_CLASS:
                    case T_STRUCT:
                    case T_DOUBLE_COLONS:
                    case T_CONST:
                    case T_CONSTEXPR:
                    case T_REGISTER:
                    case T_MUTABLE:
                    case T_VOLATILE:
                    case T_STATIC:
                        vartype.push_back(CxxVariable::LexerToken(token, depth));
                        break;
                    default:
                        UngetToken(token);
                        return true;
                    }
                    break;
                case T_AUTO:
                    isAuto = true;
                // fall
                case T_DOUBLE_COLONS:
                case T_BOOL:
                case T_CHAR:
                case T_CHAR16_T:
                case T_CHAR32_T:
                case T_CONST:
                case T_CONSTEXPR:
                case T_DOUBLE:
                case T_FLOAT:
                case T_INT:
                case T_LONG:
                case T_SHORT:
                case T_SIGNED:
                case T_UNSIGNED:
                case T_VOID:
                case T_WCHAR_T: {
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    break;
                }
                }
                case '<':
                case '[':
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    depth++;
                    break;
                case '*':
                case '&':
                case '@':       // AngelScript
                case T_AND_AND: // C++11 reference rvalue
                    // Part of the name
                    UngetToken(token);
                    return true;
                default:
                    return false;
                }
            }
        } else {
            // Depth > 0
            vartype.push_back(CxxVariable::LexerToken(token, depth));
            if(token.GetType() == '>' || token.GetType() == ']') {
                --depth;
            } else if(token.GetType() == '<' || token.GetType() == '[') {
                ++depth;
            }
        }
    }
    return false;
}

thread_local std::unordered_set<int> s_validLocalTerminators;
bool CxxVariableScanner::ReadName(wxString& varname, wxString& pointerOrRef, wxString& varInitialization)
{
    CxxLexerToken token;
    while(GetNextToken(token)) {
        if(token.GetType() == '@') {
            // AngelScript. @ is similar to * in C/C++
            // @see https://github.com/eranif/codelite/issues/1839
            if(!GetNextToken(token) || token.GetType() != T_IDENTIFIER) {
                varname.Clear();
                return false;
            }
            varname = token.GetWXString();
            varInitialization.Clear();
            pointerOrRef = "@";
            return true;
        } else if(token.GetType() == T_IDENTIFIER) {
            varname = token.GetWXString();

            // When parsing function signature, we don't have multiple arguments
            // tied to the same TYPE
            if(m_isFuncSignature) {
                return false;
            }

            // Peek at the next token
            // We can expect "=", "," "(", ";" or
            // Examples:
            // TYPE name = 1;
            // TYPE name, secondVariable;
            // TYPE name(10);
            // TYPE name;
            if(s_validLocalTerminators.empty()) {
                s_validLocalTerminators.insert((int)',');
                s_validLocalTerminators.insert((int)'=');
                s_validLocalTerminators.insert((int)';');
                s_validLocalTerminators.insert((int)')');
                s_validLocalTerminators.insert((int)'(');
                s_validLocalTerminators.insert((int)'{'); // C++11 initialization, e.g: vector<int> v {1,2,3};
                s_validLocalTerminators.insert((int)'['); // Array
            }

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) {
                // We reached EOF, but we do got the variable name
                // se we return false ("don't continue") but we don't clear the name
                return false;
            }

            // Always return the token
            UngetToken(token);

            if(s_validLocalTerminators.count(token.GetType()) == 0) {
                varname.Clear();
                return false;
            }

            ConsumeInitialization(varInitialization);

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) {
                return false;
            }

            if((token.GetType() == '{') && !varInitialization.IsEmpty()) {
                // Don't collect functions and consider them as variables
                UngetToken(token);
                varname.clear();
                return false;
            }

            if(!varInitialization.empty()) {
                varInitialization.RemoveLast();
            }

            // If we found comma, return true. Unless we are parsing a function signature
            if(!m_isFuncSignature && (token.GetType() == ',')) {
                return true;
            } else {
                UngetToken(token);
                return false;
            }
        } else if(token.GetType() == '*' || token.GetType() == '&' || token.GetType() == T_AND_AND) {
            pointerOrRef << token.GetWXString();
        } else {
            return false;
        }
    }
    return false;
}

void CxxVariableScanner::ConsumeInitialization(wxString& consumed)
{
    CxxLexerToken token;
    wxString dummy;
    if(!GetNextToken(token))
        return;
    int type = wxNOT_FOUND;
    int tokType = token.GetType();
    if(tokType == '(') {
        // Read the initialization
        std::unordered_set<int> delims;
        delims.insert(')');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        consumed.Prepend("(");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '[') {
        // Array
        std::unordered_set<int> delims;
        delims.insert(']');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        consumed.Prepend("[");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '{') {
        // Read the initialization
        std::unordered_set<int> delims;
        delims.insert('}');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        consumed.Prepend("{");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '=') {
        std::unordered_set<int> delims;
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, consumed);
    } else {
        UngetToken(token);
        consumed.clear();
        std::unordered_set<int> delims;
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);
    }

    if(type == ',' || type == (int)'{' || type == ';') {
        UngetToken(token);
    }
}

int CxxVariableScanner::ReadUntil(const std::unordered_set<int>& delims, CxxLexerToken& token, wxString& consumed)
{
    // loop until we find the open brace
    CxxVariable::LexerToken::Vec_t v;
    int depth = 0;
    while(GetNextToken(token)) {
        v.push_back(CxxVariable::LexerToken(token, depth));
        if(depth == 0) {
            if(delims.count(token.GetType())) {
                consumed = CxxVariable::PackType(v, m_standard);
                return token.GetType();
            } else {
                switch(token.GetType()) {
                case '<':
                case '{':
                case '[':
                case '(':
                    depth++;
                    break;
                default:
                    // ignore it
                    break;
                }
            }
        } else {
            switch(token.GetType()) {
            case '>':
            case '}':
            case ']':
            case ')':
                depth--;
                break;
            default:
                // ignore it
                break;
            }
        }
    }
    return wxNOT_FOUND;
}

bool CxxVariableScanner::GetNextToken(CxxLexerToken& token)
{
    bool res = false;

    while(true) {
        res = ::LexerNext(m_scanner, token);
        if(!res)
            break;

        // Ignore any T_IDENTIFIER which is declared as macro
        if((token.GetType() == T_IDENTIFIER) && m_macros.count(token.GetWXString())) {
            continue;
        }
        break;
    }

    m_eof = !res;
    switch(token.GetType()) {
    case '(':
        ++m_parenthesisDepth;
        break;
    case ')':
        --m_parenthesisDepth;
        break;
    default:
        break;
    }
    return res;
}

void CxxVariableScanner::OptimizeBuffer(const wxString& buffer, wxString& stripped_buffer)
{
    stripped_buffer.Clear();
    Scanner_t sc = ::LexerNew(buffer);
    if(!sc) {
        clWARNING() << "CxxVariableScanner::OptimizeBuffer(): failed to create Scanner_t" << clEndl;
        return; // Failed to allocate scanner
    }
)";

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

const wxString sample_text_language_h = R"(class TemplateHelper
{
    std::vector<wxArrayString> templateInstantiationVector;
};
)";

const wxString sample_text = R"(
#ifdef __WXMSW__
#endif
m_string.)";

wxString file_content =
    R"(
using namespace std;
void foo() {
    vector<string> V;
    V.empty();
    // other content goes here
    // bla bla
)";

const wxString tokenizer_sample_file_0 = R"(#include <unistd.h>
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

const wxString tokenizer_sample_file_1 = R"(#include <unistd.h>
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

const wxString cc_text_ProtocolHandler = R"(
#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "CxxCodeCompletion.hpp"
#include "CxxPreProcessor.h"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include "CxxUsingNamespaceCollector.h"
#include "CxxVariableScanner.h"
#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "LSPUtils.hpp"
#include "ProtocolHandler.hpp"
#include "Scanner.hpp"
#include "Settings.hpp"
#include "SimpleTokenizer.hpp"
#include "clFilesCollector.h"
#include "cl_calltip.h"
#include "crawler_include.h"
#include "ctags_manager.h"
#include "fc_fileopener.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "tags_options_data.h"
#include "tags_storage_sqlite3.h"

#include <deque>
#include <iostream>
#include <wx/filesys.h>

using LSP::CompletionItem;
using LSP::eSymbolKind;

namespace
{
wxStopWatch sw;
FileLogger& operator<<(FileLogger& logger, const TagEntry& tag)
{
    wxString s;
    s << tag.GetKind() << ", " << tag.GetDisplayName() << ", " << tag.GetPatternClean();
    logger.Append(s, logger.GetRequestedLogLevel());
    return logger;
}

FileLogger& operator<<(FileLogger& logger, const vector<TagEntryPtr>& tags)
{
    for(const auto& tag : tags) {
        logger << (*tag) << endl;
    }
    return logger;
}

wxString MapToString(const wxStringMap_t& m)
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

void start_timer() { sw.Start(); }

wxString stop_timer()
{
    long ms = sw.Time();
    long seconds = ms / 1000;
    ms = seconds % 1000;

    wxString elapsed;
    elapsed << _("Time elapsed: ") << seconds << "." << ms << _(" seconds");
    return elapsed;
}

/**
 * @brief given a list of files, remove all non c/c++ files from it
 */
void filter_non_important_files(wxArrayString& files, const CTagsdSettings& settings)
{
    wxArrayString tmparr;
    tmparr.reserve(files.size());

    // filter non c/c++ files
    wxArrayString ignore_spec = ::wxStringTokenize(settings.GetIgnoreSpec(), ";", wxTOKEN_STRTOK);
    auto should_skip_file = [&](const wxString& file_full_path) -> bool {
        for(const wxString& spec : ignore_spec) {
            if(file_full_path.Contains(spec)) {
                return true;
            }
        }
        return false;
    };

    for(wxString& file : files) {

        file.Trim().Trim(false);
        wxFileName fn(file);
        wxString fullpath = fn.GetFullPath(wxPATH_UNIX);
        if(should_skip_file(fullpath)) {
            continue;
        }

        if(FileExtManager::IsCxxFile(file)) {
            tmparr.Add(file);
        }
    }
    tmparr.swap(files);
}

/**
 * @brief recurse into `dir` collect all files that matches the `settings.GetFileMask`
 * @param dir root folder to scan
 * @param settings `settings.json` object
 * @param files [output]
 */
void scan_dir(const wxString& dir, const CTagsdSettings& settings, wxArrayString& files)
{
    clDEBUG() << "Searching for files to..." << endl;

    clFilesScanner scanner;
    wxArrayString exclude_folders_arr = ::wxStringTokenize(settings.GetIgnoreSpec(), ";", wxTOKEN_STRTOK);
    scanner.Scan(dir, files, settings.GetFileMask(), wxEmptyString, settings.GetIgnoreSpec());
    filter_non_important_files(files, settings);
}
} // namespace

ProtocolHandler::ProtocolHandler() {}

ProtocolHandler::~ProtocolHandler() {}

void ProtocolHandler::send_log_message(const wxString& message, int level, Channel& channel)
{
    JSON root(cJSON_Object);
    JSONItem notification = root.toElement();
    notification.addProperty("method", "window/logMessage");
    notification.addProperty("jsonrpc", "2.0");
    auto params = notification.AddObject("params");
    params.addProperty("message", wxString() << "P:" << wxGetProcessId() << ": " << message);
    params.addProperty("type", level);

    channel.write_reply(notification.format(false));
}

JSONItem ProtocolHandler::build_result(JSONItem& reply, size_t id, int result_kind)
{
    reply.addProperty("id", id);
    reply.addProperty("jsonrpc", "2.0");

    auto result = result_kind == cJSON_Array ? reply.AddArray("result") : reply.AddObject("result");
    return result;
}

void ProtocolHandler::parse_files(wxArrayString& files, Channel* channel)
{
    clDEBUG() << "Parsing" << files.size() << "files" << endl;
    clDEBUG() << "Removing un-modified and unwanted files..." << endl;
    // create/open db
    wxFileName dbfile(m_settings_folder, "tags.db");
    if(!dbfile.FileExists()) {
        clDEBUG() << dbfile << "does not exist, will create it" << endl;
    }
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(files, db);
    start_timer();

    clDEBUG() << "There are total of" << files.size() << "files that require parsing" << endl;
    if(channel) {
        send_log_message(wxString() << _("Generating ctags file for: ") << files.size() << _(" files"), LSP_LOG_INFO,
                         *channel);
    }
    clDEBUG() << "Generating ctags file..." << endl;
    if(!CTags::Generate(files, m_settings_folder, m_settings.GetCodeliteIndexer())) {
        if(channel) {
            send_log_message(_("Failed to generate `ctags` file"), LSP_LOG_ERROR, *channel);
        }
        clERROR() << "Failed to generate ctags file!" << endl;
        return;
    }
    clDEBUG() << "Success" << endl;

    if(channel) {
        send_log_message(wxString() << _("Success (") << stop_timer() << ") ", LSP_LOG_INFO, *channel);
    }

    // update the DB
    wxStringSet_t updatedFiles;
    CTags ctags(m_settings_folder);
    if(!ctags.IsOpened()) {
        clWARNING() << "Failed to open ctags file under:" << m_settings_folder << endl;
        return;
    }

    wxString curfile;

    // build the database
    TagTreePtr ttp = ctags.GetTagsTreeForFile(curfile);
    if(channel) {
        send_log_message(_("Updating symbols database..."), LSP_LOG_INFO, *channel);
    }

    start_timer();
    clDEBUG() << "Updating symbols database..." << endl;
    db->Begin();
    size_t tagsCount = 0;
    time_t update_time = time(nullptr);
    while(ttp) {
        ++tagsCount;

        // Send notification to the main window with our progress report
        db->DeleteByFileName({}, curfile, false);
        db->Store(ttp, {}, false);

        if((tagsCount % 1000) == 0) {
            // Commit what we got so far
            db->Commit();
            // Start a new transaction
            db->Begin();
        }
        ttp = ctags.GetTagsTreeForFile(curfile);
    }

    // update the files table in the database
    // we do this here, since some files might not yield tags
    // but we still want to mark them as "parsed"
    for(const wxString& file : files) {
        if(db->InsertFileEntry(file, (int)update_time) == TagExist) {
            db->UpdateFileEntry(file, (int)update_time);
        }
    }

    // Commit whats left
    db->Commit();
    if(channel) {
        send_log_message(wxString() << _("Success (") << stop_timer() << ") ", LSP_LOG_INFO, *channel);
    }
    clDEBUG() << "Success" << endl;
}

vector<wxString> ProtocolHandler::update_additional_scopes_for_file(const wxString& filepath)
{
    // we need to visit each node in the file graph and create a set of all the namespaces
    vector<wxString> additional_scopes;
    if(m_additional_scopes.count(filepath)) {
        additional_scopes = m_additional_scopes[filepath];
    } else {
        wxStringSet_t visited;
        wxStringSet_t ns;
        vector<wxString> Q;
        Q.push_back(filepath);
        while(!Q.empty()) {
            // pop the front of queue
            wxString file = Q.front();
            Q.erase(Q.begin());

            // sanity
            if(m_parsed_files_info.count(file) == 0 // no info for this file
               || visited.count(file))              // already visited this file
            {
                continue;
            }
            visited.insert(file);

            // keep the info and traverse its children
            const ParsedFileInfo& info = m_parsed_files_info[file];
            ns.insert(info.using_namespace.begin(), info.using_namespace.end());

            // append its children
            Q.insert(Q.end(), info.included_files.begin(), info.included_files.end());
        }

        additional_scopes.insert(additional_scopes.end(), ns.begin(), ns.end());
        // cache the result
        m_additional_scopes.insert({ filepath, additional_scopes });
    }
    clDEBUG() << "Setting additional scopes for file:" << filepath << endl;
    clDEBUG() << "Scopes:" << additional_scopes << endl;
    TagsManagerST::Get()->GetLanguage()->UpdateAdditionalScopesCache(filepath, additional_scopes);
    return additional_scopes;
}

bool ProtocolHandler::ensure_file_content_exists(const wxString& filepath, Channel& channel, size_t req_id)
{
    if(m_filesOpened.count(filepath) == 0) {
        // check if this file exists on the file system -> and load it instead of complaining about it
        wxString file_content;
        if(!wxFileExists(filepath) || !FileUtils::ReadFileContent(filepath, file_content)) {
            clWARNING() << "File:" << filepath << "is not opened" << endl;
            send_log_message(wxString() << _("File: `") << filepath << _("` is not opened on the server"),
                             LSP_LOG_WARNING, channel);

            JSON root(cJSON_Object);
            auto response = root.toElement();
            auto result = build_result(response, req_id, cJSON_Object);
            channel.write_reply(response);
            return false;
        }

        // update the cache
        clDEBUG() << "Updated cache with non existing file:" << filepath << "is not opened" << endl;
        m_filesOpened.insert({ filepath, file_content });
    }
    return true;
}

void ProtocolHandler::update_comments_for_file(const wxString& filepath)
{
    wxString file_content;
    if(FileUtils::ReadFileContent(filepath, file_content)) {
        update_comments_for_file(filepath, file_content);
    }
}

void ProtocolHandler::update_comments_for_file(const wxString& filepath, const wxString& file_content)
{
    m_comments_cache.erase(filepath);
    if(file_content.empty()) {
        return;
    }

    SimpleTokenizer tokenizer(file_content);
    SimpleTokenizer::Token token;
    CachedComment::Map_t file_cache;
    while(tokenizer.next_comment(&token)) {
        wxString comment = token.to_string(file_content);
        tokenizer.strip_comment(comment);
        file_cache.insert({ token.line(), comment });
    }
    m_comments_cache.insert({ filepath, file_cache });
}

const wxString& ProtocolHandler::get_comment(const wxString& filepath, long line, const wxString& default_value) const
{
    if(m_comments_cache.count(filepath) == 0) {
        return default_value;
    }
    const auto& M = m_comments_cache.find(filepath)->second;
    // try to find a comment, 1 or 2 lines above the requested line
    for(long curline = line; curline > (line - 3); --curline) {
        if(M.count(curline)) {
            return M.find(curline)->second;
        }
    }
    return default_value;
}

bool ProtocolHandler::do_comments_exist_for_file(const wxString& filepath) const
{
    return m_comments_cache.count(filepath) != 0;
}

size_t ProtocolHandler::read_file_list(wxArrayString& arr) const
{
    arr.clear();
    wxFileName file_list(m_settings_folder, "file_list.txt");
    if(file_list.FileExists()) {
        wxString file_list_content;
        FileUtils::ReadFileContent(file_list, file_list_content);
        wxArrayString files = wxStringTokenize(file_list_content, "\n", wxTOKEN_STRTOK);
        filter_non_important_files(files, m_settings);
        arr.swap(files);
    }
    return arr.size();
}

//---------------------------------------------------------------------------------
// protocol handlers
//---------------------------------------------------------------------------------

// Request <-->
void ProtocolHandler::on_initialize(unique_ptr<JSON>&& msg, Channel& channel)
{
)";

// --------------------------- CxxCodeCompletion text ----------------------------------------

const wxString cc_text_simple = R"(wxString str;)";
const wxString cc_text_lsp_event = R"(LSPEvent& event;)";
const wxString cc_text_auto_simple = R"(auto str = wxString::AfterFirst();)";
const wxString cc_text_auto_chained = R"(
auto arr = wxStringTokenize();
auto str = arr.Item(0);
)";
const wxString cc_text_function_args_simple = R"(
void Foo::Bar(const wxString& str, shared_ptr<JSON>&& json) {
)";
const wxString cc_lamda_text = R"(
void Foo::Bar(const wxString& str, shared_ptr<JSON>&& json) {
        auto callback = [&](const wxString& variable, const std::vector<std::string>& V) {
            CxxPreProcessor pp;
)";
const wxString cc_text_ranged_for = R"(
wxArrayString array;
for(auto& s : array) {
    s.
)";

const wxString cc_text_normal_for = R"(
wxArrayString array;
for(auto iter = array.begin(); iter != array.end(); ++iter) {
)";

const wxString cc_text_func_const = R"(
void Foo::Bar(const wxString& str, shared_ptr<JSON>&& json) const {
)";

const wxString cc_lsp_location = R"(
void LSP::FindReferencesRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);
    JSONItem result = response.Get("result");
    CHECK_EXPECTED_RETURN(result.isOk(), true);

    // An array of locations
    int array_size = result.arraySize();

    LSPEvent references_event{ wxEVT_LSP_REFERENCES };
    std::vector<LSP::Location>& locations = references_event.GetLocations();
    locations.reserve(array_size);

    for(int i = 0; i < array_size; ++i) {
        auto d = result[i];
        LSP::Location loc;
)";

const wxString cc_subscript_operator_2 = R"(
unique_ptr<JSON> pmsg;
auto my_json = pmsg->toElement();
)";

const wxString cc_zero_locals = R"(
CxxVariable::Vec_t CxxVariableScanner::GetVariables(bool sort)
{
    wxString strippedBuffer;
    OptimizeBuffer(m_buffer, strippedBuffer);
    CxxVariable::Vec_t vars = DoGetVariables(strippedBuffer, sort);
    if(sort) {
        std::sort(vars.begin(), vars.end(),
                  [&](CxxVariable::Ptr_t a, CxxVariable::Ptr_t b) { return a->GetName() < b->GetName(); });
    }
    return vars;
}
)";

const wxString cc_one_locals = R"(
CxxVariable::Vec_t CxxVariableScanner::GetVariables(bool sort)
{
    wxString strippedBuffer;
    OptimizeBuffer(m_buffer, strippedBuffer);
    CxxVariable::Vec_t vars = DoGetVariables(strippedBuffer, sort);
    if(sort) {
        std::sort(vars.begin(), vars.end(),
                  [&](CxxVariable::Ptr_t a, CxxVariable::Ptr_t b) { return a->GetName() < b->GetName(); });
    }
    return vars;
}

void CxxVariableScanner::Foo() {
    wxString one_local;
)";

const wxString cc_test_function_calls_parsing = R"(
CxxVariable::Vec_t CxxVariableScanner::GetVariables(bool sort)
{
    wxString strippedBuffer;
    OptimizeBuffer(m_buffer, strippedBuffer);
    CxxVariable::Vec_t vars = DoGetVariables(strippedBuffer, sort);
)";

const wxString cc_locals_with_typedef = R"(
#include <wx/app.h>
#include <wx/event.h>
#include "MainFrame.h"
#include <wx/image.h>
#include <iostream>
#include <thread>
#include <wx/cmdline.h>
#include "wxTerminalOptions.h"
#include <wx/persist.h>
#include "clPersistenceManager.h"

//#ifdef __WXMSW__
// void RedirectIOToConsole()
//{
//    AllocConsole();
//    freopen("CON", "wb", stdout);
//    freopen("CON", "wb", stderr);
//    freopen("CON", "r", stdin); // Note: "r", not "w"
//}
//#endif

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    // Switches
    { wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "w", "wait", "Wait for any key to be pressed before exiting", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    // Options
    { wxCMD_LINE_OPTION, "p", "print-tty", "Print the terminals tty (*nix only) into file", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "t", "title", "Set the console title", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "d", "working-directory", "Set the working directory", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "c", "command", "Command to execute", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "f", "file", "File contains command to execute", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

class MyPersistenceManager : public wxPersistenceManager
{
    wxFileConfig* m_config = nullptr;

public:
    MyPersistenceManager()
    {
        wxFileName localFile(wxStandardPaths::Get().GetUserDataDir(), "persistency.ini");
        m_config = new wxFileConfig("codelite-terminal", "CodeLite", localFile.GetFullPath());
    }

    ~MyPersistenceManager() { wxDELETE(m_config); }
    wxConfigBase* GetConfig() const { return m_config; }
};

// Define the MainApp
class MainApp : public wxApp
{
    MyPersistenceManager* m_persistency = nullptr;

public:
    MainApp() {}
    virtual ~MainApp() { wxDELETE(m_persistency); }

    virtual bool OnInit()
    {
        SetAppName("codelite-terminal");
        wxFileName configDir(wxStandardPaths::Get().GetUserDataDir(), "");
        configDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        m_persistency = new MyPersistenceManager();
        wxPersistenceManager::Set(*m_persistency);
        wxTerminalOptions& m_options = wxTerminalOptions::Get();

        // m_persistencManager = new clPersistenceManager();
        // wxPersistenceManager::Set(*m_persistencManager);
#ifdef __WXMSW__
        typedef BOOL WINAPI (*SetProcessDPIAwareFunc)();
        HINSTANCE user32Dll = LoadLibrary(L"User32.dll");
        if(user32Dll) {
            SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(user32Dll, "SetProcessDPIAware");
            if(pFunc) { pFunc(); }
            FreeLibrary(user32Dll);
        }
#endif
        wxCmdLineParser parser(wxApp::argc, wxApp::argv);
        parser.SetDesc(cmdLineDesc);
        const wxArrayString& argv = wxApp::argv.GetArguments();
)";
