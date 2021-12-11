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

#include <iostream>
#include <wx/filesys.h>

using LSP::CompletionItem;
using LSP::eSymbolKind;

namespace
{
wxStopWatch sw;
FileLogger& operator<<(FileLogger& logger, const wxStringSet_t& arr)
{
    wxString s;
    s << "[";
    for(const auto& d : arr) {
        s << d << ",";
    }
    s << "]";
    logger.Append(s, logger.GetRequestedLogLevel());
    return logger;
}

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

void remove_binary_files(wxArrayString& files)
{
    wxArrayString res;
    res.reserve(files.size());

    for(const wxString& s : files) {
        wxFileName filename(s);
        filename.MakeAbsolute();
        filename.Normalize();
        wxString fullpath = filename.GetFullPath();
        if(TagsManagerST::Get()->IsBinaryFile(fullpath, {}))
            continue;
        res.push_back(fullpath);
    }
    files.swap(res);
}

/**
 * @brief given a list of files, remove all non c/c++ files from it
 */
void filter_non_cxx_files(wxArrayString& files)
{
    wxArrayString tmparr;
    tmparr.reserve(files.size());

    // filter non c/c++ files
    for(wxString& file : files) {
        file.Trim().Trim(false);
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
    filter_non_cxx_files(files);
}

// TagEntryPtr create_fake_entry(const wxString& name, const wxString& kind)
//{
//     TagEntryPtr t(new TagEntry());
//     t->SetKind(kind);
//     t->SetName(name);
//     t->SetLine(wxNOT_FOUND);
//     return t;
//}

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
    params.addProperty("message", message);
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

void ProtocolHandler::parse_files(wxArrayString& files, Channel* channel, bool initial_parse)
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
    wxUnusedVar(initial_parse);

    // if(initial_parse && !db->CheckIntegrity()) {
    //     // delete the database
    //     db->RecreateDatabase();
    // }

    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(files, db);
    start_timer();

    // keep track of files we visited
    std::unordered_set<wxString> visited_files;

    remove_binary_files(files);

    // build search path which is combinbed for the paths of the workspace files
    // and from the settings
    wxArrayString search_paths;
    wxStringSet_t unique_paths;
    for(const wxString& file : files) {
        wxFileName fn(file);
        wxString path = fn.GetPath();
        if(unique_paths.count(path) == 0) {
            search_paths.Add(path);
            unique_paths.insert(path);
        }
    }

    for(const wxString& file : m_settings.GetSearchPath()) {
        wxFileName fn(file, wxEmptyString);
        wxString path = fn.GetPath();
        if(unique_paths.count(path) == 0) {
            search_paths.Add(path);
            unique_paths.insert(path);
        }
    }

    clDEBUG() << "Scanning for `#include` files..." << endl;
    clDEBUG() << "Search path:" << search_paths << endl;

    CxxPreProcessor pp{ search_paths, 50 };
    for(size_t i = 0; i < files.size(); i++) {
        if(visited_files.insert(files[i]).second) {
            // we did not visit this file just yet
            // parse it and collect both the files included from it
            // and all calls to "using namespace XXX"
            CxxUsingNamespaceCollector collector{ &pp, files[i], visited_files };
            collector.Parse();
            if(m_using_namespace_cache.count(files[i])) {
                m_using_namespace_cache.erase(files[i]);
            }
            m_using_namespace_cache.insert({ files[i], FilterNonWantedNamespaces(collector.GetUsingNamespaces()) });
        }
    }
    clDEBUG() << "Success" << endl;
    wxStringSet_t unique_files{ files.begin(), files.end() };
    for(const wxString& file : visited_files) {
        wxFileName fn(file);
        if(!fn.IsAbsolute()) {
            fn.MakeAbsolute(m_root_folder);
        }
        unique_files.insert(fn.GetFullPath());
    }

    files.clear();
    files.reserve(unique_files.size());

    for(const auto& s : unique_files) {
        files.Add(s);
    }
    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(files, db);

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
    vector<wxString> additional_scopes;
    auto where = m_using_namespace_cache.find(filepath);
    if(where != m_using_namespace_cache.end()) {
        clDEBUG() << "Setting additional scopes of:" << where->second << endl;
        additional_scopes.insert(additional_scopes.end(), where->second.begin(), where->second.end());
    }
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

void ProtocolHandler::update_using_namespace_for_file(const wxString& filepath)
{
    if(m_using_namespace_cache.count(filepath)) {
        return;
    }

    wxArrayString file_list;
    read_file_list(file_list);

    // build search path which is combinbed for the paths of the workspace files
    // and from the settings
    wxArrayString search_paths;
    wxStringSet_t unique_paths;
    for(const wxString& file : file_list) {
        wxFileName fn(file);
        wxString path = fn.GetPath();
        if(unique_paths.count(path) == 0) {
            search_paths.Add(path);
            unique_paths.insert(path);
        }
    }

    for(const wxString& file : m_settings.GetSearchPath()) {
        wxFileName fn(file);
        wxString path = fn.GetPath();
        if(unique_paths.count(path) == 0) {
            search_paths.Add(path);
            unique_paths.insert(path);
        }
    }

    clDEBUG() << "  Updating extra scopes for file" << filepath << endl;
    CxxPreProcessor pp{ search_paths, 20 };

    // we did not visit this file just yet
    // parse it and collect both the files included from it
    // and all calls to "using namespace XXX"
    wxStringSet_t visited_files;
    CxxUsingNamespaceCollector collector(&pp, filepath, visited_files);
    collector.Parse();
    if(m_using_namespace_cache.count(filepath)) {
        m_using_namespace_cache.erase(filepath);
    }

    m_using_namespace_cache.insert({ filepath, FilterNonWantedNamespaces(collector.GetUsingNamespaces()) });
    clDEBUG() << "  Visited" << visited_files.size() << "header files" << endl;
    clDEBUG() << "  scopes for file:" << filepath << "is now set to:" << m_using_namespace_cache[filepath] << endl;
    clDEBUG() << "Success" << endl;
}

size_t ProtocolHandler::read_file_list(wxArrayString& arr) const
{
    arr.clear();
    wxFileName file_list(m_settings_folder, "file_list.txt");
    if(file_list.FileExists()) {
        wxString file_list_content;
        FileUtils::ReadFileContent(file_list, file_list_content);
        wxArrayString files = wxStringTokenize(file_list_content, "\n", wxTOKEN_STRTOK);
        filter_non_cxx_files(files);
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
    tokenTypes.arrayAppend("variable"); // TYPE_VARIABLE
    tokenTypes.arrayAppend("class");    // TYPE_CLASS

    // load the configuration file
    m_root_folder = json["params"]["rootUri"].toString();
    m_root_folder = wxFileSystem::URLToFileName(m_root_folder).GetFullPath();

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

    // build the workspace file list
    wxArrayString files;
    if(read_file_list(files) == 0) {
        // try the scan dir
        scan_dir(m_root_folder, m_settings, files);
    }
    parse_files(files, &channel, true);

    TagsManagerST::Get()->CloseDatabase();
    TagsManagerST::Get()->OpenDatabase(wxFileName(m_settings_folder, "tags.db"));

    // construct TagsOptionsData
    TagsOptionsData tod;
    tod.SetFileSpec(m_settings.GetFileMask());
    tod.SetTokens(MapToString(m_settings.GetTokens()));
    tod.SetTypes(MapToString(m_settings.GetTypes()));
    TagsManagerST::Get()->SetCtagsOptions(tod);

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
    update_using_namespace_for_file(filepath);

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
    m_using_namespace_cache.erase(filepath);
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
    update_using_namespace_for_file(filepath);
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
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character, false);
    wxString expression = helper.get_expression(text, false, &last_word);

    clDEBUG() << "resolving expression:" << expression << endl;

    bool is_trigger_char =
        !expression.empty() && (expression.Last() == '>' || expression.Last() == ':' || expression.Last() == '.');
    bool is_function_calltip = !expression.empty() && expression.Last() == '(';

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
            item.addProperty("insertText", tag->GetName());
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
    m_using_namespace_cache.erase(filepath);
    update_using_namespace_for_file(filepath);

    // delete the symbols generated from this file
    TagsManagerST::Get()->GetDatabase()->DeleteByFileName({}, filepath, true);

    // re-parse the file
    wxArrayString files;
    files.Add(filepath);
    parse_files(files, nullptr, false);

    if(TagsManagerST::Get()->GetDatabase()) {
        TagsManagerST::Get()->ClearTagsCache();
    }
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
            if(locals_set.count(word)) {
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
    wxString tmpdir = clStandardPaths::Get().GetTempDir();
    vector<TagEntry> tags = CTags::Run(filepath, tmpdir, wxEmptyString, m_settings.GetCodeliteIndexer());
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

    clDEBUG() << json.format() << endl;
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG1() << "textDocument/signatureHelp: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    wxString last_word;
    CompletionHelper helper;
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character, false);
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
    clDEBUG() << json.format() << endl;

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
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character, true);
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

void ProtocolHandler::on_definition(unique_ptr<JSON>&& msg, Channel& channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    clDEBUG() << json.format() << endl;
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG1() << "textDocument/definition: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    CompletionHelper helper;
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character, true);

    wxString last_word;
    wxString expression = helper.get_expression(text, false, &last_word);

    clDEBUG() << "Calling WordCompletionCandidates with expression:" << expression << ", last_word=" << last_word
              << endl;

    update_additional_scopes_for_file(filepath);
    vector<TagEntryPtr> tags;
    TagsManagerST::Get()->FindImplDecl(filepath, line + 1, expression, last_word, text, tags, true, false);
    if(tags.empty()) {
        // try the declaration
        TagsManagerST::Get()->FindImplDecl(filepath, line + 1, expression, last_word, text, tags, false, false);
    }

    clDEBUG() << "Found" << tags.size() << "matches" << endl;

    // build the result
    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    response.addProperty("id", id);
    response.addProperty("jsonrpc", "2.0");
    auto result = response.AddArray("result");

    // add all the results
    for(auto tag : tags) {
        if(tag->GetName() == last_word) {
            auto match = result.AddObject(wxEmptyString);
            // we can only provide line number...
            LSP::Range range;
            range.SetStart({ tag->GetLine() - 1, 0 });
            range.SetEnd({ tag->GetLine() - 1, 0 });
            match.append(range.ToJSON("range"));
            match.addProperty("uri", wxFileSystem::FileNameToURL(tag->GetFile()));
        }
    }
    channel.write_reply(response);
}

wxArrayString ProtocolHandler::FilterNonWantedNamespaces(const wxArrayString& namespace_arr) const
{
    wxArrayString scopes;
    scopes.reserve(namespace_arr.size());

    scopes.Add("<global>"); // always include the global namespace
    for(const wxString& scope : namespace_arr) {
        // filter internal scopes from std
        if(!scope.StartsWith("std::_")) {
            scopes.Add(scope);
        }
    }
    return scopes;
}
