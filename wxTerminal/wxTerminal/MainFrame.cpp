#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include "wxTerminalCtrl.h"

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
    GetMainPanel()->GetSizer()->Add(new wxTerminalCtrl(GetMainPanel()), 1, wxEXPAND);
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
    info.AddDeveloper("Eran Ifrah");
    info.SetCopyright(_("My MainFrame"));
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(_("Short description goes here"));
    ::wxAboutBox(info);
}
