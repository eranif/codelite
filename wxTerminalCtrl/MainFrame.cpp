#include "MainFrame.h"

#include "SettingsDlg.h"
#include "wxTerminalCtrl.h"

#include <wx/aboutdlg.h>
#include <wx/msgdlg.h>

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
    m_terminal = new wxTerminalCtrl(GetMainPanel());
    GetMainPanel()->GetSizer()->Add(m_terminal, 1, wxEXPAND);

    GetSizer()->Fit(this);
    SetLabel("wxTerminal");
    m_terminal->Bind(wxEVT_TERMINAL_CTRL_DONE, &MainFrame::OnTerminalExit, this);
    m_terminal->Bind(wxEVT_TERMINAL_CTRL_SET_TITLE, &MainFrame::OnSetTitle, this);
    m_terminal->CallAfter(&wxTerminalCtrl::Focus);
    PostSizeEvent();

    // ensure that this window is raised on startup
    CallAfter(&wxFrame::Raise);
}

MainFrame::~MainFrame() {}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_terminal->Terminate();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetName("codelite-terminal");
    info.AddDeveloper("Eran Ifrah");
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(_("CodeLite built-in terminal emulator"));
    ::wxAboutBox(info);
}

void MainFrame::OnTerminalExit(wxTerminalEvent& event)
{
    Hide();
    CallAfter(&MainFrame::DoClose);
}

void MainFrame::DoClose()
{
    const wxTerminalHistory& history = m_terminal->GetHistory();
    wxTheApp->ExitMainLoop();
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
    SettingsDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        m_terminal->SetAttributes(dlg.GetBgColour(), dlg.GetTextColour(), dlg.GetTextFont());
        m_terminal->ClearScreen();
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    wxUnusedVar(event);
    m_terminal->Terminate();
}

void MainFrame::OnSetTitle(wxTerminalEvent& event) { SetLabel(event.GetString()); }
void MainFrame::OnClearScreen(wxCommandEvent& event) { m_terminal->ClearScreen(); }
void MainFrame::OnButton93ButtonClicked(wxCommandEvent& event) {}
