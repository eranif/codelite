#ifndef WXTERMINALCTRL_H
#define WXTERMINALCTRL_H

#include <wx/panel.h>
#include <asyncprocess.h>
#include <cl_command_event.h>
#include <wx/textctrl.h>
#include "wxTerminalColourHandler.h"
#include <wx/arrstr.h>

class wxTerminalCtrl : public wxPanel
{
protected:
    struct History {
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

    long m_style = 0;
    wxTextCtrl* m_textCtrl = nullptr;
    IProcess* m_shell = nullptr;
    long m_commandOffset = 0;
    wxTerminalColourHandler m_colourHandler;
    History m_history;

protected:
    void PostCreate();
    void AppendText(const wxString& text);
    wxString GetShellCommand() const;
    void SetShellCommand(const wxString& command);
    void SetCaretAtEnd();
    void GenerateCtrlC();
    void ClearScreen();

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnKeyDown(wxKeyEvent& event);

public:
    wxTerminalCtrl();
    wxTerminalCtrl(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                   const wxString& name = "terminal");
    bool Create(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = "terminal");
    virtual ~wxTerminalCtrl();

    // API
    void Run(const wxString& command);
};

#endif // WXTERMINALCTRL_H
