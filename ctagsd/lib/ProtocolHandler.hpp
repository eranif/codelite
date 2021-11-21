#ifndef PROTOCOLHANDLER_HPP
#define PROTOCOLHANDLER_HPP

#include "Channel.hpp"
#include "macros.h"
#include <JSON.h>
#include <memory>
#include <wx/string.h>

#include "istorage.h"

using namespace std;

class ProtocolHandler
{
public:
    typedef void (ProtocolHandler::*CallbackFunc)(unique_ptr<JSON>&& msg, Channel& channel);

private:
    wxString m_file_mask = "*.cpp;*.h;*.hpp;*.cxx;*.cc;*.hxx";
    wxArrayString m_search_path;
    wxStringMap_t m_tokens;
    wxString m_root_folder;
    wxString m_settings_folder;
    wxString m_codelite_indexer;
    wxString m_ignore_spec = ".git;.svn;build-debug/;build-release/;build/";
    vector<wxFileName> m_files;
    wxStringMap_t m_filesOpened;

private:
    void build_result(JSONItem& reply, size_t id);
    void parse_files(Channel& channel);

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

    /**
     * @brief send a "window/logMessage" message to the client
     */
    void send_log_message(const wxString& message, int level, Channel& channel);
};

#endif // PROTOCOLHANDLER_HPP
