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

using LSP::CompletionItem;
using LSP::eSymbolKind;

unordered_map<wxString, vector<TagEntry>> ProtocolHandler::m_tags_cache;

namespace
{
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
} // namespace

ProtocolHandler::ProtocolHandler() {}

ProtocolHandler::~ProtocolHandler() { m_parse_thread.stop(); }

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

void ProtocolHandler::parse_file_async(const wxString& filepath, const wxString& file_content,
                                       const wxString& settings_folder, const wxString& indexer_path)
{
    // write the content into a temporary file
    clTempFile tempfile{ "cpp" };
    tempfile.Write(file_content);

    // prepare a list of files (size=1)
    wxArrayString files;
    files.Add(tempfile.GetFullPath());

    // force the output to the actual file name
    parse_files(files, settings_folder, indexer_path, filepath);
}

void ProtocolHandler::parse_files(const wxArrayString& file_list, const wxString& settings_folder,
                                  const wxString& indexer_path, const wxString& alternate_filename)
{
    clDEBUG() << "Parsing" << file_list.size() << "files" << endl;
    clDEBUG() << "Removing un-modified and unwanted files..." << endl;
    // create/open db
    wxFileName dbfile(settings_folder, "tags.db");
    if(!dbfile.FileExists()) {
        clDEBUG() << dbfile << "does not exist, will create it" << endl;
    }
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    wxArrayString files_to_parse = file_list;
    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(files_to_parse, db);

    clDEBUG() << "There are total of" << files_to_parse.size() << "files that require parsing" << endl;
    clDEBUG() << "Generating ctags file..." << endl;
    if(!CTags::Generate(files_to_parse, settings_folder, indexer_path)) {
        clERROR() << "Failed to generate ctags file!" << endl;
        return;
    }
    clDEBUG() << "Success" << endl;

    // update the DB
    wxStringSet_t updatedFiles;
    CTags ctags(settings_folder);
    if(!ctags.IsOpened()) {
        clWARNING() << "Failed to open ctags file under:" << settings_folder << endl;
        return;
    }

    wxString curfile;

    // build the database
    vector<TagEntry> tags;
    TagTreePtr ttp = ctags.GetTagsTreeForFile(curfile, tags, alternate_filename);
    clDEBUG() << "Updating symbols database..." << endl;
    db->Begin();
    size_t tagsCount = 0;
    time_t update_time = time(nullptr);
    while(ttp) {
        // cache the symbols
        ProtocolHandler::cache_set_document_symbols(alternate_filename, move(tags));
        ++tagsCount;

        // Send notification to the main window with our progress report
        db->DeleteByFileName({}, alternate_filename.empty() ? curfile : alternate_filename, false);
        db->Store(ttp, {}, false);

        if((tagsCount % 1000) == 0) {
            // Commit what we got so far
            db->Commit();
            // Start a new transaction
            db->Begin();
        }
        tags.clear();
        ttp = ctags.GetTagsTreeForFile(curfile, tags, alternate_filename);
    }

    // update the files table in the database
    // we do this here, since some files might not yield tags
    // but we still want to mark them as "parsed"
    for(const wxString& file : files_to_parse) {
        if(db->InsertFileEntry(file, (int)update_time) == TagExist) {
            db->UpdateFileEntry(file, (int)update_time);
        }
    }

    // Commit whats left
    db->Commit();
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
    auto where = find_if(additional_scopes.begin(), additional_scopes.end(),
                         [](const wxString& scope) { return scope.empty(); });
    if(where == additional_scopes.end()) {
        // no globl scope, add it
        additional_scopes.push_back(wxEmptyString);
    }
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
    // start the parser thread
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
    if(read_file_list(files) == 0) {
        // try the scan dir
        scan_dir(m_root_folder, m_settings, files);
    }
    build_search_path();

    // build a list of files to parse (including all include statements)
    files = get_files_to_parse(files);

    send_log_message(_("Updating symbols database..."), LSP_LOG_INFO, channel);
    parse_files(files, m_settings_folder, m_settings.GetCodeliteIndexer());
    send_log_message(_("Success"), LSP_LOG_INFO, channel);

    TagsManagerST::Get()->CloseDatabase();
    TagsManagerST::Get()->OpenDatabase(wxFileName(m_settings_folder, "tags.db"));
    TagsManagerST::Get()->GetDatabase()->SetSingleSearchLimit(m_settings.GetLimitResults());
    m_completer.reset(new CxxCodeCompletion(TagsManagerST::Get()->GetDatabase()));
    m_completer->set_macros_table(m_settings.GetTokens());
    m_completer->set_types_table(m_settings.GetTypes());
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
    parse_file_async(filepath, file_content, m_settings_folder, m_settings.GetCodeliteIndexer());

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
    cache_erase_document_symbols(filepath);
}

