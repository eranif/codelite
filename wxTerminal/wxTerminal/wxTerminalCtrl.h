#ifndef WXTERMINALCTRL_H
#define WXTERMINALCTRL_H

#include <wx/panel.h>
#include <asyncprocess.h>
#include <cl_command_event.h>
#include <wx/textctrl.h>
#include "wxTerminalColourHandler.h"

class wxTerminalCtrl : public wxPanel
{
    long m_style = 0;
    wxTextCtrl* m_textCtrl = nullptr;
    IProcess* m_shell = nullptr;
    long m_commandOffset = 0;
    wxTerminalColourHandler m_colourHandler;

protected:
    void PostCreate();
    void AppendText(const wxString& text);
    wxString GetShellCommand() const;

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
