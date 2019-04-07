#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include "wxTerminalCtrl.h"

MainFrame::MainFrame(wxWindow* parent, wxTerminalOptions& options)
    : MainFrameBaseClass(parent)
    , m_options(options)
{
    m_terminal = new wxTerminalCtrl(GetMainPanel());
    m_terminal->SetWorkingDirectory(m_options.GetWorkingDirectory());
    m_terminal->SetPauseOnExit(m_options.IsWaitOnExit());
    m_terminal->SetPrintTTY(m_options.IsPrintTTY());
    m_terminal->Start(m_options.GetCommand());
    
    GetMainPanel()->GetSizer()->Add(m_terminal, 1, wxEXPAND);
    SetLabel(m_options.GetTitle().IsEmpty() ? "codelite-terminal" : m_options.GetTitle());
    m_terminal->Bind(wxEVT_TERMINAL_CTRL_DONE, &MainFrame::OnTerminalExit, this);
}

MainFrame::~MainFrame() {}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
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

void MainFrame::OnTerminalExit(clCommandEvent& event) { wxExit(); }
