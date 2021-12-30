#include "ProtocolHandler.hpp"

#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "CxxPreProcessor.h"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include "CxxUsingNamespaceCollector.h"
#include "CxxVariableScanner.h"
#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "LSPUtils.hpp"
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
        send_log_message(wxString() << _("Success (") << stop_timer() << ")", LSP_LOG_INFO, *channel);
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
        send_log_message(wxString() << _("Success (") << stop_timer() << ")", LSP_LOG_INFO, *channel);
    }
    clDEBUG() << "Success" << endl;
}

void ProtocolHandler::update_additional_scopes_for_file(const wxString& filepath)
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
    clDEBUG() << "Received `initialize` request" << endl;
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    auto result = build_result(response, id, cJSON_Object);

    auto capabilities = result.AddObject("capabilities");
    capabilities.addProperty("completionProvider", true);
    capabilities.addProperty("declarationProvider", true);
    capabilities.addProperty("definitionProvider", true);
    capabilities.addProperty("documentSymbolProvider", true);
    capabilities.addProperty("hoverProvider", true);
    auto semanticTokensProvider = capabilities.AddObject("semanticTokensProvider");
    auto full = semanticTokensProvider.AddObject("full");
    auto legend = semanticTokensProvider.AddObject("legend");
    full.addProperty("delta", true);

    legend.AddArray("tokenModifiers"); // empty array
    auto tokenTypes = legend.AddArray("tokenTypes");
    // we report our supported token types
    // the order must match the definition in eTokenType
    tokenTypes.arrayAppend("variable"); // TYPE_VARIABLE
    tokenTypes.arrayAppend("class");    // TYPE_CLASS
    tokenTypes.arrayAppend("function"); // TYPE_FUNCTION

    // load the configuration file
    m_root_folder = json["params"]["rootUri"].toString();
    m_root_folder = wxFileSystem::URLToFileName(m_root_folder).GetFullPath();

    // set the working directory to the workspace folder
    ::wxSetWorkingDirectory(m_root_folder);

    // create the settings directory
    wxFileName fn_settings_dir(m_root_folder, wxEmptyString);
    fn_settings_dir.AppendDir(".ctagsd");
    fn_settings_dir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    m_settings_folder = fn_settings_dir.GetPath();

    wxFileName fn_config_file(m_settings_folder, "settings.json");
    m_settings.Load(fn_config_file);

    // export CTAGS_REPLACEMENTS
    wxFileName ctagsReplacements(m_settings_folder, "ctags.replacements");
    wxSetEnv("CTAGS_REPLACEMENTS", ctagsReplacements.GetFullPath());

    m_codelite_indexer.reset(new CodeLiteIndexer());
    m_codelite_indexer->set_exe_path(m_settings.GetCodeliteIndexer());
    m_codelite_indexer->start();
    TagsManagerST::Get()->SetIndexer(m_codelite_indexer);

    // construct TagsOptionsData
    TagsOptionsData tod;
    tod.SetFileSpec(m_settings.GetFileMask());
    tod.SetTokens(MapToString(m_settings.GetTokens()));
    tod.SetTypes(MapToString(m_settings.GetTypes()));
    tod.SetCcNumberOfDisplayItems(m_settings.GetLimitResults());

    TagsManagerST::Get()->SetCtagsOptions(tod);

    // build the workspace file list
    wxArrayString files;
    if(read_file_list(files) == 0) {
        // try the scan dir
        scan_dir(m_root_folder, m_settings, files);
    }
    build_search_path();

    // build a list of files to parse (including all include statements)
    files = get_files_to_parse(files);
    parse_files(files, &channel);

    TagsManagerST::Get()->CloseDatabase();
    TagsManagerST::Get()->OpenDatabase(wxFileName(m_settings_folder, "tags.db"));
    TagsManagerST::Get()->SetCtagsOptions(tod);
    TagsManagerST::Get()->GetDatabase()->SetSingleSearchLimit(tod.GetCcNumberOfDisplayItems());
    channel.write_reply(response.format(false));
}

