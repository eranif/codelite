#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include "wxTerminalCtrl.h"
#include "SettingsDlg.h"
#include "wxTerminalOptions.h"

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
    wxTerminalOptions& options = wxTerminalOptions::Get();
    options.Load();
    m_terminal = new wxTerminalCtrl(GetMainPanel());
    m_terminal->SetWorkingDirectory(options.GetWorkingDirectory());
    m_terminal->SetPauseOnExit(options.IsWaitOnExit());
    m_terminal->SetPrintTTY(options.IsPrintTTY());
    m_terminal->SetLogfile(options.GetLogfile());
    m_terminal->Start(options.GetCommand());

    GetMainPanel()->GetSizer()->Add(m_terminal, 1, wxEXPAND);
    SetLabel(options.GetTitle().IsEmpty() ? "codelite-terminal" : options.GetTitle());
    m_terminal->Bind(wxEVT_TERMINAL_CTRL_DONE, &MainFrame::OnTerminalExit, this);
    m_terminal->Bind(wxEVT_TERMINAL_CTRL_SET_TITLE, &MainFrame::OnSetTitle, this);
}

MainFrame::~MainFrame() {}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Hide();
    CallAfter(&MainFrame::DoClose);
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

void MainFrame::OnTerminalExit(clCommandEvent& event)
{
    Hide();
    CallAfter(&MainFrame::DoClose);
}

void MainFrame::DoClose()
{
    wxTerminalOptions::Get().Save();
    wxTheApp->ExitMainLoop();
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
    SettingsDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        dlg.Save();
        m_terminal->ReloadSettings();
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    Hide();
    CallAfter(&MainFrame::DoClose);
}

void MainFrame::OnSetTitle(clCommandEvent& event) { SetLabel(event.GetString()); }
