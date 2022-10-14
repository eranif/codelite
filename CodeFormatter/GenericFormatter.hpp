#ifndef GENERICFORMATTER_HPP
#define GENERICFORMATTER_HPP

#include "SourceFormatterBase.hpp"
#include "clCodeLiteRemoteProcess.hpp"
#include "clConcurrent.hpp"

#include <vector>
#include <wx/arrstr.h>

/// A formatter based on a simple command execution
class GenericFormatter : public SourceFormatterBase
{
    wxArrayString m_command;
    wxString m_remote_command;
    wxString m_workingDirectory;
    clConcurrent m_concurrent;

protected:
    bool DoFormatFile(const wxString& filepath, FileExtManager::FileType file_type, wxEvtHandler* sink,
                      wxString* output);
    void thread_format(const wxString& cmd, const wxString& wd, const wxString& filepath, bool inplace_formatter,
                       wxEvtHandler* sink);

public:
    GenericFormatter();
    virtual ~GenericFormatter();

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

    bool FormatFile(const wxFileName& filepath, FileExtManager::FileType file_type, wxEvtHandler* sink) override;
    bool FormatFile(const wxString& filepath, FileExtManager::FileType file_type, wxEvtHandler* sink) override;
    bool FormatRemoteFile(const wxString& filepath, FileExtManager::FileType file_type, wxEvtHandler* sink) override;
    bool FormatString(const wxString& content, const wxString& fullpath, wxString* output) override;

    bool CanHandleRemoteFile() const { return !m_remote_command.empty(); }
    void SetRemoteCommand(const wxString& cmd);

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
