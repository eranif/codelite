#ifndef GENERICFORMATTER_HPP
#define GENERICFORMATTER_HPP

#include "SourceFormatterBase.hpp"
#include "clCodeLiteRemoteProcess.hpp"
#include "cl_remote_executor.hpp"
#include "procutils.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <wx/arrstr.h>

struct CommandMetadata {
    wxString m_command;
    wxString m_filepath;
    wxEvtHandler* m_sink = nullptr;

    CommandMetadata() = default;
    CommandMetadata(const wxString& command, const wxString& filepath, wxEvtHandler* sink)
        : m_command(command)
        , m_filepath(filepath)
        , m_sink(sink)
    {
    }
};

/// A formatter based on a simple command execution
class GenericFormatter : public SourceFormatterBase
{
    wxArrayString m_command;
    wxString m_workingDirectory;
    std::unordered_map<long, CommandMetadata> m_pid_commands;
    std::vector<std::pair<wxString, wxEvtHandler*>> m_inFlightFiles;

    // remote execution
    wxString m_remote_command;
    clEnvList_t m_remote_env;
    wxString m_remote_wd;

protected:
    bool DoFormatFile(const wxString& filepath, wxEvtHandler* sink, wxString* output);
    void AsyncFormat(const wxString& cmd, const wxString& wd, const wxString& filepath, bool inplace_formatter,
                     wxEvtHandler* sink);
    bool SyncFormat(const wxString& cmd, const wxString& wd, bool inplace_formatter, wxString* output);
    void OnAsyncShellProcessTerminated(clShellProcessEvent& event);
    void OnRemoteCommandStdout(clCommandEvent& event);
    void OnRemoteCommandStderr(clCommandEvent& event);
    void OnRemoteCommandDone(clCommandEvent& event);
    void OnRemoteCommandError(clCommandEvent& event);
    std::unique_ptr<clEnvList_t> CreateLocalEnv();

public:
    GenericFormatter();
    virtual ~GenericFormatter();

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

    bool FormatFile(const wxFileName& filepath, wxEvtHandler* sink) override;
    bool FormatFile(const wxString& filepath, wxEvtHandler* sink) override;
    bool FormatRemoteFile(const wxString& filepath, wxEvtHandler* sink) override;
    bool FormatString(const wxString& content, const wxString& fullpath, wxString* output) override;

    bool CanHandleRemoteFile() const { return !m_remote_command.empty(); }
    void SetRemoteCommand(const wxString& cmd, const wxString& remote_wd, const clEnvList_t& env);

    void SetCommand(const wxArrayString& command) { this->m_command = command; }
    void SetCommand(const std::vector<wxString>& command);
    void SetCommandFromString(const wxString& command);

    const wxArrayString& GetCommand() const { return m_command; }
    wxString GetCommandAsString() const;

    const wxString& GetRemoteCommand() const { return m_remote_command; }

    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

    wxString GetCommandWithComments() const;
};

#endif // GENERICFORMATTER_HPP