// Request <-->
void ProtocolHandler::on_unsupported_message(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxString message;
    message << _("unsupported message: `") << msg->toElement()["method"].toString() << "`";
    send_log_message(message, LSP_LOG_WARNING, channel);
}

// Notification -->
void ProtocolHandler::on_initialized(unique_ptr<JSON>&& msg, Channel& channel)
{
    // a notification
    wxUnusedVar(msg);
    wxUnusedVar(channel);
    clSYSTEM() << "Received `initialized` message" << endl;
}

// Notification -->
void ProtocolHandler::on_did_open(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(channel);
    // keep the file content in the internal map
    auto json = msg->toElement();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    clDEBUG() << "textDocument/didOpen: caching new content for file:" << filepath << endl;
    m_filesOpened.erase(filepath);
    wxString file_content = json["params"]["textDocument"]["text"].toString();
    // update comments cache
    update_comments_for_file(filepath, file_content);

    // update using namespace cache
    parse_file_for_includes_and_using_namespace(filepath);

    // keep the file content in-cache
    m_filesOpened.insert({ filepath, file_content });
}

// Notification -->
void ProtocolHandler::on_did_close(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(channel);
    auto json = msg->toElement();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();
    m_filesOpened.erase(filepath);
    update_comments_for_file(filepath, wxEmptyString);
    // clear various caches for this file
    m_comments_cache.erase(filepath);
    m_parsed_files_info.erase(filepath);
    m_additional_scopes.erase(filepath);
}

// Notification -->
void ProtocolHandler::on_did_change(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(channel);
    // keep the file content in the internal map
    auto json = msg->toElement();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    clDEBUG() << "textDocument/didChange: caching new content for file:" << filepath << endl;
    m_filesOpened.erase(filepath);
    wxString file_content = json["params"]["contentChanges"][0]["text"].toString();
    m_comments_cache.erase(filepath);
    m_filesOpened.insert({ filepath, file_content });

    clDEBUG() << "Updating content for file:" << filepath << endl;
    clDEBUG1() << file_content << endl;

    // update using namespace cache
    parse_file_for_includes_and_using_namespace(filepath);
}

