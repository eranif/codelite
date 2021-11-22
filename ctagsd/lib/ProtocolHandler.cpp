#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "LSP/LSPEvent.h"
#include "ProtocolHandler.hpp"
#include "Settings.hpp"
#include "clFilesCollector.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "tags_storage_sqlite3.h"
#include <iostream>
#include <wx/filesys.h>

// TODO::
// 1. CodeLite should generate settings.json file

namespace
{
wxStopWatch sw;
FileLogger& operator<<(FileLogger& logger, const vector<TagEntryPtr>& tags)
{
    wxString s;
    s << "[";
    for(const auto& tag : tags) {
        s << tag->GetDisplayName() << ", ";
    }
    if(s.EndsWith(", ")) {
        s.RemoveLast(2);
    }
    s << "]";
    logger.Append(s, logger.GetRequestedLogLevel());
    return logger;
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

JSONItem ProtocolHandler::build_result(JSONItem& reply, size_t id)
{
    reply.addProperty("id", id);
    reply.addProperty("jsonrpc", "2.0");
    return reply.AddObject("result");
}

void ProtocolHandler::parse_files(Channel& channel)
{
    clDEBUG() << "Searching for files to parse..." << endl;
    clFilesScanner scanner;
    scanner.Scan(m_root_folder, m_files, m_settings.GetFileMask(), wxEmptyString, m_settings.GetIgnoreSpec());
    clDEBUG() << "Found" << m_files.size() << "files" << endl;
    send_log_message(wxString() << _("Generating `ctags` file for: ") << m_files.size() << _(" files"), LSP_LOG_INFO,
                     channel);

    start_timer();

    if(!CTags::Generate(m_files, m_settings_folder, m_settings.GetCodeliteIndexer())) {
        send_log_message(_("Failed to generate `ctags` file"), LSP_LOG_ERROR, channel);
        clERROR() << "Failed to generate ctags file!" << endl;
        return;
    }

    send_log_message(wxString() << _("Success (") << stop_timer() << ")", LSP_LOG_INFO, channel);

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
    ITagsStoragePtr db(new TagsStorageSQLite());

    wxFileName dbfile(m_settings_folder, "tags.db");
    db->OpenDatabase(dbfile);

    /// TODO: scan the database and filter all non modified files since last parsing was done
    send_log_message(_("Creating symbols database..."), LSP_LOG_INFO, channel);

    start_timer();
    db->Begin();
    size_t tagsCount = 0;
    while(ttp) {
        ++tagsCount;

        // Send notification to the main window with our progress report
        db->Store(ttp, {}, false);
        if(db->InsertFileEntry(curfile, (int)time(NULL)) == TagExist) {
            db->UpdateFileEntry(curfile, (int)time(NULL));
        }

        if((tagsCount % 1000) == 0) {
            // Commit what we got so far
            db->Commit();
            // Start a new transaction
            db->Begin();
        }
        ttp = ctags.GetTagsTreeForFile(curfile);
    }

    // Commit whats left
    db->Commit();
    send_log_message(wxString() << _("Success (") << stop_timer() << ")", LSP_LOG_INFO, channel);
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
    auto result = build_result(response, id);

    auto capabilities = result.AddObject("capabilities");
    capabilities.addProperty("completionProvider", true);
    capabilities.addProperty("declarationProvider", true);
    capabilities.addProperty("definitionProvider", true);
    capabilities.addProperty("documentSymbolProvider", true);
    capabilities.addProperty("hoverProvider", true);

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

    parse_files(channel);

    TagsManagerST::Get()->CloseDatabase();
    TagsManagerST::Get()->OpenDatabase(wxFileName(m_settings_folder, "tags.db"));
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

    clDEBUG() << "didOpen: caching new content for file:" << filepath << endl;
    m_filesOpened.erase(filepath);
    m_filesOpened.insert({ filepath, json["params"]["textDocument"]["text"].toString() });
}

// Notification -->
void ProtocolHandler::on_did_close(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(channel);
    auto json = msg->toElement();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();
    m_filesOpened.erase(filepath);
}

// Notification -->
void ProtocolHandler::on_did_change(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(channel);
    // keep the file content in the internal map
    auto json = msg->toElement();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    clDEBUG() << "didChange: caching new content for file:" << filepath << endl;
    m_filesOpened.erase(filepath);
    m_filesOpened.insert({ filepath, json["params"]["contentChanges"][0]["text"].toString() });
}

// Request <-->
void ProtocolHandler::on_completion(unique_ptr<JSON>&& msg, Channel& channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    if(m_filesOpened.count(filepath) == 0) {
        // check if this file exists on the file system -> and load it instead of complaining about it
        wxString file_content;
        if(!wxFileExists(filepath) || !FileUtils::ReadFileContent(filepath, file_content)) {
            clWARNING() << "File:" << filepath << "is not opened" << endl;
            send_log_message(wxString() << _("`textDocument/completion` error. File: ") << filepath
                                        << _(" is not opened on the server"),
                             LSP_LOG_WARNING, channel);
            return;
        }

        // update the cache
        clDEBUG() << "Updated cache with non existing file:" << filepath << "is not opened" << endl;
        m_filesOpened.insert({ filepath, file_content });
    }

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    // get the expression at this given position
    wxString last_word;
    CompletionHelper helper;
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character);
    wxString expression = helper.get_expression(text, &last_word);

    clDEBUG() << "resolving expression:" << expression << endl;

    bool is_trigger_char =
        !expression.empty() && (expression.Last() == '>' || expression.Last() == ':' || expression.Last() == '.');
    bool is_function_calltip = !expression.empty() && expression.Last() == '(';

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
        JSON root(cJSON_Object);
        JSONItem response = root.toElement();
        auto result = build_result(response, id);

        result.addProperty("isIncomplete", false);
        auto items = result.AddArray("items");
        // send them over the client
        for(auto tag : candidates) {
            auto item = items.AddObject(wxEmptyString);
            if(!tag->GetComment().empty()) {
                auto doc = item.AddObject("documentation");
                doc.addProperty("kind", "plaintext");
                doc.addProperty("value", tag->GetComment());
            }

            item.addProperty("label", tag->GetDisplayName());
            item.addProperty("filterText", tag->GetName());
            item.addProperty("insertText", tag->GetName());
            item.addProperty("detail", tag->GetReturnValue());

            // set the kind
            using LSP::CompletionItem;
            CompletionItem::eCompletionItemKind kind = CompletionItem::kKindVariable;
            if(tag->IsMethod() || tag->IsConstructor()) {
                kind = CompletionItem::kKindFunction;
            } else if(tag->IsClass()) {
                kind = CompletionItem::kKindClass;
            } else if(tag->IsStruct()) {
                kind = CompletionItem::kKindStruct;
            } else if(tag->IsLocalVariable()) {
                kind = CompletionItem::kKindVariable;
            } else if(tag->IsTypedef()) {
                kind = CompletionItem::kKindTypeParameter;
            } else if(tag->IsMacro()) {
                kind = CompletionItem::kKindTypeParameter;
            } else if(tag->GetKind() == "namespace") {
                kind = CompletionItem::kKindModule;
            } else if(tag->GetKind() == "union") {
                kind = CompletionItem::kKindStruct;
            } else if(tag->GetKind() == "enum") {
                kind = CompletionItem::kKindEnum;
            } else if(tag->GetKind() == "enumerator") {
                kind = CompletionItem::kKindEnumMember;
            }
            item.addProperty("kind", static_cast<int>(kind));
        }
        channel.write_reply(response.format(false));
    }
}