// Notification -->
void ProtocolHandler::on_did_change(unique_ptr<JSON>&& msg, Channel& channel)
{
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

    if(line_count_before != line_count_after) {
        // parse this file (async)
        m_parse_thread.queue_parse_request(filepath, file_content);

        // update using namespace cache
        parse_file_for_includes_and_using_namespace(filepath);
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
        clDEBUG1() << "Minimized file into:" << endl;
        clDEBUG1() << text << endl;
    } else {
        // use the entire file content
        text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character,
                                                CompletionHelper::TRUNCATE_EXACT_POS);
        clDEBUG1() << "Unable to minimize the buffer, using the complete buffer" << endl;
    }
    return text;
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

    int line = json["params"]["position"]["line"].toInt(0);
    int character = json["params"]["position"]["character"].toInt(0);

    clDEBUG() << "completion requested at:" << filepath << ":" << line << ":" << character << endl;

    wxString last_word;
    CompletionHelper helper;
    wxString suffix;
    const wxString& full_buffer = m_filesOpened[filepath];
    bool is_include_completion = false;
    wxString file_name;
    vector<TagEntryPtr> candidates;

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
        wxString minimized_buffer = minimize_buffer(filepath, line, character, full_buffer);

        clDEBUG1() << "Success" << endl;
        clDEBUG1() << "Getting expression..." << endl;
        wxString expression = helper.get_expression(minimized_buffer, false, &last_word);

        clDEBUG1() << "Success" << endl;
        clDEBUG() << "resolving expression:" << expression << endl;

        bool is_trigger_char =
            !expression.empty() && (expression.Last() == '>' || expression.Last() == ':' || expression.Last() == '.');
        bool is_function_calltip = !expression.empty() && expression.Last() == '(';
        vector<wxString> visible_scopes = update_additional_scopes_for_file(filepath);

        if(is_trigger_char) {
            // ----------------------------------
            // code completion
            // ----------------------------------
            clDEBUG() << "CodeComplete expression:" << expression << endl;
            CxxRemainder remainder;

            m_completer->set_text(minimized_buffer, filepath, line);
            TagEntryPtr resolved = m_completer->code_complete(expression, visible_scopes, &remainder);
            if(resolved) {
                clDEBUG() << "resolved into:" << resolved->GetPath() << endl;
                clDEBUG() << "filter:" << remainder.filter << endl;
                m_completer->get_completions(resolved, remainder.operand_string, remainder.filter, candidates,
                                             visible_scopes);
            }
            clDEBUG() << "Number of completion entries:" << candidates.size() << endl;
            clDEBUG1() << candidates << endl;
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
            item.addProperty("detail", tag->IsMethod() ? m_completer->get_return_value(tag) : tag->GetReturnValue());

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

    // reparse the file
    parse_files(files, m_settings_folder, m_settings.GetCodeliteIndexer());

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

    // collect all interesting tokens from the document
    SimpleTokenizer tokenizer(buffer);
    TokenWrapper token_wrapper;

    wxStringSet_t visited;

    unordered_map<wxString, TokenWrapper> variables;
    unordered_map<wxString, TokenWrapper> classes;
    unordered_map<wxString, TokenWrapper> functions;

    while(tokenizer.next(&token_wrapper.token)) {
        const auto& tok = token_wrapper.token;
        wxString word = tok.to_string(buffer);
        if(word == "DebuggerMgr") {
            int br;
            br++;
            wxUnusedVar(br);
        }

        if(!CompletionHelper::is_cxx_keyword(word)) {
            if(tok.following_char1_is('(')) {
                // TOKEN(
                token_wrapper.type = TYPE_FUNCTION;
                functions.insert({ word, token_wrapper });

            } else if(tok.following_char1_is('.')) {
                // TOKEN.
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

            } else if(locals_set.count(word)) {
                // we know that this one is a variable
                token_wrapper.type = TYPE_VARIABLE;
                variables.insert({ word, token_wrapper });

            } else if(types_set.count(word)) {
                // A typename
                token_wrapper.type = TYPE_CLASS;
                classes.insert({ word, token_wrapper });
            }
        }
    }

    // remove all duplicate entries
    vector<TokenWrapper> tokens_vec;
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
    clDEBUG() << "textDocument/documentSymbol: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel, id))
        return;

    // parse the file and return the symbols
    clDEBUG() << "Fetching symbols from cache for file:" << filepath << endl;
    vector<TagEntry> tags;
    cache_get_document_symbols(filepath, tags);

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

    wxString text =
        minimize_buffer(filepath, line, character, m_filesOpened[filepath], CompletionHelper::TRUNCATE_EXACT_POS);
    wxString expression = helper.get_expression(text, true, &last_word);

    vector<TagEntryPtr> candidates;
    vector<wxString> visible_scopes = update_additional_scopes_for_file(filepath);
    m_completer->word_complete(filepath, line + 1, expression, text, visible_scopes, true, candidates);
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
    wxString text =
        minimize_buffer(filepath, line, character, m_filesOpened[filepath], CompletionHelper::TRUNCATE_COMPLETE_WORDS);
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
        vector<wxString> visible_scopes = update_additional_scopes_for_file(filepath);
        TagEntryPtr match = m_completer->find_definition(filepath, line + 1, expression, text, visible_scopes);
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

size_t ProtocolHandler::get_includes_recrusively(const wxString& filepath, wxStringSet_t* output)
{
    deque<wxString> Q;
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

static wxCriticalSection cs;

void ProtocolHandler::cache_set_document_symbols(const wxString& filepath, vector<TagEntry>&& tags)
{
    wxCriticalSectionLocker locker{ cs };
    if(m_tags_cache.count(filepath)) {
        m_tags_cache.erase(filepath);
    }
    m_tags_cache.insert({ filepath, move(tags) });
}

size_t ProtocolHandler::cache_get_document_symbols(const wxString& filepath, vector<TagEntry>& tags)
{
    wxCriticalSectionLocker locker{ cs };
    if(m_tags_cache.count(filepath) == 0) {
        return 0;
    }
    tags = m_tags_cache.find(filepath)->second;
    return tags.size();
}

void ProtocolHandler::cache_erase_document_symbols(const wxString& filepath)
{
    wxCriticalSectionLocker locker{ cs };
    if(m_tags_cache.count(filepath)) {
        m_tags_cache.erase(filepath);
    }
}
