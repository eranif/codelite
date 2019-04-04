#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include "wxTerminalCtrl.h"

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
    m_terminal = new wxTerminalCtrl(GetMainPanel());
    GetMainPanel()->GetSizer()->Add(m_terminal, 1, wxEXPAND);
    wxFont f(wxFontInfo(18).FaceName("Consolas"));
    m_terminal->SetDefaultStyle(wxTextAttr(wxNullColour, wxNullColour, f));
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
