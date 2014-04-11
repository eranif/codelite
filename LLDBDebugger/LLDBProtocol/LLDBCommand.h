#ifndef LLDBCOMMAND_H
#define LLDBCOMMAND_H

#define LLDB_PORT  45674

#include <wx/string.h>
#include "json_node.h"
#include "LLDBEnums.h"
#include "LLDBBreakpoint.h"

class LLDBCommand
{
protected:
    int                   m_commandType;
    wxString              m_commandArguments;
    wxString              m_workingDirectory;
    wxString              m_executable;
    wxString              m_redirectTTY;
    LLDBBreakpoint::Vec_t m_breakpoints;
    int                   m_interruptReason;

public:
    // Serialization API
    JSONElement ToJSON() const;
    void FromJSON(const JSONElement &json);

    LLDBCommand() : m_commandType(kCommandInvalid), m_interruptReason(kInterruptReasonNone) {}
    LLDBCommand(const wxString &jsonString);
    virtual ~LLDBCommand();

    void Clear() {
        m_commandType = kCommandInvalid;
        m_commandArguments.clear();
        m_workingDirectory.clear();
        m_executable.clear();
        m_redirectTTY.clear();
        m_breakpoints.clear();
    }

    void SetInterruptReason(int interruptReason) {
        this->m_interruptReason = interruptReason;
    }
    int GetInterruptReason() const {
        return m_interruptReason;
    }
    void SetRedirectTTY(const wxString& redirectTTY) {
        this->m_redirectTTY = redirectTTY;
    }
    const wxString& GetRedirectTTY() const {
        return m_redirectTTY;
    }
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
    void SetBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints) {
        this->m_breakpoints = breakpoints;
    }
    const LLDBBreakpoint::Vec_t& GetBreakpoints() const {
        return m_breakpoints;
    }
};

#endif // LLDBCOMMAND_H
