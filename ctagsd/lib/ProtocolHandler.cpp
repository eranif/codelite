#include "ProtocolHandler.hpp"
#include "CTags.hpp"
#include "CompletionHelper.hpp"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
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
#include "tags_storage_sqlite3.h"

#include <iostream>
#include <wx/filesys.h>

using LSP::CompletionItem;
using LSP::eSymbolKind;

namespace
{
wxStopWatch sw;
FileLogger& operator<<(FileLogger& logger, const TokenWrapper& wrapper)
{
    wxString s;
    s << "{" << (wrapper.type == TYPE_CLASS ? "class" : "variable") << ", " << wrapper.token.line() << ", "
      << wrapper.token.column() << ", " << wrapper.token.length() << "}";
    logger.Append(s, logger.GetRequestedLogLevel());
    return logger;
}

// FileLogger& operator<<(FileLogger& logger, clCallTipPtr ct)
// {
//     wxString s;
//     if(!ct) {
//         s << "{clCallTipPtr: nullptr}";
//     } else {
//         s << ct->All();
//     }
//     logger.Append(s, logger.GetRequestedLogLevel());
//     return logger;
// }

FileLogger& operator<<(FileLogger& logger, const vector<TokenWrapper>& arr)
{
    for(const auto& d : arr) {
        logger << d << endl;
    }
    return logger;
}

FileLogger& operator<<(FileLogger& logger, const TagEntry& tag)
{
    wxString s;
    s << tag.GetKind() << ": " << tag.GetDisplayName();
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

void scan_dir(const wxString& dir, const CTagsdSettings& settings, wxArrayString& files)
{
    clDEBUG() << "Searching for files to..." << endl;

    clFilesScanner scanner;
    wxArrayString exclude_folders_arr = ::wxStringTokenize(settings.GetIgnoreSpec(), ";", wxTOKEN_STRTOK);
    scanner.Scan(dir, files, settings.GetFileMask(), wxEmptyString, settings.GetIgnoreSpec());
}

CompletionItem::eCompletionItemKind get_completion_kind(const TagEntry& tag)
{
    CompletionItem::eCompletionItemKind kind = CompletionItem::kKindVariable;
    if(tag.IsMethod()) {
        kind = CompletionItem::kKindFunction;
    } else if(tag.IsConstructor()) {
        kind = CompletionItem::kKindClass;
    } else if(tag.IsClass()) {
        kind = CompletionItem::kKindClass;
    } else if(tag.IsStruct()) {
        kind = CompletionItem::kKindStruct;
    } else if(tag.IsLocalVariable()) {
        kind = CompletionItem::kKindVariable;
    } else if(tag.IsTypedef()) {
        kind = CompletionItem::kKindTypeParameter;
    } else if(tag.IsMacro()) {
        kind = CompletionItem::kKindTypeParameter;
    } else if(tag.GetKind() == "namespace") {
        kind = CompletionItem::kKindModule;
    } else if(tag.GetKind() == "union") {
        kind = CompletionItem::kKindStruct;
    } else if(tag.GetKind() == "enum") {
        kind = CompletionItem::kKindEnum;
    } else if(tag.GetKind() == "enumerator") {
        kind = CompletionItem::kKindEnumMember;
    }
    return kind;
}

// eSymbolKind get_symbol_kind(const TagEntry& tag)
// {
//     eSymbolKind kind = eSymbolKind::kSK_Variable;
//     if(tag.IsMethod()) {
//         kind = eSymbolKind::kSK_Method;
//     } else if(tag.IsConstructor()) {
//         kind = eSymbolKind::kSK_Class;
//     } else if(tag.IsClass()) {
//         kind = eSymbolKind::kSK_Class;
//     } else if(tag.IsStruct()) {
//         kind = eSymbolKind::kSK_Struct;
//     } else if(tag.IsLocalVariable()) {
//         kind = eSymbolKind::kSK_Variable;
//     } else if(tag.IsTypedef()) {
//         kind = eSymbolKind::kSK_TypeParameter;
//     } else if(tag.IsMacro()) {
//         kind = eSymbolKind::kSK_TypeParameter;
//     } else if(tag.GetKind() == "namespace") {
//         kind = eSymbolKind::kSK_Module;
//     } else if(tag.GetKind() == "union") {
//         kind = eSymbolKind::kSK_Struct;
//     } else if(tag.GetKind() == "enum") {
//         kind = eSymbolKind::kSK_Enum;
//     } else if(tag.GetKind() == "enumerator") {
//         kind = eSymbolKind::kSK_EnumMember;
//     }
//     return kind;
// }

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
    clDEBUG() << "Removing un-modified files and unwanted files..." << endl;
    // create/open db
    wxFileName dbfile(m_settings_folder, "tags.db");

    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);
    if(initial_parse && !db->CheckIntegrity()) {
        // delete the database
        db->RecreateDatabase();
    }

    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(files, db);
    start_timer();

