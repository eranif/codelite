#include "ProtocolHandler.hpp"

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
#include "Scanner.hpp"
#include "Settings.hpp"
#include "SimpleTokenizer.hpp"
#include "clFilesCollector.h"
#include "clTempFile.hpp"
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
#include <wx/stackwalk.h>

using LSP::CompletionItem;
using LSP::eSymbolKind;

namespace
{
FileLogger& operator<<(FileLogger& logger, const TagEntry& tag)
{
    wxString s;
    s << tag.GetKind() << ", " << tag.GetDisplayName() << ", " << tag.GetPatternClean();
    logger.Append(s, logger.GetRequestedLogLevel());
    return logger;
}

FileLogger& operator<<(FileLogger& logger, const std::vector<TagEntryPtr>& tags)
{
    for(const auto& tag : tags) {
        logger << (*tag) << endl;
    }
    return logger;
}

void remove_db_if_needed(const wxString& dbpath)
{
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbpath);
    if(db->GetVersion() != db->GetSchemaVersion()) {
        clSYSTEM() << "DB version schema upgrade is required" << endl;
        clSYSTEM() << "New schema version is:" << db->GetVersion() << endl;
        clSYSTEM() << "Current schema version is:" << db->GetSchemaVersion() << endl;
        db.Reset(nullptr);
        FileUtils::RemoveFile(dbpath);
    } else {
        clDEBUG() << "No schema changes detected" << endl;
    }
}

inline size_t count_lines(const wxString& content)
{
    size_t lf_count = 0;
    for(wxChar c : content) {
        if(c == '\n') {
            lf_count++;
        }
    }
    return lf_count;
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

/**
 * @brief read a list of files from .ctagsd/file_list.txt
 * filter all non important files
 * @param settings_dir
 * @param arr
 * @return
 */
size_t read_file_list(const wxString& settings_dir, const CTagsdSettings& settings, wxArrayString& arr)
{
    arr.clear();
    wxFileName file_list(settings_dir, "file_list.txt");
    if(file_list.FileExists()) {
        wxString file_list_content;
        FileUtils::ReadFileContent(file_list, file_list_content);
        wxArrayString files = wxStringTokenize(file_list_content, "\n", wxTOKEN_STRTOK);
        filter_non_important_files(files, settings);
        arr.swap(files);
    }
    return arr.size();
}

} // namespace

ProtocolHandler::ProtocolHandler() {}

ProtocolHandler::~ProtocolHandler() { m_parse_thread.stop(); }

void ProtocolHandler::send_log_message(const wxString& message, int level, Channel::ptr_t channel)
{
    JSON root(cJSON_Object);
    JSONItem notification = root.toElement();
    notification.addProperty("method", "window/logMessage");
    notification.addProperty("jsonrpc", "2.0");
    auto params = notification.AddObject("params");
    params.addProperty("message", wxString() << "P:" << wxGetProcessId() << ": " << message);
    params.addProperty("type", level);

    channel->write_reply(notification.format(false));
}

JSONItem ProtocolHandler::build_result(JSONItem& reply, size_t id, int result_kind)
{
    reply.addProperty("id", id);
    reply.addProperty("jsonrpc", "2.0");

    auto result = result_kind == cJSON_Array ? reply.AddArray("result") : reply.AddObject("result");
    return result;
}

void ProtocolHandler::parse_buffer(const wxFileName& filename, const wxString& buffer, const CTagsdSettings& settings)
{
    clDEBUG() << "Parsing buffer of file:" << filename << endl;

    // create/open db
    wxFileName dbfile(settings.GetSettingsDir(), "tags.db");
    if(!dbfile.FileExists()) {
        clDEBUG() << dbfile << "does not exist, will create it" << endl;
    }
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);
    clDEBUG() << "Generating ctags file..." << endl;

    std::vector<TagEntryPtr> tags;
    if(CTags::ParseBuffer(filename, buffer, settings.GetCodeliteIndexer(), settings.GetMacroTable(), tags) == 0) {
        clDEBUG() << "Failed to generate ctags file for buffer. file:" << endl;
        return;
    }

    clDEBUG() << "Success" << endl;
    clDEBUG() << "Updating symbols database..." << endl;

    db->Begin();
    time_t update_time = time(nullptr);
    db->Store(tags, false);

    if(db->InsertFileEntry(filename.GetFullPath(), (int)update_time) == TagExist) {
        db->UpdateFileEntry(filename.GetFullPath(), (int)update_time);
    }

    // Commit whats left
    db->Commit();
    clDEBUG() << "Success" << endl;
}

void ProtocolHandler::parse_file(const wxFileName& filename, const CTagsdSettings& settings)
{
    parse_files({ filename.GetFullPath() }, settings);
}

void ProtocolHandler::do_parse_chunk(ITagsStoragePtr db, const std::vector<wxString>& file_list, size_t chunk_id,
                                     const CTagsdSettings& settings)
{
    std::vector<TagEntryPtr> tags;
    LOG_IF_DEBUG { clDEBUG() << "Parsing chunk (" << chunk_id << ") of" << file_list.size() << "files" << endl; }
    if(CTags::ParseFiles(file_list, settings.GetCodeliteIndexer(), settings.GetMacroTable(), tags) == 0) {
        clDEBUG() << "0 tags generated. processed:" << file_list.size()
                  << "files. Indexer:" << settings.GetCodeliteIndexer() << endl;
        return;
    }

    LOG_IF_TRACE
    {
        clDEBUG1() << "Success" << endl;
        clDEBUG1() << "Updating symbols database..." << endl;
    }
    LOG_IF_DEBUG { clDEBUG() << "Storing" << tags.size() << "tags" << endl; }
    db->Begin();

    time_t update_time = time(nullptr);
    db->Store(tags, false);

    // update the files table in the database
    // we do this here, since some files might not yield tags
    // but we still want to mark them as "parsed"
    for(const wxString& file : file_list) {
        if(db->InsertFileEntry(file, (int)update_time) == TagExist) {
            db->UpdateFileEntry(file, (int)update_time);
        }
    }

    // Commit whats left
    db->Commit();
}