// Request <-->
void ProtocolHandler::on_completion(unique_ptr<JSON>&& msg, Channel& channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    // ensure the indexer is running
    if(!m_codelite_indexer->is_running()) {
        m_codelite_indexer->start();
    }

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    // get the expression at this given position
    wxString last_word;
    CompletionHelper helper;
    clDEBUG1() << "Truncating file..." << endl;
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character,
                                                     CompletionHelper::TRUNCATE_EXACT_POS);
    clDEBUG1() << "Success" << endl;

    clDEBUG1() << "Getting expression..." << endl;
    wxString expression = helper.get_expression(text, false, &last_word);

    clDEBUG1() << "Success" << endl;
    clDEBUG() << "resolving expression:" << expression << endl;

    bool is_trigger_char =
        !expression.empty() && (expression.Last() == '>' || expression.Last() == ':' || expression.Last() == '.');
    bool is_function_calltip = !expression.empty() && expression.Last() == '(';

    wxString file_name;
    wxString suffix;
    bool is_include_completion = helper.is_include_statement(text, &file_name, &suffix);
    update_additional_scopes_for_file(filepath);

    vector<TagEntryPtr> candidates;
    if(is_trigger_char) {
        clDEBUG() << "CodeComplete expression:" << expression << endl;
        TagsManagerST::Get()->AutoCompleteCandidates(filepath, line + 1, expression, text, candidates);
        clDEBUG() << "Number of completion entries:" << candidates.size() << endl;
        clDEBUG1() << candidates << endl;
    } else if(is_function_calltip) {
        // TODO:
        // function calltip
    } else if(is_include_completion) {
        // provide a list of files for code completion
        wxArrayString files;
        clDEBUG() << "File Completion:" << filepath << endl;
        TagsManagerST::Get()->GetFilesForCC(file_name, files);
        candidates.reserve(files.size());
        clDEBUG() << files << endl;
        for(const wxString& file : files) {
            // exclude source file
            if(FileExtManager::GetType(file) == FileExtManager::TypeSourceC ||
               FileExtManager::GetType(file) == FileExtManager::TypeSourceCpp) {
                continue;
            }
            TagEntryPtr tag(new TagEntry());

            wxString display_name = file + suffix;
            tag->SetKind("file");
            tag->SetName(display_name); // display name
            display_name = display_name.AfterLast('/');
            tag->SetPattern(display_name); // insert text
            tag->SetLine(-1);
            candidates.push_back(tag);
        }
    } else {
        // word completion
        clDEBUG() << "WordComplete expression:" << expression << endl;
        TagsManagerST::Get()->WordCompletionCandidates(filepath, line + 1, expression, text, last_word, candidates);
        clDEBUG() << "Number of completion entries:" << candidates.size() << endl;
        clDEBUG1() << candidates << endl;
    }

    if(!candidates.empty()) {
        // ensure all relevant files have been parsed for comments
        clDEBUG() << "Updating comments for matches..." << endl;
        for(auto tag : candidates) {
            wxFileName fn(tag->GetFile());
            if(!do_comments_exist_for_file(fn.GetFullPath())) {
                update_comments_for_file(fn.GetFullPath());
            }
        }
        clDEBUG() << "Success" << endl;
        clDEBUG() << "Building reply..." << endl;
        JSON root(cJSON_Object);
        JSONItem response = root.toElement();
        auto result = build_result(response, id, cJSON_Object);

        result.addProperty("isIncomplete", false);
        auto items = result.AddArray("items");

        // send them over the client
        for(TagEntryPtr tag : candidates) {
            wxString comment_string =
                get_comment(wxFileName(tag->GetFile()).GetFullPath(), tag->GetLine() - 1, wxEmptyString);
            tag->SetComment(comment_string);
            auto item = items.AddObject(wxEmptyString);
            wxString doc_comment = helper.format_comment(tag, comment_string);
            auto documentation = item.AddObject("documentation");
            documentation.addProperty("kind", "markdown");
            documentation.addProperty("value", doc_comment);

            item.addProperty("label", tag->GetDisplayName());
            item.addProperty("filterText", tag->GetName());
            item.addProperty("insertText", tag->GetKind() == "file" ? tag->GetPattern() : tag->GetName());
            item.addProperty("detail", tag->GetReturnValue());

            // set the kind
            CompletionItem::eCompletionItemKind kind = LSPUtils::get_completion_kind(tag.Get());
            item.addProperty("kind", static_cast<int>(kind));
        }

        clDEBUG() << "Sending the reply..." << endl;
        channel.write_reply(response.format(false));
        clDEBUG() << "Success" << endl;
    }
}

// Notificatin -->
void ProtocolHandler::on_did_save(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(channel);
    JSONItem json = msg->toElement();
    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    clDEBUG() << "textDocument/didSave: caching new content for file:" << filepath << endl;
    m_filesOpened.erase(filepath);
    m_filesOpened.insert({ filepath, json["params"]["contentChanges"][0]["text"].toString() });

    // update the file using namespace
    parse_file_for_includes_and_using_namespace(filepath);

    // delete the symbols generated from this file
    TagsManagerST::Get()->GetDatabase()->DeleteByFileName({}, filepath, true);

    // re-parse the file
    wxArrayString files;
    files.Add(filepath);

    wxArrayString includes = get_files_to_parse(get_first_level_includes(filepath));
    files.reserve(files.size() + includes.size());
    files.insert(files.end(), includes.begin(), includes.end());
    parse_files(files, nullptr);

    if(TagsManagerST::Get()->GetDatabase()) {
        TagsManagerST::Get()->ClearTagsCache();
    }
    // clear the cached "using namespace"
    m_additional_scopes.clear();
}

