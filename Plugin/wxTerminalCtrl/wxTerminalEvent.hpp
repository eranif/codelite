#ifndef WXTERMINALEVENT_HPP
#define WXTERMINALEVENT_HPP

#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/ffile.h>
#include <wx/utils.h>

/// a wxCommandEvent that takes ownership of the clientData
class WXDLLIMPEXP_SDK wxTerminalCtrlEvent : public wxCommandEvent
{
protected:
    wxArrayString m_strings;
    wxString m_fileName;
    wxString m_oldName;
    bool m_answer = false;
    bool m_allowed = true;
    int m_lineNumber = 0;
    bool m_selected = false;
    std::string m_stringRaw;

public:
    wxTerminalCtrlEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    wxTerminalCtrlEvent(const wxTerminalCtrlEvent&) = default;
    wxTerminalCtrlEvent& operator=(const wxTerminalCtrlEvent&) = delete;
    ~wxTerminalCtrlEvent() override = default;

    //wxClientData* GetClientObject() const;
    wxEvent* Clone() const override;

    wxTerminalCtrlEvent& SetLineNumber(int lineNumber)
    {
        this->m_lineNumber = lineNumber;
        return *this;
    }
    int GetLineNumber() const { return m_lineNumber; }
    wxTerminalCtrlEvent& SetAllowed(bool allowed)
    {
        this->m_allowed = allowed;
        return *this;
    }
    wxTerminalCtrlEvent& SetAnswer(bool answer)
    {
        this->m_answer = answer;
        return *this;
    }
    wxTerminalCtrlEvent& SetFileName(const wxString& fileName)
    {
        this->m_fileName = fileName;
        return *this;
    }
    wxTerminalCtrlEvent& SetOldName(const wxString& oldName)
    {
        this->m_oldName = oldName;
        return *this;
    }
    wxTerminalCtrlEvent& SetStrings(const wxArrayString& strings)
    {
        this->m_strings = strings;
        return *this;
    }
    bool IsAllowed() const { return m_allowed; }
    bool IsAnswer() const { return m_answer; }
    const wxString& GetFileName() const { return m_fileName; }
    const wxString& GetOldName() const { return m_oldName; }
    const wxArrayString& GetStrings() const { return m_strings; }
    wxArrayString& GetStrings() { return m_strings; }
    const std::string& GetStringRaw() const { return m_stringRaw; }
    void SetStringRaw(const std::string& str) { m_stringRaw = str; }
};

using wxTerminalEventFunction = void (wxEvtHandler::*)(wxTerminalCtrlEvent&);
#define wxTerminalEventHandler(func) wxEVENT_HANDLER_CAST(wxTerminalEventFunction, func)

// The terminal is ready. This event will include the PTS name (e.g. /dev/pts/12).
// Use event.GetString()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_READY, wxTerminalCtrlEvent);
// Fired when stdout output is ready
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_OUTPUT, wxTerminalCtrlEvent);
// Fired when stderr output is ready
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_STDERR, wxTerminalCtrlEvent);
// The terminal has exited
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_DONE, wxTerminalCtrlEvent);
// Set the terminal title
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_SET_TITLE, wxTerminalCtrlEvent);

#endif // WXTERMINALEVENT_HPP
