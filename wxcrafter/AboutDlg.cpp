#include "AboutDlg.h"
#include "serial_number.h"
#include "wxc_settings.h"
#include "wxcsvninfo.h"
#include "wxgui_bitmaploader.h"
#include <json_node.h>
#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/url.h>

wxcAboutDlg::wxcAboutDlg(wxWindow* parent)
    : wxcAboutDlgBaseClass(parent)
    , m_modified(false)
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

void wxcAboutDlg::OnUpdateLicense(wxCommandEvent& event)
{
    m_modified = false;
    wxcSettings::Get().SetAnnoyDialogs(wxcSettings::Get().GetAnnoyDialogs() | wxcSettings::LICENSE_ACTIVATED);
    wxcSettings::Get().Save();

    wxCommandEvent evt(wxEVT_LICENSE_UPDATED_SUCCESSFULLY);
    wxTheApp->AddPendingEvent(evt);
    DoRefresh();
}

void wxcAboutDlg::OnUpdateLicenseUI(wxUpdateUIEvent& event) { event.Enable(true); }

void wxcAboutDlg::OnTextUpdated(wxCommandEvent& event)
{
    m_modified = true;
    event.Skip();
}

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

bool wxcAboutDlg::IsValidUser(const wxString& username, const wxString& serialNumber) const
{
    wxUnusedVar(username);
    wxUnusedVar(serialNumber);
    return true;
}