namespace
{
void add_to_locals_set(const wxString& name, wxStringSet_t& locals, wxStringSet_t& types)
{
    if(types.count(name)) {
        types.erase(name);
    }
    locals.insert(name);
}

void add_to_types_set(const wxString& name, wxStringSet_t& types, const wxStringSet_t& locals)
{
    if(locals.count(name)) {
        return;
    }
    types.insert(name);
}
} // namespace

// Request <-->
void ProtocolHandler::on_semantic_tokens(unique_ptr<JSON>&& msg, Channel& channel)
{
    JSONItem json = msg->toElement();
    clDEBUG1() << json.format() << endl;
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG() << "textDocument/semanticTokens/full: for file" << filepath << endl;

    // use CTags to gather local variables
    wxString tmpdir = clStandardPaths::Get().GetTempDir();
    vector<TagEntry> tags =
        CTags::Run(filepath, tmpdir, "--excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+l --C++-kinds=+l ",
                   m_settings.GetCodeliteIndexer());

    clDEBUG1() << "File tags:" << tags.size() << endl;
    wxStringSet_t locals_set;
    wxStringSet_t types_set;

    for(const auto& tag : tags) {
        if(tag.IsLocalVariable()) {
            wxString type = tag.GetLocalType();
            auto parts = wxStringTokenize(type, ":", wxTOKEN_STRTOK);
            for(const wxString& part : parts) {
                add_to_types_set(part, types_set, locals_set);
            }
            add_to_locals_set(tag.GetName(), locals_set, types_set);
        } else if(tag.IsMethod()) {
            const wxString& path = tag.GetPath();
            auto parts = wxStringTokenize(path, ":", wxTOKEN_STRTOK);
            if(!parts.empty()) {
                // the last part is the method name, we don't want to include it
                parts.pop_back();
            }
            for(const wxString& part : parts) {
                add_to_types_set(part, types_set, locals_set);
            }

            // we also want the method signature arguments
            wxString signature = tag.GetSignature();
            CxxVariableScanner scanner(signature, eCxxStandard::kCxx11, {}, true);
            auto vars = scanner.ParseFunctionArguments();
            for(const auto& var : vars) {
                add_to_locals_set(var->GetName(), locals_set, types_set);
                const auto& typeParts = var->GetType();
                for(const auto& p : typeParts) {
                    if(p.type == T_IDENTIFIER) {
                        add_to_types_set(p.text, types_set, locals_set);
                    }
                }
            }
        } else if(tag.IsClass() || tag.IsStruct() || tag.GetKind() == "enum") {
            add_to_types_set(tag.GetName(), types_set, locals_set);
        } else if(tag.GetKind() == "enumerator") {
            add_to_locals_set(tag.GetName(), locals_set, types_set);
        } else if(tag.GetKind() == "member") {
            add_to_locals_set(tag.GetName(), locals_set, types_set);
            CxxVariableScanner scanner(tag.GetPatternClean(), eCxxStandard::kCxx11, {}, false);
            auto vars = scanner.GetVariables();
            for(const auto& var : vars) {
                const auto& typeParts = var->GetType();
                for(const auto& p : typeParts) {
                    if(p.type == T_IDENTIFIER) {
                        add_to_types_set(p.text, types_set, locals_set);
                    }
                }
            }
        }
    }

    clDEBUG1() << "The following semantic tokens were found:" << endl;
    clDEBUG1() << "Locals:" << locals_set << endl;
    clDEBUG1() << "Types:" << types_set << endl;

    wxString buffer;
    FileUtils::ReadFileContent(filepath, buffer);

    vector<TokenWrapper> tokens_vec;
    tokens_vec.reserve(1000);

    // collect all interesting tokens from the document
    SimpleTokenizer tokenizer(buffer);
    TokenWrapper token_wrapper;
    wxArrayString words;
    words.reserve(1000);

    wxStringSet_t visited;
    wxArrayString simple_tokens;
    while(tokenizer.next(&token_wrapper.token)) {
        const auto& tok = token_wrapper.token;
        wxString word = tok.to_string(buffer);
        simple_tokens.Add(word);
        if(!CompletionHelper::is_cxx_keyword(word) && (visited.count(word) == 0)) {
            visited.insert(word);
            if(tok.following_char_is('(')) {
                token_wrapper.type = TYPE_FUNCTION;
                tokens_vec.push_back(token_wrapper);
            } else if(tok.following_char_is('.')) {
                token_wrapper.type = TYPE_VARIABLE;
                tokens_vec.push_back(token_wrapper);
            } else if(locals_set.count(word)) {
                // we know that this one is a variable
                token_wrapper.type = TYPE_VARIABLE;
                tokens_vec.push_back(token_wrapper);

            } else if(types_set.count(word)) {
                token_wrapper.type = TYPE_CLASS;
                tokens_vec.push_back(token_wrapper);
            }
        }
    }

    // build the response
    size_t id = json["id"].toSize_t();
    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    auto result = build_result(response, id, cJSON_Object);

    vector<int> encoding;
    encoding.reserve(5 * tokens_vec.size());

    clDEBUG() << "Found" << tokens_vec.size() << "semantic tokens" << endl;
    LSPUtils::encode_semantic_tokens(tokens_vec, &encoding);
    result.addProperty("data", encoding);
    clDEBUG1() << response.format() << endl;
    channel.write_reply(response);
}

