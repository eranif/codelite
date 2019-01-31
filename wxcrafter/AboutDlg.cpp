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
    m_textCtrl1Username->ChangeValue(wxcSettings::Get().GetUsername());
    m_textCtrlSerialNumber->ChangeValue(wxcSettings::Get().GetSerialNumber());

    wxString buildNumber;
    wxString vers;

    buildNumber << GIT_REVISION;
    vers << "wxCrafter-" << GIT_REVISION;
    m_staticTextBuildNumber->SetLabel(buildNumber);
    m_staticTextVersion->SetLabel(vers);

    m_textCtrlEULA->SetEditable(true);
    wxCrafter::ResourceLoader rl;
    wxString EULA = rl.File("LICENSE");
    m_textCtrlEULA->ChangeValue(EULA);
    m_textCtrlEULA->SetEditable(false);
}

wxcAboutDlg::~wxcAboutDlg() {}

void wxcAboutDlg::OnUpdateLicense(wxCommandEvent& event)
{
    m_modified = false;
    // Contact codelite.org to acticated the license
    if(!IsValidUser(m_textCtrl1Username->GetValue(), m_textCtrlSerialNumber->GetValue())) {
        ::wxMessageBox(_("Invalid username/serial number"), "wxCrafter");
        return;
    }

    wxcSettings::Get().SetSerialNumber(m_textCtrlSerialNumber->GetValue());
    wxcSettings::Get().SetUsername(m_textCtrl1Username->GetValue());
    wxcSettings::Get().SetAnnoyDialogs(wxcSettings::Get().GetAnnoyDialogs() | wxcSettings::LICENSE_ACTIVATED);
    wxcSettings::Get().Save();

    wxCommandEvent evt(wxEVT_LICENSE_UPDATED_SUCCESSFULLY);
    wxTheApp->AddPendingEvent(evt);
    m_infobarInvalid->ShowMessage(_("License updated successfully"), wxICON_INFORMATION);
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
    if(wxcSettings::Get().IsRegistered()) {
        m_staticBitmapLicenseStatusError->Hide();
        m_staticBitmapLicenseStatusOK->Show();
        m_banner193->SetText(_(""), _("Your wxCrafter is registered"));

    } else {
        m_staticBitmapLicenseStatusError->Show();
        m_staticBitmapLicenseStatusOK->Hide();
        m_banner193->SetText(_("Using an unregistered version wxCrafter"), _("Please register your wxCrafter copy"));
    }

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
    wxString licenseURL = "http://codelite.org/wxcrafter/activate_license.php";
    licenseURL << "?user=" << m_textCtrl1Username->GetValue() << "&key=" << m_textCtrlSerialNumber->GetValue();

    wxURL url(licenseURL);
    if(url.GetError() == wxURL_NOERR) {

        wxInputStream* in_stream = url.GetInputStream();
        if(!in_stream) { return false; }

        unsigned char buffer[1024];

        if(!in_stream->Eof()) {
            in_stream->Read(buffer, sizeof(buffer));
            size_t bytes_read = in_stream->LastRead();
            if(bytes_read > 0) {

                buffer[bytes_read] = 0;
                wxString buffRead((const char*)buffer, wxConvUTF8);
                JSONRoot root(buffRead);
                return root.toElement().namedObject("success").toBool();
            }
        }
    }
    return false;
}