    // Now that we got the list of files - include all the "#include" statements
    fcFileOpener::Get()->ClearResults();
    fcFileOpener::Get()->ClearSearchPath();

    for(const auto& search_path : m_settings.GetSearchPath()) {
        const wxCharBuffer path = search_path.mb_str(wxConvUTF8);
        fcFileOpener::Get()->AddSearchPath(path.data());
    }

    remove_binary_files(files);
    clDEBUG() << "Adding #include'ed files..." << endl;
    for(size_t i = 0; i < files.size(); i++) {
        const wxCharBuffer cfile = files[i].mb_str(wxConvUTF8);
        crawlerScan(cfile.data());
    }

    wxStringSet_t unique_files{ files.begin(), files.end() };

    const auto& result_set = fcFileOpener::Get()->GetResults();
    for(const wxString& file : result_set) {
        wxFileName fn(file);
        fn.MakeAbsolute(m_root_folder);
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
    while(ttp) {
        ++tagsCount;

        // Send notification to the main window with our progress report
        db->DeleteByFileName({}, curfile, false);
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
    if(channel) {
        send_log_message(wxString() << _("Success (") << stop_timer() << ")", LSP_LOG_INFO, *channel);
    }
    clDEBUG() << "Success" << endl;
}

bool ProtocolHandler::ensure_file_content_exists(const wxString& filepath, Channel& channel)
{
    if(m_filesOpened.count(filepath) == 0) {
        // check if this file exists on the file system -> and load it instead of complaining about it
        wxString file_content;
        if(!wxFileExists(filepath) || !FileUtils::ReadFileContent(filepath, file_content)) {
            clWARNING() << "File:" << filepath << "is not opened" << endl;
            send_log_message(wxString() << _("`textDocument/completion` error. File: ") << filepath
                                        << _(" is not opened on the server"),
                             LSP_LOG_WARNING, channel);
            return false;
        }

        // update the cache
        clDEBUG() << "Updated cache with non existing file:" << filepath << "is not opened" << endl;
        m_filesOpened.insert({ filepath, file_content });
    }
    return true;
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

    // build the workspace file list
    wxArrayString files;
    wxFileName file_list(m_settings_folder, "file_list.txt");
    if(file_list.FileExists()) {
        wxString file_list_content;
        FileUtils::ReadFileContent(file_list, file_list_content);
        files = ::wxStringTokenize(file_list_content, "\n", wxTOKEN_STRTOK);
    } else {
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
    update_comments_for_file(filepath, file_content);
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
    m_comments_cache.erase(filepath);
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
    update_comments_for_file(filepath, file_content);
    m_filesOpened.insert({ filepath, file_content });
}

// Request <-->
void ProtocolHandler::on_completion(unique_ptr<JSON>&& msg, Channel& channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    wxString filepath = json["params"]["textDocument"]["uri"].toString();
    filepath = wxFileSystem::URLToFileName(filepath).GetFullPath();

    if(!ensure_file_content_exists(filepath, channel))
        return;

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
        for(auto tag : candidates) {
            wxFileName fn(tag->GetFile());
            if(!do_comments_exist_for_file(fn.GetFullPath())) {
                update_comments_for_file(fn.GetFullPath());
            }
        }

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
            wxString doc_comment = tag->FormatComment();
            auto documentation = item.AddObject("documentation");
            documentation.addProperty("kind", "markdown");
            documentation.addProperty("value", doc_comment);

            item.addProperty("label", tag->GetDisplayName());
            item.addProperty("filterText", tag->GetName());
            item.addProperty("insertText", tag->GetName());
            item.addProperty("detail", tag->GetReturnValue());

            // set the kind
            CompletionItem::eCompletionItemKind kind = get_completion_kind(*tag.Get());
            item.addProperty("kind", static_cast<int>(kind));
        }
        channel.write_reply(response.format(false));
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

    // re-parse the file
    wxArrayString files;
    files.Add(filepath);
    parse_files(files, nullptr, false);

    if(TagsManagerST::Get()->GetDatabase()) {
        TagsManagerST::Get()->ClearTagsCache();
    }
}

// Request <-->
void ProtocolHandler::on_semantic_tokens(unique_ptr<JSON>&& msg, Channel& channel)
{
    JSONItem json = msg->toElement();
    clDEBUG() << json.format() << endl;
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG1() << "textDocument/semanticTokens/full: for file" << filepath << endl;

    // use CTags to gather local variables
    wxString tmpdir = clStandardPaths::Get().GetTempDir();
    vector<TagEntry> tags =
        CTags::Run(filepath, tmpdir, "--excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+pfl --C++-kinds=+pfl ",
                   m_settings.GetCodeliteIndexer());

    clDEBUG() << "File tags:" << tags.size() << endl;

    wxStringSet_t locals_set;
    wxStringSet_t types_set;
    for(const TagEntry& tag : tags) {
        if(tag.IsLocalVariable()) {
            wxString type = tag.GetLocalType();
            auto parts = wxStringTokenize(type, ":", wxTOKEN_STRTOK);
            for(const wxString& part : parts) {
                types_set.insert(part);
            }
            locals_set.insert(tag.GetName());
        } else if(tag.IsMethod()) {
            const wxString& path = tag.GetPath();
            auto parts = wxStringTokenize(path, ":", wxTOKEN_STRTOK);
            if(!parts.empty()) {
                // the last part is the method name, we don't want to include it
                parts.pop_back();
            }
            for(const wxString& part : parts) {
                types_set.insert(part);
            }

            // we also want the method signature arguments
            wxString signature = tag.GetSignature();
            CxxVariableScanner scanner(signature, eCxxStandard::kCxx11, {}, true);
            auto functionArgs = scanner.ParseFunctionArguments();
            for(const auto& var : functionArgs) {
                locals_set.insert(var->GetName());
                const auto& typeParts = var->GetType();
                for(const auto& p : typeParts) {
                    if(p.type == T_IDENTIFIER) {
                        types_set.insert(p.text);
                    }
                }
            }
        }
    }

    wxString buffer;
    FileUtils::ReadFileContent(filepath, buffer);

    vector<TokenWrapper> tokens_vec;
    tokens_vec.reserve(1000);

    // collect all interesting tokens from the document
    SimpleTokenizer tokenizer(buffer);
    TokenWrapper token_wrapper;
    wxArrayString words;
    words.reserve(1000);

    wxStringSet_t unique_tokens;
    while(tokenizer.next(&token_wrapper.token)) {
        const auto& tok = token_wrapper.token;
        wxString word = tok.to_string(buffer);
        if(!CompletionHelper::is_cxx_keyword(word) && (unique_tokens.count(word) == 0)) {
            words.push_back(word);
            unique_tokens.insert(word);
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

    clDEBUG() << tokens_vec << endl;
    // build the response
    size_t id = json["id"].toSize_t();
    JSON root(cJSON_Object);
    JSONItem response = root.toElement();
    auto result = build_result(response, id, cJSON_Object);

    vector<int> encoding;
    encoding.reserve(5 * tokens_vec.size());

    LSPUtils::encode_semantic_tokens(tokens_vec, &encoding);
    result.addProperty("data", encoding);
    clDEBUG() << response.format() << endl;
    channel.write_reply(response);
}

// Request <-->
void ProtocolHandler::on_document_symbol(unique_ptr<JSON>&& msg, Channel& channel)
{
    wxUnusedVar(msg);
    wxUnusedVar(channel);
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

    if(!ensure_file_content_exists(filepath, channel))
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

void ProtocolHandler::on_definition(unique_ptr<JSON>&& msg, Channel& channel)
{
    auto json = msg->toElement();
    size_t id = json["id"].toSize_t();

    clDEBUG() << json.format() << endl;
    wxString filepath_uri = json["params"]["textDocument"]["uri"].toString();
    wxString filepath = wxFileSystem::URLToFileName(filepath_uri).GetFullPath();
    clDEBUG1() << "textDocument/definition: for file" << filepath << endl;

    if(!ensure_file_content_exists(filepath, channel))
        return;

    size_t line = json["params"]["position"]["line"].toSize_t();
    size_t character = json["params"]["position"]["character"].toSize_t();

    CompletionHelper helper;
    wxString text = helper.truncate_file_to_location(m_filesOpened[filepath], line, character, true);

    wxString last_word;
    wxString expression = helper.get_expression(text, false, &last_word);

    clDEBUG() << "Calling WordCompletionCandidates with expression:" << expression << ", last_word=" << last_word
              << endl;
    vector<TagEntryPtr> tags;
    TagsManagerST::Get()->WordCompletionCandidates(filepath, line + 1, expression, text, last_word, tags);
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