void ProtocolHandler::parse_files(const std::vector<wxString>& file_list, const CTagsdSettings& settings)
{
    clDEBUG() << "Parsing" << file_list.size() << "files" << endl;
    clDEBUG() << "Removing un-modified and unwanted files..." << endl;
    // create/open db
    wxFileName dbfile(settings.GetSettingsDir(), "tags.db");
    if(!dbfile.FileExists()) {
        clDEBUG() << dbfile << "does not exist, will create it" << endl;
    }
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    wxArrayString files_to_parse;
    files_to_parse.reserve(file_list.size());
    for(const wxString& file : file_list) {
        files_to_parse.Add(file);
    }

    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(files_to_parse, db);
    std::vector<wxString> filtered_file_list = { files_to_parse.begin(), files_to_parse.end() };
    clDEBUG() << "There are total of" << filtered_file_list.size() << "files that require parsing" << endl;
    clDEBUG() << "Generating ctags file..." << endl;

    if(filtered_file_list.empty()) {
        return;
    }

    // don't parse all files at once, split them into chunks
    // how many chunks?
    size_t chunk_size = 2500;
    size_t chunk_count = filtered_file_list.size() / chunk_size + 1;
    clDEBUG() << "Parsing" << filtered_file_list.size() << "files..." << endl;
    for(size_t i = 0; i < chunk_count; ++i) {
        // determine the start/end iterators for each range
        size_t start_offset = i * chunk_size;
        auto iter_start = filtered_file_list.begin() + start_offset;
        if(iter_start == filtered_file_list.end())
            // last chunk is empty...
            // this can happen when our initial file list size divides by 1000
            // with 0 remainder
            break;

        auto iter_end = iter_start + chunk_size;
        if((start_offset + chunk_size) > filtered_file_list.size()) {
            iter_end = filtered_file_list.end();
        }
        std::vector<wxString> chunk_vec{ iter_start, iter_end };
        do_parse_chunk(db, chunk_vec, i, settings);
    }
    clDEBUG() << "Success" << endl;
}

