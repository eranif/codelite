#include "ProtocolHandler.hpp"
#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "LSP/LSPEvent.h"
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
FileLogger& operator<<(FileLogger& logger, const wxStringMap_t& m)
{
    wxString s;
    s << "{";
    for(const auto& vt : m) {
        s << "{ " << vt.first << ", " << vt.second << " },";
    }
    if(s.EndsWith(",")) {
        s.RemoveLast();
    }
    s << "}";
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

void ProtocolHandler::build_result(JSONItem& reply, size_t id)
{
    reply.addProperty("id", id);
    reply.addProperty("jsonrpc", "2.0");
    reply.AddObject("result");
}

void ProtocolHandler::parse_files(Channel& channel)
{
    clDEBUG() << "Searching for files to parse..." << endl;
    clFilesScanner scanner;
    scanner.Scan(m_root_folder, m_files, m_file_mask, wxEmptyString, m_ignore_spec);
    clDEBUG() << "Found" << m_files.size() << "files" << endl;
    send_log_message(wxString() << _("Generating `ctags` file for: ") << m_files.size() << _(" files"), LSP_LOG_INFO,
                     channel);

    start_timer();

    if(!CTags::Generate(m_files, m_settings_folder, m_codelite_indexer)) {
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

    if(!m_db) {
        m_db.Reset(new TagsStorageSQLite());

        wxFileName dbfile(m_settings_folder, "tags.db");
        m_db->OpenDatabase(dbfile);
    }

    /// TODO: scan the database and filter all non modified files since last parsing was done
    send_log_message(_("Creating symbols database..."), LSP_LOG_INFO, channel);

    start_timer();
    m_db->Begin();
    size_t tagsCount = 0;
    while(ttp) {
        ++tagsCount;

        // Send notification to the main window with our progress report
        m_db->Store(ttp, {}, false);
        if(m_db->InsertFileEntry(curfile, (int)time(NULL)) == TagExist) {
            m_db->UpdateFileEntry(curfile, (int)time(NULL));
        }

        if((tagsCount % 1000) == 0) {
            // Commit what we got so far
            m_db->Commit();
            // Start a new transaction
            m_db->Begin();
        }
        ttp = ctags.GetTagsTreeForFile(curfile);
    }

    // Commit whats left
    m_db->Commit();
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
    JSONItem resposne = root.toElement();
    build_result(resposne, id);

    auto capabilities = resposne.AddObject("capabilities");
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

    JSON config_file(fn_config_file);
    if(!config_file.isOk()) {
        clWARNING() << "Could not locate configuration file:" << fn_config_file << endl;
    } else {
        auto config = config_file.toElement();
        m_search_path = config["search_path"].toArrayString();
        m_tokens = config["tokens"].toStringMap();
        m_file_mask = config["file_mask"].toString(m_file_mask);
        m_ignore_spec = config["ignore_spec"].toString(m_ignore_spec);
        m_codelite_indexer = config["codelite_indexer"].toString();
    }

    clDEBUG() << "search path:" << m_search_path << endl;
    clDEBUG() << "tokens:" << m_tokens << endl;
    clDEBUG() << "file_mask:" << m_file_mask << endl;
    clDEBUG() << "codelite-indexer:" << m_codelite_indexer << endl;

    parse_files(channel);
    channel.write_reply(resposne.format(false));
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
    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    if(m_filesOpened.count(filepath) == 0) {
        clWARNING() << "File:" << filepath << "is not opened" << endl;
        send_log_message(wxString() << _("`textDocument/completion` error. File:") << filepath
                                    << _("is not opened in the server"),
                         LSP_LOG_WARNING, channel);
        return;
    }

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    // get the expression at this given position
    CompletionHelper helper;
    wxString expression = helper.get_expression_from_location(m_filesOpened[filepath], line, character, m_db);
}
