#include "AboutDlg.h"

#include "wxcsvninfo.h"

wxcAboutDlg::wxcAboutDlg(wxWindow* parent)
    : wxcAboutDlgBaseClass(parent)
{
    DoRefresh();

    wxString buildNumber;
    wxString vers;

    buildNumber << GIT_REVISION;
    vers << "wxCrafter-" << GIT_REVISION;
    m_staticTextBuildNumber->SetLabel(buildNumber);
    m_staticTextVersion->SetLabel(vers);
}

wxcAboutDlg::~wxcAboutDlg() {}

void wxcAboutDlg::DoRefresh()
{
    GetSizer()->Fit(this);
    Layout();
}

void wxcAboutDlg::OnSize(wxSizeEvent& event)
{
    event.Skip();
    m_staticBitmap178->Refresh();
}
