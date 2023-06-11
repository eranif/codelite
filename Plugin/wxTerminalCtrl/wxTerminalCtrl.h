#ifndef WXTERMINALCTRL_H
#define WXTERMINALCTRL_H

#include "asyncprocess.h"
#include "codelite_exports.h"
#include "processreaderthread.h"
#include "wxTerminalColourHandler.h"

#include <unordered_set>
#include <wx/arrstr.h>
#include <wx/ffile.h>
#include <wx/panel.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <wx/utils.h>

class TextView;
struct WXDLLIMPEXP_SDK wxTerminalHistory {
    wxArrayString m_commands;
    int m_current = wxNOT_FOUND;

    void Add(const wxString& command)
    {
        m_commands.Insert(command, 0);
        m_current = wxNOT_FOUND;
    }

    void Up()
    {
        if(m_commands.empty()) {
            return;
        }
        ++m_current;
        if(m_current >= (int)m_commands.size()) {
            m_current = (m_commands.size() - 1);
        }
    }

    void Down()
    {
        if(m_commands.empty()) {
            return;
        }
        --m_current;
        if(m_current < 0) {
            m_current = 0;
        }
    }

    wxString Get() const
    {
        if(m_current < 0 || m_current >= (int)m_commands.size()) {
            return "";
        }
        return m_commands.Item(m_current);
    }

    void Clear()
    {
        m_current = wxNOT_FOUND;
        m_commands.clear();
    }

    const wxArrayString& GetCommands() const { return m_commands; }
    void SetCommands(const wxArrayString& commands)
    {
        m_commands = commands;
        m_current = wxNOT_FOUND;
    }
};

// Styles
enum {
    // Low word (0-16)

    // Should we forward the process events or handle them internally only?
    // Note that the events will be processed internally first and then will
    // be delegated
    wxTERMINAL_CTRL_USE_EVENTS = (1 << 0),
};

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

enum class TerminalState {
    NORMAL,
    PASSWORD,
};

class WXDLLIMPEXP_SDK wxTerminalCtrl : public wxPanel
{
protected:
    long m_style = 0;
    IProcess* m_shell = nullptr;
    TextView* m_textCtrl = nullptr;
    long m_commandOffset = 0;
    wxTerminalHistory m_history;
    std::unordered_set<long> m_initialProcesses;
    wxTextAttr m_preEchoOffAttr;
    wxString m_workingDirectory;
    bool m_pauseOnExit = false;
    bool m_printTTY = false;
    wxString m_startupCommand;
    wxString m_logfile;
    wxString m_ttyfile;
    bool m_terminating = false;
    TerminalState m_state = TerminalState::NORMAL;

protected:
    void StartShell();
    void AppendText(const std::string& text);
    wxString GetShellCommand() const;
    void SetShellCommand(const wxString& command);
    void SetCaretAtEnd();
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessError(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void ChangeToPasswordStateIfNeeded();

protected:
    void OnCharHook(wxKeyEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void DoProcessTerminated();
    void CheckInsertionPoint();

public:
    wxTerminalCtrl();
    wxTerminalCtrl(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxTERMINAL_CTRL_USE_EVENTS,
                   const wxString& name = "terminal");
    bool Create(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxTERMINAL_CTRL_USE_EVENTS,
                const wxString& name = "terminal");
    virtual ~wxTerminalCtrl();

    TextView* GetView() { return m_textCtrl; }

    void Terminate();
    void SetAttributes(const wxColour& bg_colour, const wxColour& text_colour, const wxFont& font);

    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

    void SetPauseOnExit(bool pauseOnExit) { this->m_pauseOnExit = pauseOnExit; }
    bool IsPauseOnExit() const { return m_pauseOnExit; }

    const wxTerminalHistory& GetHistory() const { return m_history; }

    /**
     * @brief execute a command in the temrinal
     * @param command
     */
    void Run(const wxString& command);

    /**
     * @brief generate Ctrl-C like. By default this will send SIGTERM (Ctrl-C)
     */
    void GenerateCtrlC();
    /**
     * @brief clear the display (Ctrl-L)
     */
    void ClearScreen();

    /**
     * @brief clear the current line (Ctrl-U)
     */
    void ClearLine();
    /**
     * @brief Logout from the current session (Ctrl-D)
     */
    void Logout();

    void SetLogfile(const wxString& logfile) { this->m_logfile = logfile; }
    const wxString& GetLogfile() const { return m_logfile; }

    /**
     * @brief set the focus to the text area
     */
    void Focus();
};

#endif // WXTERMINALCTRL_H
