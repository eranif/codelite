#ifndef WXTERMINALEVENT_HPP
#define WXTERMINALEVENT_HPP

#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/ffile.h>
#include <wx/utils.h>

/// a wxCommandEvent that takes ownership of the clientData
class WXDLLIMPEXP_SDK wxTerminalEvent : public wxCommandEvent
{
protected:
    wxArrayString m_strings;
    wxString m_fileName;
    wxString m_oldName;
    bool m_answer;
    bool m_allowed;
    int m_lineNumber;
    bool m_selected;
    std::string m_stringRaw;

public:
    wxTerminalEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    wxTerminalEvent(const wxTerminalEvent& event);
    wxTerminalEvent& operator=(const wxTerminalEvent& src);
    virtual ~wxTerminalEvent();

    wxClientData* GetClientObject() const;
    virtual wxEvent* Clone() const;

    wxTerminalEvent& SetLineNumber(int lineNumber)
    {
        this->m_lineNumber = lineNumber;
        return *this;
    }
    int GetLineNumber() const { return m_lineNumber; }
    wxTerminalEvent& SetAllowed(bool allowed)
    {
        this->m_allowed = allowed;
        return *this;
    }
    wxTerminalEvent& SetAnswer(bool answer)
    {
        this->m_answer = answer;
        return *this;
    }
    wxTerminalEvent& SetFileName(const wxString& fileName)
    {
        this->m_fileName = fileName;
        return *this;
    }
    wxTerminalEvent& SetOldName(const wxString& oldName)
    {
        this->m_oldName = oldName;
        return *this;
    }
    wxTerminalEvent& SetStrings(const wxArrayString& strings)
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

typedef void (wxEvtHandler::*wxTerminalEventFunction)(wxTerminalEvent&);
#define wxTerminalEventHandler(func) wxEVENT_HANDLER_CAST(wxTerminalEventFunction, func)

// The terminal is ready. This event will include the PTS name (e.g. /dev/pts/12).
// Use event.GetString()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_READY, wxTerminalEvent);
// Fired when stdout output is ready
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_OUTPUT, wxTerminalEvent);
// Fired when stderr output is ready
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_STDERR, wxTerminalEvent);
// The terminal has exited
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_DONE, wxTerminalEvent);
// Set the terminal title
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_SET_TITLE, wxTerminalEvent);

#endif // WXTERMINALEVENT_HPP
