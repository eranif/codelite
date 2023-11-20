#ifndef WXTERMINALCTRL_H
#define WXTERMINALCTRL_H

#include "asyncprocess.h"
#include "clResult.hpp"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "processreaderthread.h"
#include "wxTerminalColourHandler.h"
#include "wxTerminalEvent.hpp"
#include "wxTerminalHistory.hpp"
#include "wxTerminalInputCtrl.hpp"

#include <unordered_set>
#include <wx/arrstr.h>
#include <wx/ffile.h>
#include <wx/panel.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <wx/utils.h>

class wxTerminalOutputCtrl;
// Styles
enum {
    // Low word (0-16)

    // Should we forward the process events or handle them internally only?
    // Note that the events will be processed internally first and then will
    // be delegated
    wxTERMINAL_CTRL_USE_EVENTS = (1 << 0),
};

class WXDLLIMPEXP_SDK wxTerminalCtrl : public wxPanel
{
protected:
    long m_style = 0;
    IProcess* m_shell = nullptr;
    wxTerminalOutputCtrl* m_outputView = nullptr;
    wxTerminalInputCtrl* m_inputCtrl = nullptr;
    std::unordered_set<long> m_initialProcesses;
    wxTextAttr m_preEchoOffAttr;
    bool m_pauseOnExit = false;
    bool m_printTTY = false;
    wxString m_startupCommand;
    wxString m_logfile;
    wxString m_ttyfile;
    bool m_terminating = false;
    wxString m_processOutput;
    wxString m_startingDirectory;
    wxString m_shellCommand;

protected:
    void StartShell();
    void AppendText(wxStringView text);
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessError(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    bool PromptForPasswordIfNeeded(const wxString& line_lowercase);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void ProcessOutputBuffer();
    wxStringView GetNextLine();

protected:
    void DoProcessTerminated();

public:
    wxTerminalCtrl();
    wxTerminalCtrl(wxWindow* parent, wxWindowID winid, const wxString& working_directory,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                   long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxTERMINAL_CTRL_USE_EVENTS,
                   const wxString& name = "terminal");
    bool Create(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxTERMINAL_CTRL_USE_EVENTS,
                const wxString& name = "terminal");
    virtual ~wxTerminalCtrl();

    wxTerminalOutputCtrl* GetView() { return m_outputView; }
    bool IsFocused();
    void SSHAndSetWorkingDirectory(const wxString& ssh_account, const wxString& path);
    void SetTerminalWorkingDirectory(const wxString& path);
    void Terminate();
    void SetAttributes(const wxColour& bg_colour, const wxColour& text_colour, const wxFont& font);

    void SetPauseOnExit(bool pauseOnExit) { this->m_pauseOnExit = pauseOnExit; }
    bool IsPauseOnExit() const { return m_pauseOnExit; }

    wxTerminalInputCtrl* GetInputCtrl() { return m_inputCtrl; }

    void SetShellCommand(const wxString& shellName) { this->m_shellCommand = shellName; }
    const wxString& GetShellCommand() const { return m_shellCommand; }

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
     * @brief send TAB char to the terminal
     */
    void SendTab();

    /**
     * @brief clear the display (Ctrl-L)
     */
    void ClearScreen();

    /**
     * @brief Logout from the current session (Ctrl-D)
     */
    void Logout();

    void SetLogfile(const wxString& logfile) { this->m_logfile = logfile; }
    const wxString& GetLogfile() const { return m_logfile; }
};

#endif // WXTERMINALCTRL_H