std::vector<wxString> ProtocolHandler::update_additional_scopes_for_file(const wxString& filepath)
{
    // we need to visit each node in the file graph and create a set of all the namespaces
    std::vector<wxString> additional_scopes;
    if(m_additional_scopes.count(filepath)) {
        additional_scopes = m_additional_scopes[filepath];
    } else {
        wxStringSet_t visited;
        wxStringSet_t ns;
        std::vector<wxString> Q;
        Q.push_back(filepath);
        while(!Q.empty()) {
            // pop the front of queue
            wxString file = Q.front();
            Q.front() = Q.back();
            Q.pop_back();

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
    auto where = find_if(additional_scopes.begin(), additional_scopes.end(),
                         [](const wxString& scope) { return scope.empty(); });
    if(where == additional_scopes.end()) {
        // no globl scope, add it
        additional_scopes.push_back(wxEmptyString);
    }
    return additional_scopes;
}

bool ProtocolHandler::ensure_file_content_exists(const wxString& filepath, Channel::ptr_t channel, size_t req_id)
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
            channel->write_reply(response);
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

//---------------------------------------------------------------------------------
// protocol handlers
//---------------------------------------------------------------------------------

// Request <-->
void ProtocolHandler::on_initialize(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    // start the parser thread
    clDEBUG() << "Received `initialize` request" << endl;
    send_log_message(_("Initialization started..."), LSP_LOG_INFO, channel);

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

    wxFileName fn_config_file(m_settings_folder, "ctagsd.json");
    m_settings.Load(fn_config_file);

    // this will also generate the ctags.replacements file
    m_settings.Save(fn_config_file);

    // export CTAGS_REPLACEMENTS
    wxFileName ctagsReplacements(m_settings_folder, "ctags.replacements");
    wxSetEnv("CTAGS_REPLACEMENTS", ctagsReplacements.GetFullPath());
    TagsManagerST::Get()->SetIndexerPath(m_settings.GetCodeliteIndexer());

    // start the "on_change" parser thread
    m_parse_thread.start(m_settings_folder, m_settings.GetCodeliteIndexer());

    // build the workspace file list
    wxArrayString files;
    if(read_file_list(m_settings_folder, m_settings, files) == 0) {
        // try the scan dir
        scan_dir(m_root_folder, m_settings, files);
    }
    build_search_path();

    // build a list of files to parse (including all include statements)
    files = get_files_to_parse(files);

    // Check the database version

    // sanity: ensure that we don't have an open handle
    // parse the workspace
    TagsManagerST::Get()->CloseDatabase();
    wxFileName fn_db_path(m_settings_folder, "tags.db");
    remove_db_if_needed(fn_db_path.GetFullPath());

    wxString indexer_path = m_settings.GetCodeliteIndexer();
    std::vector<wxString> files_to_parse = { files.begin(), files.end() };
    clDEBUG() << "on_initialize(): parsing files..." << endl;
    ProtocolHandler::parse_files(files_to_parse, m_settings);
    clDEBUG() << "on_initialize(): parsing files... Success" << endl;

    // Now that the database is parsed, re-open it
    TagsManagerST::Get()->CloseDatabase();
    TagsManagerST::Get()->OpenDatabase(fn_db_path);
    TagsManagerST::Get()->GetDatabase()->SetSingleSearchLimit(m_settings.GetLimitResults());
    TagsManagerST::Get()->GetDatabase()->SetUseCache(true);

    // reparse the workspace
    send_log_message(_("Initialization completed"), LSP_LOG_INFO, channel);

    m_completer.reset(new CxxCodeCompletion(TagsManagerST::Get()->GetDatabase(), m_settings.GetCodeliteIndexer()));
    m_completer->set_macros_table(m_settings.GetTokens());
    m_completer->set_types_table(m_settings.GetTypes());
    channel->write_reply(response.format(false));
}

// Request <-->
void ProtocolHandler::on_unsupported_message(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    wxString message;
    message << _("unsupported message: `") << msg->toElement()["method"].toString() << "`";
    send_log_message(message, LSP_LOG_WARNING, channel);
}

// Notification -->
void ProtocolHandler::on_initialized(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    // a notification
    wxUnusedVar(msg);
    wxUnusedVar(channel);
    clDEBUG() << "Received `initialized` message" << endl;
}

// Notification -->
void ProtocolHandler::on_did_open(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
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

    // make sure this file is up to date
    parse_file(filepath, m_settings);

    // keep the file content in-cache
    m_filesOpened.insert({ filepath, file_content });
}

// Notification -->
void ProtocolHandler::on_did_close(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
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
void ProtocolHandler::on_did_change(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    static wxStringSet_t empty_set;

    wxUnusedVar(channel);
    // keep the file content in the internal map
    auto json = msg->toElement();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    // Check if a real change was made that requires parsing
    size_t line_count_before = 0;
    size_t line_count_after = 0;
    if(m_filesOpened.count(filepath)) {
        line_count_before = count_lines(m_filesOpened[filepath]);
    }
    wxString file_content = json["params"]["contentChanges"][0]["text"].toString();
    line_count_after = count_lines(file_content);

    // update the new content
    clDEBUG() << "textDocument/didChange: caching new content for file:" << filepath << endl;
    m_filesOpened.erase(filepath);
    m_comments_cache.erase(filepath);
    m_filesOpened.insert({ filepath, file_content });
    clDEBUG() << "Updating content for file:" << filepath << endl;

    // we compare the preamble of both before and after the file
    // modification
    // if we see a difference, i.e. new header file was added

    // Note: we make a copy here since the call to `parse_buffer_for_includes_and_using_namespace()`
    // will update `m_parsed_files_info[filepath].included_files`
    auto prev_preamble = m_parsed_files_info.count(filepath) ? m_parsed_files_info[filepath].included_files : empty_set;
    parse_buffer_for_includes_and_using_namespace(filepath, file_content);
    const auto& curr_preabmle = m_parsed_files_info[filepath].included_files;
    auto diff = setdiff(curr_preabmle, prev_preamble);
    wxArrayString new_includes;
    if(!diff.empty()) {
        // curr_preabmle contains new headers that do not exist in the
        // previous preamble - parse them
        wxArrayString tmp;
        for(const auto& header_file : diff) {
            tmp.Add(header_file);
        }
        new_includes = get_files_to_parse(tmp);
        clDEBUG() << "Need to parse these new file:" << new_includes << endl;
    }

    if(line_count_before != line_count_after || !new_includes.empty()) {
        // parse the file buffer
        clDEBUG() << "Re-parsing file:" << filepath << endl;
        wxString indexer_path = m_settings.GetCodeliteIndexer();
        wxString settings_folder = m_settings_folder;
        ParseThreadTaskFunc buffer_parse_task = [=]() {
            clDEBUG() << "on_did_change(): parsing file task" << filepath << endl;
            ProtocolHandler::parse_buffer(filepath, file_content, m_settings);
            clDEBUG() << "on_did_change(): parsing file task ... Success" << endl;
            return eParseThreadCallbackRC::RC_SUCCESS;
        };
        clDEBUG() << "Pushing parse request to worker thread" << endl;
        m_parse_thread.queue_parse_request(std::move(buffer_parse_task));

        // parse the files included by this file
        if(!new_includes.empty()) {
            std::vector<wxString> includes_to_parse{ new_includes.begin(), new_includes.end() };
            ParseThreadTaskFunc headers_parse_task = [=]() {
                clDEBUG() << "on_did_change(): parsing header files" << includes_to_parse << endl;
                ProtocolHandler::parse_files(includes_to_parse, m_settings);
                clDEBUG() << "on_did_change(): parsing header files ... Success" << endl;
                return eParseThreadCallbackRC::RC_SUCCESS;
            };
            m_parse_thread.queue_parse_request(std::move(headers_parse_task));
        }
    } else {
        clDEBUG() << "No real change detected. Will not re-parse the file" << endl;
    }
}

wxString ProtocolHandler::minimize_buffer(const wxString& filepath, int line, int character, const wxString& src_string,
                                          CompletionHelper::eTruncateStyle flag)
{
    // optimization: since we know that the file was saved
    // at one point, we can reduce the processing needed by only using the code from
    // current function downward
    CompletionHelper helper;
    m_completer->set_text(wxEmptyString, filepath, line);

    wxString truncated_text;
    wxString text;
    auto curr_function_tag = m_completer->get_current_function_tag();
    if(curr_function_tag) {
        // remove all the text from the start of text -> scope starting position
        const wxString& orig_text = m_filesOpened[filepath];

        wxArrayString lines = ::wxStringTokenize(orig_text, "\n", wxTOKEN_RET_EMPTY_ALL);
        if((size_t)curr_function_tag->GetLine() < lines.size()) {
            line -= curr_function_tag->GetLine() - 1;
            lines.erase(lines.begin(), lines.begin() + curr_function_tag->GetLine() - 1);
            truncated_text = wxJoin(lines, '\n');
        }

        text = helper.truncate_file_to_location(truncated_text, line, character, flag);
        LOG_IF_TRACE
        {
            clDEBUG1() << "Minimized file into:" << endl;
            clDEBUG1() << text << endl;
        }
    } else {
        // use the entire file content
        text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character, flag);
        LOG_IF_TRACE { clDEBUG1() << "Unable to minimize the buffer, using the complete buffer" << endl; }
    }
    return text;
}

// Request <-->
void ProtocolHandler::on_completion(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    int line = json["params"]["position"]["line"].toInt(0);
    int character = json["params"]["position"]["character"].toInt(0);

    clDEBUG() << "completion requested at:" << filepath << ":" << line << ":" << character << endl;

    wxString last_word;
    CompletionHelper helper;
    wxString suffix;
    const wxString& full_buffer = m_filesOpened[filepath];
    bool is_include_completion = false;
    wxString file_name;
    std::vector<TagEntryPtr> candidates;

    wxArrayString lines = ::wxStringTokenize(full_buffer, "\n", wxTOKEN_RET_EMPTY_ALL);
    if(line < (int)lines.size()) {
        wxString requested_line = lines[line];
        requested_line.Trim();

        clDEBUG() << "checking for include completion:" << requested_line << endl;
        is_include_completion = helper.is_line_include_statement(requested_line, &file_name, &suffix);
    }

    if(is_include_completion) {
        // provide a list of files for code completion
        clDEBUG() << "File Completion:" << filepath << endl;
        m_completer->get_file_completions(file_name, candidates, suffix);
    } else {
        LOG_IF_DEBUG { clDEBUG() << "  --> minimize_buffer() " << endl; }
        wxString minimized_buffer = minimize_buffer(filepath, line, character, full_buffer);
        LOG_IF_DEBUG { clDEBUG() << "  <-- minimize_buffer() " << endl; }
        LOG_IF_TRACE
        {
            clDEBUG1() << "Success" << endl;
            clDEBUG1() << "Getting expression..." << endl;
        }
        LOG_IF_DEBUG { clDEBUG() << "  --> helper.get_expression() " << endl; }
        wxString expression = helper.get_expression(minimized_buffer, false, &last_word);
        LOG_IF_DEBUG { clDEBUG() << "  <-- helper.get_expression() " << endl; }
        LOG_IF_TRACE { clDEBUG1() << "Success" << endl; }
        LOG_IF_DEBUG { clDEBUG() << "resolving expression:" << expression << endl; }
        bool is_trigger_char =
            !expression.empty() && (expression.Last() == '>' || expression.Last() == ':' || expression.Last() == '.');
        bool is_function_calltip = !expression.empty() && expression.Last() == '(';

        LOG_IF_DEBUG { clDEBUG() << "  --> update_additional_scopes_for_file() " << endl; }
        std::vector<wxString> visible_scopes = update_additional_scopes_for_file(filepath);
        LOG_IF_DEBUG { clDEBUG() << "  -<-- update_additional_scopes_for_file() " << endl; }

        if(is_trigger_char) {
            // ----------------------------------
            // code completion
            // ----------------------------------
            LOG_IF_DEBUG { clDEBUG() << "CodeComplete expression:" << expression << endl; }
            CxxRemainder remainder;

            LOG_IF_DEBUG { clDEBUG() << "  --> m_completer->set_text() " << endl; }
            m_completer->set_text(minimized_buffer, filepath, line);
            LOG_IF_DEBUG { clDEBUG() << "  <-- m_completer->set_text() " << endl; }

            LOG_IF_DEBUG { clDEBUG() << "  <-- m_completer->code_complete() " << endl; }
            TagEntryPtr resolved = m_completer->code_complete(expression, visible_scopes, &remainder);
            LOG_IF_DEBUG { clDEBUG() << "  <-- m_completer->code_complete() " << endl; }

            if(resolved) {
                LOG_IF_DEBUG
                {
                    clDEBUG() << "resolved into:" << resolved->GetPath() << endl;
                    clDEBUG() << "filter:" << remainder.filter << endl;
                }
                m_completer->get_completions(resolved, remainder.operand_string, remainder.filter, candidates,
                                             visible_scopes);
            }
            LOG_IF_DEBUG { clDEBUG() << "Number of completion entries:" << candidates.size() << endl; }
            LOG_IF_TRACE { clDEBUG1() << candidates << endl; }
        } else if(is_function_calltip) {
            // TODO:
            // function calltip
        } else {
            // ----------------------------------
            // word completion
            // ----------------------------------
            wxStringSet_t visible_files;
            get_includes_recrusively(filepath, &visible_files);
            m_completer->word_complete(filepath, line, expression, minimized_buffer, visible_scopes, false, candidates,
                                       visible_files);
        }
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
        // truncate the list the match the requested settings
        size_t counter = 0;
        for(TagEntryPtr tag : candidates) {
            if(counter >= m_settings.GetLimitResults()) {
                clDEBUG() << "truncated completion list to:" << m_settings.GetLimitResults()
                          << "(original list size was:" << candidates.size() << ")" << endl;
                break;
            }
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
            item.addProperty("detail", tag->GetTypename());

            // set the kind
            CompletionItem::eCompletionItemKind kind = LSPUtils::get_completion_kind(tag.Get());
            item.addProperty("kind", static_cast<int>(kind));
            counter++;
        }

        clDEBUG() << "Sending the reply..." << endl;
        channel->write_reply(response.format(false));
        clDEBUG() << "Success" << endl;
    }
}

// Notificatin -->
void ProtocolHandler::on_did_save(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    wxUnusedVar(channel);
    JSONItem json = msg->toElement();
    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    wxString file_content = json["params"]["text"].toString();

    clDEBUG() << "textDocument/didSave: caching new content for file:" << filepath << endl;
    clDEBUG() << "new file content size is:" << file_content.size() << endl;

    m_filesOpened.erase(filepath);
    m_filesOpened.insert({ filepath, file_content });

    // update the file using namespace
    clDEBUG() << "did_save: collecting files to parse..." << endl;
    parse_file_for_includes_and_using_namespace(filepath);
    clDEBUG() << "done" << endl;

    // delete the symbols generated from this file
    TagsManagerST::Get()->GetDatabase()->DeleteByFileName({}, filepath, true);

    // re-parse the file
    std::vector<wxString> files;
    files.push_back(filepath);

    wxArrayString includes = get_files_to_parse(get_first_level_includes(filepath));
    files.reserve(files.size() + includes.size());
    files.insert(files.end(), includes.begin(), includes.end());

    // parse this file (async)
    wxString indexer_path = m_settings.GetCodeliteIndexer();
    wxString settings_folder = m_settings_folder;
    ParseThreadTaskFunc task = [=]() {
        clDEBUG() << "on_did_save: parsing task:" << files.size() << "files..." << endl;
        ProtocolHandler::parse_files(files, m_settings);
        clDEBUG() << "on_did_save: parsing task: ... Success!" << endl;
        return eParseThreadCallbackRC::RC_SUCCESS;
    };

    m_parse_thread.queue_parse_request(std::move(task));
    TagsManagerST::Get()->GetDatabase()->ClearCache();

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
void ProtocolHandler::on_semantic_tokens(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    JSONItem json = msg->toElement();
    LOG_IF_TRACE { clDEBUG1() << json.format() << endl; }
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG() << "textDocument/semanticTokens/full: for file" << filepath << endl;

    // use CTags to gather local variables
    wxString tmpdir = clStandardPaths::Get().GetTempDir();
    std::vector<TagEntryPtr> tags;

    // get list of local tags
    CTags::ParseLocals(filepath, m_filesOpened[filepath], m_settings.GetCodeliteIndexer(), m_settings.GetMacroTable(),
                       tags);

    LOG_IF_TRACE { clDEBUG1() << "File tags:" << tags.size() << endl; }
    wxStringSet_t locals_set;
    wxStringSet_t types_set;

    for(auto tag : tags) {
        if(tag->IsLocalVariable() || tag->IsParameter() || tag->IsMember()) {
            wxString type = tag->GetTypename();
            auto parts = wxStringTokenize(type, ":", wxTOKEN_STRTOK);
            for(const wxString& part : parts) {
                add_to_types_set(part, types_set, locals_set);
            }
            add_to_locals_set(tag->GetName(), locals_set, types_set);
        } else if(tag->IsMethod()) {
            const wxString& path = tag->GetPath();
            auto parts = wxStringTokenize(path, ":", wxTOKEN_STRTOK);
            if(!parts.empty()) {
                // the last part is the method name, we don't want to include it
                parts.pop_back();
            }
            for(const wxString& part : parts) {
                add_to_types_set(part, types_set, locals_set);
            }

            // we also want the method signature arguments
            wxString signature = tag->GetSignature();
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
        }
    }

    LOG_IF_TRACE { clDEBUG1() << "The following semantic tokens were found:" << endl; }
    LOG_IF_TRACE { clDEBUG1() << "Locals:" << locals_set << endl; }
    LOG_IF_TRACE { clDEBUG1() << "Types:" << types_set << endl; }

    const wxString& buffer = m_filesOpened[filepath];

    // collect all interesting tokens from the document
    SimpleTokenizer tokenizer(buffer);
    TokenWrapper token_wrapper;

    wxStringSet_t visited;

    std::unordered_map<wxString, TokenWrapper> variables;
    std::unordered_map<wxString, TokenWrapper> classes;
    std::unordered_map<wxString, TokenWrapper> functions;

    while(tokenizer.next(&token_wrapper.token)) {
        const auto& tok = token_wrapper.token;
        auto word = tok.to_string(buffer);
        if(!CompletionHelper::is_cxx_keyword(word)) {
            if(locals_set.count(word)) {
                token_wrapper.type = TYPE_VARIABLE;
                variables.insert({ word, token_wrapper });

            } else if(types_set.count(word)) {
                token_wrapper.type = TYPE_CLASS;
                classes.insert({ word, token_wrapper });

            } else if(tok.following_char1_is('(')) {
                // TOKEN(
                token_wrapper.type = TYPE_FUNCTION;
                functions.insert({ word, token_wrapper });

            } else if(tok.following_char1_is('.') || tok.following_char1_is('=')) {
                // TOKEN. or TOKEN =
                token_wrapper.type = TYPE_VARIABLE;
                variables.insert({ word, token_wrapper });

            } else if(tok.following_char1_is('-') && tok.following_char2_is('>')) {
                // TOKEN->
                token_wrapper.type = TYPE_VARIABLE;
                variables.insert({ word, token_wrapper });

            } else if(tok.following_char1_is(':') && tok.following_char2_is(':')) {
                // TOKEN::
                token_wrapper.type = TYPE_CLASS;
                classes.insert({ word, token_wrapper });

            } else if(tok.following_char1_is('&') || tok.following_char1_is('*') || tok.following_char1_is('<')) {
                // TOKEN< || TOKEN& || TOKEN*
                token_wrapper.type = TYPE_CLASS;
                classes.insert({ word, token_wrapper });
            }
        }
    }

    // remove all duplicate entries
    std::vector<TokenWrapper> tokens_vec;
    tokens_vec.reserve(functions.size() + variables.size() + classes.size());

    for(const auto& vt : classes) {
        if(functions.count(vt.first)) {
            functions.erase(vt.first);
        }
        if(variables.count(vt.first)) {
            variables.erase(vt.first);
        }
        tokens_vec.emplace_back(vt.second);
    }

    // remove all duplicates entries that exist both in variables and classes
    for(const auto& vt : functions) {
        if(variables.count(vt.first)) {
            variables.erase(vt.first);
        }
        tokens_vec.emplace_back(vt.second);
    }

    // add the variables
    for(const auto& vt : variables) {
        tokens_vec.emplace_back(vt.second);
    }

    // build the response
    size_t id = json["id"].toSize_t();
    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    auto result = build_result(response, id, cJSON_Object);

    std::vector<int> encoding;
    encoding.reserve(5 * tokens_vec.size());

    clDEBUG() << "Found" << tokens_vec.size() << "semantic tokens" << endl;
    LSPUtils::encode_semantic_tokens(tokens_vec, &encoding);
    result.addProperty("data", encoding);
    LOG_IF_TRACE { clDEBUG1() << response.format() << endl; }
    channel->write_reply(response);
}

// Request <-->
void ProtocolHandler::on_workspace_symbol(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    wxString query = json["params"]["query"].toString();
    wxArrayString parts = ::wxStringTokenize(query, " \t", wxTOKEN_STRTOK);

    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->GetTagsByPartialNames(parts, tags);

    // build the reply
    JSON root(cJSON_Object);
    auto response = root.toElement();
    auto result = build_result(response, id, cJSON_Array);

    auto symbols = LSPUtils::to_symbol_information_array(tags, false);
    for(const LSP::SymbolInformation& symbol : symbols) {
        result.arrayAppend(symbol.ToJSON(wxEmptyString));
    }
    channel->write_reply(response);
}

// Request <-->
void ProtocolHandler::on_document_symbol(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    wxUnusedVar(msg);
    wxUnusedVar(channel);

    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    LOG_IF_TRACE { clDEBUG1() << json.format() << endl; }
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG() << "textDocument/documentSymbol: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    // parse hte buffer
    std::vector<TagEntryPtr> tags;
    CTags::ParseBuffer(filepath, m_filesOpened[filepath], m_settings.GetCodeliteIndexer(), m_settings.GetMacroTable(),
                       tags);
    if(tags.empty()) {
        clDEBUG() << "no tags were found in file:" << filepath << endl;
    }

    // remove parameters from the list
    std::vector<TagEntryPtr> tags_no_parameters;
    tags_no_parameters.reserve(tags.size());
    for(auto tag : tags) {
        if(tag->IsParameter()) {
            continue;
        }
        tags_no_parameters.emplace_back(tag);
    }
    tags.swap(tags_no_parameters);

    // tags are sorted by line number, just wrap them in JSON and send them over to the client
    JSON root(cJSON_Object);
    auto response = root.toElement();
    auto result = build_result(response, id, cJSON_Array);

    auto symbols = LSPUtils::to_symbol_information_array(tags, true);
    for(const LSP::SymbolInformation& symbol : symbols) {
        result.arrayAppend(symbol.ToJSON(wxEmptyString));
    }
    channel->write_reply(response);
}

// Request <-->
void ProtocolHandler::on_document_signature_help(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    wxUnusedVar(channel);

    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    LOG_IF_TRACE { clDEBUG1() << json.format() << endl; }
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    LOG_IF_TRACE { clDEBUG1() << "textDocument/signatureHelp: for file" << filepath << endl; }

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    wxString last_word;
    CompletionHelper helper;

    wxString text =
        minimize_buffer(filepath, line, character, m_filesOpened[filepath], CompletionHelper::TRUNCATE_EXACT_POS);
    wxString expression = helper.get_expression(text, true, &last_word);

    std::vector<TagEntryPtr> candidates;
    std::vector<wxString> visible_scopes = update_additional_scopes_for_file(filepath);
    m_completer->word_complete(filepath, line + 1, expression, text, visible_scopes, true, candidates);

    // filter everything and just keep the methods tags
    std::vector<TagEntryPtr> matches;
    matches.reserve(candidates.size() * 5);
    for(TagEntryPtr match : candidates) {
        if(match->IsClass() || match->IsStruct()) {
            std::vector<TagEntryPtr> ctors;
            m_completer->get_class_constructors(match, ctors);
            matches.insert(matches.end(), ctors.begin(), ctors.end());
        } else if(match->IsMethod()) {
            matches.push_back(match);
        }
    }

    // sort the matches
    candidates.clear();
    m_completer->sort_tags(matches, candidates, true, {});

    clCallTipPtr tip(new clCallTip(candidates));
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
    channel->write_reply(response);
}

void ProtocolHandler::on_hover(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();
    LOG_IF_TRACE { clDEBUG1() << json.format() << endl; }

    std::vector<TagEntryPtr> tags;
    do_find_definition_tags(std::move(msg), channel, true, tags, nullptr);

    // format tip from tag
    std::vector<TagEntryPtr> function_tag_arr;
    wxStringSet_t visited;
    CompletionHelper helper;

    wxString tooltip;

    if(!tags.empty()) {
        auto first_tag = tags[0];
        tooltip << first_tag->GetKind() << " `" << first_tag->GetName() << "`\n";
        tooltip << "===\n";
        if(first_tag->IsMethod()) {
            tooltip.clear();
            if(!first_tag->GetScope().empty()) {
                if(first_tag->GetScope().StartsWith("__anon")) {
                    tooltip << "function: `" << first_tag->GetName() << "()` of `(anonymous scope)`\n";
                } else if(first_tag->GetScope().empty() || first_tag->GetScope() == "<global>") {
                    tooltip << "function: `" << first_tag->GetName() << "()`\n";
                } else if(first_tag->is_static()) {
                    tooltip << "class-method: `" << first_tag->GetPath() << "()`";
                } else {
                    tooltip << "instance-method: `" << first_tag->GetPath() << "()`";
                }
            } else {
                tooltip << "function `" << first_tag->GetName() << "()` ";
            }
            tooltip << "\n===\n";
            if(tags.size() == 1) {
                // fancy formatting
                auto args = helper.split_function_signature(first_tag->GetSignature(), nullptr, 0);
                tooltip << "-> `" << first_tag->GetTypename() << "`\n";
                if(!args.empty()) {
                    tooltip << "\n";
                }
                for(auto arg : args) {
                    tooltip << "- `" << arg << "`\n";
                }

                // build comment string
                if(!do_comments_exist_for_file(first_tag->GetFile())) {
                    update_comments_for_file(first_tag->GetFile());
                }

                wxString comment_string = get_comment(wxFileName(first_tag->GetFile()).GetFullPath(),
                                                      first_tag->GetLine() - 1, wxEmptyString);
                wxString doc_comment = helper.format_comment(nullptr, comment_string);

                if(!doc_comment.empty()) {
                    tooltip << "\n===\n";
                    tooltip << doc_comment << "\n";
                }
            } else {
                // overloading, use one liner per method
                for(TagEntryPtr tag : tags) {
                    if(tag->IsMethod()) {
                        if(visited.insert(helper.normalize_function(tag)).second) {
                            function_tag_arr.push_back(tag);
                        }
                    }
                }
            }

            tooltip << "```\n";
            bool long_functions = false;
            for(auto tag : function_tag_arr) {
                auto params = helper.split_function_signature(tag->GetSignature(), nullptr, 0);
                tooltip << tag->GetName() << "(";
                wxString params_str;

                // more than 4 function parameters, put them over multiple lines
                bool each_param_on_line = (params.size() > 4);
                if(each_param_on_line) {
                    tooltip << "\n  ";
                }

                // remember whether we had "long" functions here
                if(!long_functions) {
                    long_functions = each_param_on_line;
                }

                for(const auto& param : params) {
                    if(!params_str.empty()) {
                        if(each_param_on_line) {
                            params_str << ",\n  ";
                        } else {
                            params_str << ", ";
                        }
                    }
                    params_str << param;
                }
                tooltip << params_str << ") -> " << (tag->GetTypename().empty() ? wxString("()") : tag->GetTypename())
                        << "\n";
                tooltip << "{separator_placeholder}";
            }
            tooltip << "```";
            if(long_functions) {
                tooltip.Replace("{separator_placeholder}", "```\n```\n");
            } else {
                tooltip.Replace("{separator_placeholder}", "");
            }

        } else if(first_tag->IsMember()) {
            tooltip << "```\n";
            tooltip << first_tag->GetTypename() << " " << first_tag->GetScope() << "::" << first_tag->GetName() << "\n";
            tooltip << "```";
        } else if(first_tag->IsVariable()) {
            tooltip << "```\n";
            tooltip << first_tag->GetTypename() << " " << first_tag->GetName() << "\n";
            tooltip << "```";
        } else {
            tooltip << "```\n";
            tooltip << first_tag->GetPatternClean() << "\n";
            tooltip << "```";
        }
    }

    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    auto result = build_result(response, id, cJSON_Object);
    auto contents = result.AddObject("contents");
    contents.addProperty("kind", "markdown");
    contents.addProperty("value", tooltip);
    channel->write_reply(response);
}

size_t ProtocolHandler::do_find_definition_tags(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel,
                                                bool try_definition_first, std::vector<TagEntryPtr>& tags,
                                                wxString* file_match)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();
    tags.clear();

    LOG_IF_TRACE { clDEBUG1() << json.format() << endl; }
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG() << "textDocument/definition: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return 0;

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    CompletionHelper helper;
    wxString last_word;
    wxString text =
        minimize_buffer(filepath, line, character, m_filesOpened[filepath], CompletionHelper::TRUNCATE_COMPLETE_WORDS);
    wxString expression = helper.get_expression(text, false, &last_word);

    // get the last line
    wxString suffix;

    wxString text2 = helper.truncate_file_to_location(m_filesOpened[filepath], line, character,
                                                      CompletionHelper::TRUNCATE_COMPLETE_LINES);
    bool is_include_completion = false;

    if(file_match) {
        is_include_completion = helper.is_include_statement(text2, file_match, &suffix);
        if(is_include_completion) {
            clDEBUG() << "Is #include for file:" << *file_match << endl;
        } else {
            clDEBUG() << "Not #include. will call `find_definition` with expression:" << expression
                      << ", last_word=" << last_word << endl;
        }
    }

    if(file_match && is_include_completion) {
        clDEBUG() << "Found include file:" << *file_match << endl;
        for(const wxString& search_path : m_search_paths) {
            wxString full_path = search_path + "/" + *file_match;
            LOG_IF_TRACE { clDEBUG1() << "Trying path:" << full_path << endl; }
            wxFileName fn(full_path);
            fn.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
            if(fn.FileExists()) {
                // this is our match, construct a response and send it back
                *file_match = fn.GetFullPath();
                clDEBUG() << " --> Match found:" << *file_match << endl;
                break;
            }
        }
    } else {
        std::vector<wxString> visible_scopes = update_additional_scopes_for_file(filepath);
        m_completer->find_definition(filepath, line + 1, expression, text, visible_scopes, tags);
        clDEBUG() << " --> Match found:" << tags.size() << "matches" << endl;
        clDEBUG() << tags << endl;
    }
    return tags.size();
}

void ProtocolHandler::do_definition(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel, bool try_definition_first)
{
    std::vector<TagEntryPtr> tags;
    wxString file_match;
    size_t id = msg->toElement()["id"].toSize_t();
    do_find_definition_tags(std::move(msg), channel, try_definition_first, tags, &file_match);

    // build the result
    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    response.addProperty("id", id);
    response.addProperty("jsonrpc", "2.0");
    auto result = response.AddArray("result");

    if(!file_match.empty()) {
        // prepare a single file match result
        auto match = result.AddObject(wxEmptyString);
        LSP::Range range;
        range.SetStart({ 0, 0 }).SetEnd({ 0, 0 });
        match.append(range.ToJSON("range"));
        match.addProperty("uri", FileUtils::FilePathToURI(file_match));
    } else {
        // add all the results
        CompletionHelper helper;
        for(TagEntryPtr tag : tags) {
            clDEBUG() << " --> Adding tag:" << tag->GetName() << tag->GetFile() << ":" << tag->GetLine() << endl;
            auto match = result.AddObject(wxEmptyString);
            // we can only provide line number...
            LSP::Range range;
            range.SetStart({ tag->GetLine() - 1, 0 }).SetEnd({ tag->GetLine() - 1, 0 });
            match.append(range.ToJSON("range"));
            match.addProperty("uri", FileUtils::FilePathToURI(tag->GetFile()));

            // we will use this to highlight the selection
            match.addProperty("name", tag->GetName());

            // the pattern property is used to display "multi-selection" dialog to the user
            if(tag->IsMethod()) {
                match.addProperty("pattern", helper.normalize_function(tag, 0));
            } else {
                match.addProperty("pattern", tag->GetKind() + " " + tag->GetName());
            }
        }
    }
    channel->write_reply(response);
}

void ProtocolHandler::on_declaration(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    do_definition(std::move(msg), channel, false);
}

void ProtocolHandler::on_definition(std::unique_ptr<JSON>&& msg, Channel::ptr_t channel)
{
    do_definition(std::move(msg), channel, true);
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
    if(read_file_list(m_settings_folder, m_settings, files) == 0) {
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
    for(const auto& file : m_settings.GetSearchPath()) {
        wxFileName fn(file, wxEmptyString);
        fn.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
        wxString path = fn.GetPath();
        if(unique_paths.count(path) == 0) {
            m_search_paths.Add(path);
            unique_paths.insert(path);
        }
    }
}

void ProtocolHandler::parse_buffer_for_includes_and_using_namespace(const wxString& filepath, const wxString& buffer)
{
    ParsedFileInfo entry;
    m_file_scanner.scan_buffer(filepath, buffer, m_search_paths, &entry.included_files, &entry.using_namespace);

    m_parsed_files_info.erase(filepath);
    m_parsed_files_info.insert({ filepath, entry });
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
    std::deque<wxString> files_to_parse{ files.begin(), files.end() };

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

size_t ProtocolHandler::get_includes_recrusively(const wxString& filepath, wxStringSet_t* output)
{
    std::deque<wxString> Q;
    Q.push_back(filepath);
    output->insert(filepath);

    wxStringSet_t visited;
    while(!Q.empty()) {
        wxString filepath = Q.front();
        Q.pop_front();
        output->insert(filepath);

        if(!visited.insert(filepath).second)
            continue;

        if(m_parsed_files_info.count(filepath) == 0)
            continue;

        // append all its children to the vector
        const auto& include_files = m_parsed_files_info[filepath].included_files;
        Q.insert(Q.end(), include_files.begin(), include_files.end());
    }
    return output->size();
}

wxStringSet_t ProtocolHandler::setdiff(const wxStringSet_t& a, const wxStringSet_t& b)
{
    wxStringSet_t diff;
    diff.reserve(a.size());

    // go over the smaller one collect all
    for(const auto& item : a) {
        if(b.count(item) == 0) {
            diff.insert(item);
        }
    }
    return diff;
}
