#ifndef LLDBCOMMAND_H
#define LLDBCOMMAND_H

#define LLDB_PORT  45674

#include <wx/string.h>
#include "json_node.h"
class LLDBCommand
{
public:
    enum {
        kCommandInvalid = -1,
        kCommandStart,
        kCommandRun,
        kCommandStop,
    };

protected:
    int m_commandType;
    wxString m_commandArguments;
    wxString m_workingDirectory;
    wxString m_executable;
public:

    // Serialization API
    JSONElement ToJSON() const;
    void FromJSON(const JSONElement &json);

    LLDBCommand() : m_commandType(kCommandInvalid) {}
    LLDBCommand(const wxString &jsonString);
    virtual ~LLDBCommand();


    void SetExecutable(const wxString& executable) {
        this->m_executable = executable;
    }
    const wxString& GetExecutable() const {
        return m_executable;
    }
    void SetCommandArguments(const wxString& commandArguments) {
        this->m_commandArguments = commandArguments;
    }
    void SetCommandType(int commandType) {
        this->m_commandType = commandType;
    }
    const wxString& GetCommandArguments() const {
        return m_commandArguments;
    }
    int GetCommandType() const {
        return m_commandType;
    }
    void SetWorkingDirectory(const wxString& workingDirectory) {
        this->m_workingDirectory = workingDirectory;
    }
    const wxString& GetWorkingDirectory() const {
        return m_workingDirectory;
    }
};

#endif // LLDBCOMMAND_H
