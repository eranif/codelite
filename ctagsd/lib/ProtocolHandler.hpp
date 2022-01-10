#ifndef PROTOCOLHANDLER_HPP
#define PROTOCOLHANDLER_HPP

#include "Channel.hpp"
#include "CodeLiteIndexer.hpp"
#include "CompletionHelper.hpp"
#include "CxxCodeCompletion.hpp"
#include "ParseThread.hpp"
#include "Scanner.hpp"
#include "Settings.hpp"
#include "istorage.h"
#include "macros.h"

#include <JSON.h>
#include <algorithm>
#include <memory>
#include <wx/string.h>

using namespace std;

struct CachedComment {
    wxString str;
    long line;
    long column;
    // line to comment map
    typedef unordered_map<long, wxString> Map_t;
};

struct ParsedFileInfo {
    wxStringSet_t included_files;
    wxStringSet_t using_namespace;
};

class ProtocolHandler
{
public:
    typedef void (ProtocolHandler::*CallbackFunc)(unique_ptr<JSON>&& msg, Channel& channel);

private:
    CTagsdSettings m_settings;
    wxString m_root_folder;
    wxString m_settings_folder;
    wxStringMap_t m_filesOpened;

    // cached parsed comments file <-> comments
    unordered_map<wxString, CachedComment::Map_t> m_comments_cache;
    unordered_map<wxString, ParsedFileInfo> m_parsed_files_info;
    unordered_map<wxString, vector<wxString>> m_additional_scopes;
    wxArrayString m_search_paths;
    Scanner m_file_scanner;
    CxxCodeCompletion::ptr_t m_completer;
    ParseThread m_parse_thread;
    static unordered_map<wxString, vector<TagEntry>> m_tags_cache;

private:
    JSONItem build_result(JSONItem& reply, size_t id, int result_kind);
    static void parse_files(const wxArrayString& files, const wxString& settings_folder, const wxString& indexer_path,
                            const wxString& alternate_filename = wxEmptyString);
    bool ensure_file_content_exists(const wxString& filepath, Channel& channel, size_t req_id);
    void update_comments_for_file(const wxString& filepath, const wxString& file_content);
    void update_comments_for_file(const wxString& filepath);
    const wxString& get_comment(const wxString& filepath, long line, const wxString& default_value) const;
    bool do_comments_exist_for_file(const wxString& filepath) const;
    vector<wxString> update_additional_scopes_for_file(const wxString& filepath);

    wxArrayString FilterNonWantedNamespaces(const wxArrayString& namespace_arr) const;
    void do_definition(unique_ptr<JSON>&& msg, Channel& channel, bool try_definition_first);
    void build_search_path();
    void parse_file_for_includes_and_using_namespace(const wxString& filepath);
    /**
     * @brief return list of files for parsing. The list is constructed using the `#include`
     * statements found in `files` and their children (recursively)
     */
    wxArrayString get_files_to_parse(const wxArrayString& files);
    /**
     * @brief return list of files included directly by `filepath`
     */
    wxArrayString get_first_level_includes(const wxString& filepath);

    size_t get_includes_recrusively(const wxString& filepath, wxStringSet_t* output);
    wxString minimize_buffer(const wxString& filepath, int line, int character, const wxString& src_string,
                             CompletionHelper::eTruncateStyle flag = CompletionHelper::TRUNCATE_EXACT_POS);

public:
    ProtocolHandler();
    ~ProtocolHandler();

    // symbols cache API
    static void cache_set_document_symbols(const wxString& filepath, vector<TagEntry>&& tags);
    static size_t cache_get_document_symbols(const wxString& filepath, vector<TagEntry>& tags);
    static void cache_erase_document_symbols(const wxString& filepath);

    // General API
    static void parse_buffer_async(const wxString& filepath, const wxString& file_content,
                                   const wxString& settings_folder, const wxString& indexer_path);
    static void parse_files_async(const vector<wxString>& files, const wxString& settings_folder,
                                  const wxString& indexer_path);

    void on_initialize(unique_ptr<JSON>&& msg, Channel& channel);
    void on_initialized(unique_ptr<JSON>&& msg, Channel& channel);
    void on_unsupported_message(unique_ptr<JSON>&& msg, Channel& channel);
    void on_did_open(unique_ptr<JSON>&& msg, Channel& channel);
    void on_did_change(unique_ptr<JSON>&& msg, Channel& channel);
    void on_completion(unique_ptr<JSON>&& msg, Channel& channel);
    void on_did_close(unique_ptr<JSON>&& msg, Channel& channel);
    void on_did_save(unique_ptr<JSON>&& msg, Channel& channel);
    void on_semantic_tokens(unique_ptr<JSON>&& msg, Channel& channel);
    void on_document_symbol(unique_ptr<JSON>&& msg, Channel& channel);
    void on_document_signature_help(unique_ptr<JSON>&& msg, Channel& channel);
    void on_definition(unique_ptr<JSON>&& msg, Channel& channel);
    void on_declaration(unique_ptr<JSON>&& msg, Channel& channel);
    void on_hover(unique_ptr<JSON>&& msg, Channel& channel);
    void on_workspace_symbol(unique_ptr<JSON>&& msg, Channel& channel);

    /**
     * @brief send a "window/logMessage" message to the client
     */
    void send_log_message(const wxString& message, int level, Channel& channel);
};

#endif // PROTOCOLHANDLER_HPP
