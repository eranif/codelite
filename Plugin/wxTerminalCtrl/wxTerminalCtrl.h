#ifndef WXTERMINALCTRL_H
#define WXTERMINALCTRL_H

#include "asyncprocess.h"
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

class TextView;
// Styles
enum {
    // Low word (0-16)

    // Should we forward the process events or handle them internally only?
    // Note that the events will be processed internally first and then will
    // be delegated
    wxTERMINAL_CTRL_USE_EVENTS = (1 << 0),
};

enum class TerminalState {
    NORMAL,
    PASSWORD,
};

class WXDLLIMPEXP_SDK wxTerminalCtrl : public wxPanel
{
protected:
    long m_style = 0;
    IProcess* m_shell = nullptr;
    TextView* m_outputView = nullptr;
    wxTerminalInputCtrl* m_inputCtrl = nullptr;
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
    void SetCaretAtEnd();
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessError(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void ChangeToPasswordStateIfNeeded();

protected:
    void DoProcessTerminated();

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

    TextView* GetView() { return m_outputView; }

    void Terminate();
    void SetAttributes(const wxColour& bg_colour, const wxColour& text_colour, const wxFont& font);

    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

    void SetPauseOnExit(bool pauseOnExit) { this->m_pauseOnExit = pauseOnExit; }
    bool IsPauseOnExit() const { return m_pauseOnExit; }

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