// Request <-->
void ProtocolHandler::on_workspace_symbol(unique_ptr<JSON>&& msg, Channel& channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    wxString query = json["params"]["query"].toString();
    wxArrayString parts = ::wxStringTokenize(query, " \t", wxTOKEN_STRTOK);

    vector<TagEntryPtr> tags;
    TagsManagerST::Get()->GetTagsByPartialNames(parts, tags);

    // build the reply
    JSON root(cJSON_Object);
    auto response = root.toElement();
    auto result = build_result(response, id, cJSON_Array);

    auto symbols = LSPUtils::to_symbol_information_array(tags, false);
    for(const LSP::SymbolInformation& symbol : symbols) {
        result.arrayAppend(symbol.ToJSON(wxEmptyString));
    }
    channel.write_reply(response);
}

// Request <-->
void ProtocolHandler::on_document_symbol(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(msg);
    wxUnusedVar(channel);

    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    clDEBUG1() << json.format() << endl;
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG1() << "textDocument/documentSymbol: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    // parse the file and return the symbols
    vector<TagEntryPtr> tags = TagsManagerST::Get()->ParseBuffer(m_filesOpened[filepath], filepath);

    // tags are sorted by line number, just wrap them in JSON and send them over to the client
    JSON root(cJSON_Object);
    auto response = root.toElement();
    auto result = build_result(response, id, cJSON_Array);

    auto symbols = LSPUtils::to_symbol_information_array(tags, true);
    for(const LSP::SymbolInformation& symbol : symbols) {
        result.arrayAppend(symbol.ToJSON(wxEmptyString));
    }
    channel.write_reply(response);
}

// Request <-->
void ProtocolHandler::on_document_signature_help(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(channel);

    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    clDEBUG1() << json.format() << endl;
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG1() << "textDocument/signatureHelp: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    wxString last_word;
    CompletionHelper helper;
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character,
                                                     CompletionHelper::TRUNCATE_EXACT_POS);
    wxString expression = helper.get_expression(text, true, &last_word);

    clDEBUG() << "resolving expression:" << expression << endl;
    clCallTipPtr tip = TagsManagerST::Get()->GetFunctionTip(filepath, line + 1, expression, text, last_word);
    LSP::SignatureHelp sh;
    if(tip) {
        CompletionHelper helper;
        wxString return_value;
        LSP::SignatureInformation::Vec_t signatures;
        signatures.reserve(tip->Count());
        for(int i = 0; i < tip->Count(); ++i) {
            wxString tip_text = tip->TipAt(i);

            signatures.emplace_back();
            LSP::SignatureInformation& si = signatures.back();
            LSP::ParameterInformation::Vec_t parameters;

            auto params = helper.split_function_signature(tip_text, &return_value);
            parameters.reserve(params.size());

            for(const auto& param : params) {
                LSP::ParameterInformation pi;
                pi.SetLabel(param);
                parameters.push_back(pi);
            }
            si.SetParameters(parameters);
            si.SetLabel(tip_text);
        }
        sh.SetSignatures(signatures);
    }
    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    auto result = sh.ToJSON("result");
    response.addProperty("id", id);
    response.addProperty("jsonrpc", "2.0");
    response.append(result);
    channel.write_reply(response);
}

