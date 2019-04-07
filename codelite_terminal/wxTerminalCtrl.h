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

class WXDLLIMPEXP_SDK wxTerminalCtrl : public wxPanel
{
protected:
    wxExecuteEnv m_env;
    long m_style = 0;
    wxTextCtrl* m_textCtrl = nullptr;
    IProcess* m_shell = nullptr;
    long m_commandOffset = 0;
    wxTerminalColourHandler m_colourHandler;
    wxTerminalHistory m_history;
    std::unordered_set<long> m_initialProcesses;
    std::string m_pts;      // Unix only
    std::string m_password; // Not used atm
    wxString m_workingDirectory;
    bool m_pauseOnExit = false;
    bool m_printTTY = false;
    bool m_waitingForKey = false;
    bool m_exitWhenDone = false;

protected:
    void PostCreate();
    void AppendText(const wxString& text);
    wxString GetShellCommand() const;
    void SetShellCommand(const wxString& command);
    void SetCaretAtEnd();

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    bool IsEchoOFF() const;
    void DoProcessTerminated();

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

    void SetPrintTTY(bool printTTY) { this->m_printTTY = printTTY; }
    bool IsPrintTTY() const { return m_printTTY; }

    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

    void SetPauseOnExit(bool pauseOnExit) { this->m_pauseOnExit = pauseOnExit; }
    bool IsPauseOnExit() const { return m_pauseOnExit; }

    /**
     * @brief start the terminal
     */
    void Start();

    /**
     * @brief execute a command in the temrinal
     * @param command
     */
    void Run(const wxString& command, bool exitAfter = false);

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

    /**
     * @brief set default style
     */
    void SetDefaultStyle(const wxTextAttr& attr);
};

#endif // WXTERMINALCTRL_H
