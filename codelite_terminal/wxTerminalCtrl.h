#ifndef WXTERMINALCTRL_H
#define WXTERMINALCTRL_H

#include <wx/panel.h>
#include <asyncprocess.h>
#include <cl_command_event.h>
#include <wx/textctrl.h>
#include "wxTerminalColourHandler.h"
#include <wx/arrstr.h>
#include "codelite_exports.h"
#include <wx/utils.h>
#include <wx/ffile.h>

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
        if(m_commands.empty()) { return; }
        ++m_current;
        if(m_current >= (int)m_commands.size()) { m_current = (m_commands.size() - 1); }
    }

    void Down()
    {
        if(m_commands.empty()) { return; }
        --m_current;
        if(m_current < 0) { m_current = 0; }
    }

    wxString Get() const
    {
        if(m_current < 0 || m_current >= (int)m_commands.size()) { return ""; }
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

// The terminal is ready. This event will include the PTS name (e.g. /dev/pts/12).
// Use event.GetString()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_READY, clCommandEvent);
// Fired when stdout output is ready
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_OUTPUT, clCommandEvent);
// Fired when stderr output is ready
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_STDERR, clCommandEvent);
// The terminal has exited
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_DONE, clCommandEvent);
// Set the terminal title
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_SET_TITLE, clCommandEvent);

class WXDLLIMPEXP_SDK wxTerminalCtrl : public wxPanel
{
protected:
    wxExecuteEnv m_env;
    long m_style = 0;
    TextView* m_textCtrl = nullptr;
    IProcess* m_shell = nullptr;
    long m_commandOffset = 0;
    wxTerminalHistory m_history;
    std::unordered_set<long> m_initialProcesses;
    std::string m_pts;      // Unix only
    bool m_echoOff = false; // Not used atm
    wxTextAttr m_preEchoOffAttr;
    wxString m_workingDirectory;
    bool m_pauseOnExit = false;
    bool m_printTTY = false;
    bool m_waitingForKey = false;
    wxString m_startupCommand;
    wxString m_logfile;
    wxFFile m_log;
    wxString m_ttyfile;

protected:
    void PostCreate();
    void AppendText(const wxString& text);
    wxString GetShellCommand() const;
    void SetShellCommand(const wxString& command);
    void SetCaretAtEnd();
    void OnIdle(wxIdleEvent& event);
    
protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void SetEchoOff();
    void DoProcessTerminated();
    void CheckInsertionPoint();

public:
    wxTerminalCtrl();
    wxTerminalCtrl(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxExecuteEnv& env = wxExecuteEnv(),
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                   long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxTERMINAL_CTRL_USE_EVENTS,
                   const wxString& name = "terminal");
    bool Create(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxExecuteEnv& env = wxExecuteEnv(),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxTERMINAL_CTRL_USE_EVENTS,
                const wxString& name = "terminal");
    virtual ~wxTerminalCtrl();

    void ReloadSettings();
    void SetPrintTTY(bool printTTY, const wxString& ttyfile)
    {
        this->m_printTTY = printTTY;
        this->m_ttyfile = ttyfile;
    }
    bool IsPrintTTY() const { return m_printTTY; }

    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

    void SetPauseOnExit(bool pauseOnExit) { this->m_pauseOnExit = pauseOnExit; }
    bool IsPauseOnExit() const { return m_pauseOnExit; }

    const wxTerminalHistory& GetHistory() const { return m_history; }

    /**
     * @brief start the terminal
     */
    /**
     * @brief
     * @param startupCommand
     */
    void Start(const wxString& startupCommand);

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
    /**
     * @brief return the underlying controlling terminal (e.g. /dev/pts/9).
     * Returns empty string on Windows
     */
    wxString GetPTS() const;

    void SetLogfile(const wxString& logfile) { this->m_logfile = logfile; }
    const wxString& GetLogfile() const { return m_logfile; }

    /**
     * @brief set the focus to the text area
     */
    void Focus();
};

#endif // WXTERMINALCTRL_H