void ProtocolHandler::on_hover(unique_ptr<JSON>&& msg, Channel& channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();
    clDEBUG1() << json.format() << endl;

    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG1() << "textDocument/signatureHelp: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    // get the expression at this given position
    wxString last_word;
    CompletionHelper helper;
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character,
                                                     CompletionHelper::TRUNCATE_COMPLETE_WORDS);
    wxString expression = helper.get_expression(text, false, &last_word);

    clDEBUG() << "resolving expression:" << expression << endl;

    vector<wxString> tips;
    TagsManagerST::Get()->GetHoverTip(filepath, line + 1, expression, last_word, text, tips);

    // construct the content
    wxString tip_content;
    for(wxString& tip : tips) {
        tip.Trim().Trim(false);
        if(!tip_content.empty()) {
            tip_content << "\n---\n";
        }
        tip_content << tip;
    }

    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    auto result = build_result(response, id, cJSON_Object);
    auto contents = result.AddObject("contents");
    contents.addProperty("kind", "markdown");
    contents.addProperty("value", tip_content);
    channel.write_reply(response);
}

void ProtocolHandler::do_definition(unique_ptr<JSON>&& msg, Channel& channel, bool try_definition_first)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    clDEBUG1() << json.format() << endl;
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG() << "textDocument/definition: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    CompletionHelper helper;
    wxString last_word;
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character,
                                                     CompletionHelper::TRUNCATE_COMPLETE_WORDS);
    wxString expression = helper.get_expression(text, false, &last_word);

    // get the last line
    wxString suffix;
    wxString file_name;

    wxString text2 = helper.truncate_file_to_location(m_filesOpened[filepath], line, character,
                                                      CompletionHelper::TRUNCATE_COMPLETE_LINES);
    bool is_include_completion = helper.is_include_statement(text2, &file_name, &suffix);
    if(is_include_completion) {
        clDEBUG() << "Resolving #include" << file_name << endl;
    } else {
        clDEBUG() << "Calling WordCompletionCandidates with expression:" << expression << ", last_word=" << last_word
                  << endl;
    }

    vector<TagEntryPtr> tags;
    wxString file_match;
    if(is_include_completion) {
        clDEBUG() << "Find incldue file:" << file_name << endl;
        for(const wxString& search_path : m_search_paths) {
            wxString full_path = search_path + "/" + file_name;
            clDEBUG1() << "Trying path:" << full_path << endl;
            wxFileName fn(full_path);
            fn.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
            if(fn.FileExists()) {
                // this is our match, construct a response and send it back
                file_match = fn.GetFullPath();
                clDEBUG() << " --> Match found:" << file_match << endl;
                break;
            }
        }
    } else {
        update_additional_scopes_for_file(filepath);
        TagEntryPtr match = TagsManagerST::Get()->FindDefinition(filepath, line + 1, expression, last_word, text);
        if(match) {
            tags.push_back(match);
        }
        clDEBUG() << " --> Match found:" << tags.size() << "matches" << endl;
        clDEBUG() << tags << endl;
    }

    // build the result
    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    response.addProperty("id", id);
    response.addProperty("jsonrpc", "2.0");
    auto result = response.AddArray("result");

    if(is_include_completion) {
        // prepare a single file match result
        auto match = result.AddObject(wxEmptyString);
        LSP::Range range;
        range.SetStart({ 0, 0 }).SetEnd({ 0, 0 });
        match.append(range.ToJSON("range"));
        match.addProperty("uri", wxFileSystem::FileNameToURL(file_match));
    } else {
        // add all the results
        for(TagEntryPtr tag : tags) {
            if(tag->GetName() == last_word) {
                clDEBUG() << " --> Adding tag:" << tag->GetName() << tag->GetFile() << ":" << tag->GetLine() << endl;
                auto match = result.AddObject(wxEmptyString);
                // we can only provide line number...
                LSP::Range range;
                range.SetStart({ tag->GetLine() - 1, 0 }).SetEnd({ tag->GetLine() - 1, 0 });
                match.append(range.ToJSON("range"));
                match.addProperty("uri", wxFileSystem::FileNameToURL(tag->GetFile()));
            }
        }
    }
    channel.write_reply(response);
}

