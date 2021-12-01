#ifndef PROTOCOLHANDLER_HPP
#define PROTOCOLHANDLER_HPP

#include "Channel.hpp"
#include "macros.h"
#include <JSON.h>
#include <memory>
#include <wx/string.h>

#include "Settings.hpp"
#include "istorage.h"

using namespace std;

struct CachedComment {
    wxString str;
    long line;
    long column;
    // line to comment map
    typedef unordered_map<long, wxString> Map_t;
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

private:
    JSONItem build_result(JSONItem& reply, size_t id, int result_kind);
    void parse_files(wxArrayString& files, Channel* channel, bool initial_parse);
    bool ensure_file_content_exists(const wxString& filepath, Channel& channel);
    void update_comments_for_file(const wxString& filepath, const wxString& file_content);
    void update_comments_for_file(const wxString& filepath);
    const wxString& get_comment(const wxString& filepath, long line, const wxString& default_value) const;
    bool do_comments_exist_for_file(const wxString& filepath) const;

public:
    ProtocolHandler();
    ~ProtocolHandler();

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
    /**
     * @brief send a "window/logMessage" message to the client
     */
    void send_log_message(const wxString& message, int level, Channel& channel);
};

#endif // PROTOCOLHANDLER_HPP