void ProtocolHandler::on_declaration(unique_ptr<JSON>&& msg, Channel& channel)
{
    do_definition(move(msg), channel, false);
}

void ProtocolHandler::on_definition(unique_ptr<JSON>&& msg, Channel& channel)
{
    do_definition(move(msg), channel, true);
}

wxArrayString ProtocolHandler::FilterNonWantedNamespaces(const wxArrayString& namespace_arr) const
{
    wxArrayString scopes;
    scopes.reserve(namespace_arr.size());

    for(const wxString& scope : namespace_arr) {
        // filter internal scopes from std
        if(!scope.StartsWith("std::_")) {
            scopes.Add(scope);
        }
    }
    // the global namespace is **always** last
    scopes.Add("<global>"); // always include the global namespace
    return scopes;
}

void ProtocolHandler::build_search_path()
{
    // build the workspace file list
    wxArrayString files;
    if(read_file_list(files) == 0) {
        // try the scan dir
        scan_dir(m_root_folder, m_settings, files);
    }

    // build search path which is combinbed for the paths of the workspace files
    // and from the settings
    wxStringSet_t unique_paths;
    for(const wxString& file : files) {
        wxFileName fn(file);
        fn.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
        wxString path = fn.GetPath();
        if(unique_paths.count(path) == 0) {
            m_search_paths.Add(path);
            unique_paths.insert(path);
        }
    }

    // add the global search paths
    for(const wxString& file : m_settings.GetSearchPath()) {
        wxFileName fn(file, wxEmptyString);
        fn.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
        wxString path = fn.GetPath();
        if(unique_paths.count(path) == 0) {
            m_search_paths.Add(path);
            unique_paths.insert(path);
        }
    }
}

void ProtocolHandler::parse_file_for_includes_and_using_namespace(const wxString& filepath)
{
    ParsedFileInfo entry;
    m_file_scanner.scan(filepath, m_search_paths, &entry.included_files, &entry.using_namespace);

    m_parsed_files_info.erase(filepath);
    m_parsed_files_info.insert({ filepath, entry });
}

wxArrayString ProtocolHandler::get_files_to_parse(const wxArrayString& files)
{
    clDEBUG() << "Scanning for files to parse (base list contains:" << files.size() << "files)" << endl;
    deque<wxString> files_to_parse{ files.begin(), files.end() };

    wxArrayString result;
    result.reserve(10000);

    while(!files_to_parse.empty()) {
        wxString filepath = files_to_parse.front();
        files_to_parse.pop_front();
        if(m_parsed_files_info.count(filepath)) {
            continue;
        }
        result.Add(filepath);
        parse_file_for_includes_and_using_namespace(filepath);

        const auto& includes = m_parsed_files_info[filepath].included_files;
        // append all its children to the vector
        files_to_parse.insert(files_to_parse.end(), includes.begin(), includes.end());
    }

    result.Shrink();
    clDEBUG() << "List of files to parse:" << result.size() << endl;
    return result;
}

wxArrayString ProtocolHandler::get_first_level_includes(const wxString& filepath)
{
    // get list of files included directly by this file
    wxArrayString files;
    if(m_parsed_files_info.count(filepath)) {
        const ParsedFileInfo& parsed_info = m_parsed_files_info[filepath];
        files.reserve(files.size() + parsed_info.included_files.size());
        for(const wxString& include : parsed_info.included_files) {
            files.Add(include);
        }
    }
    return files;
}
