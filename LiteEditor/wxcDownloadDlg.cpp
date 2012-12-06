#include "wxcDownloadDlg.h"
#include <wx/utils.h>

wxcDownloadDlg::wxcDownloadDlg(wxWindow* parent)
    : wxcDownloadDlgBaseClass(parent)
{
    m_cmdLnkBtnDownload->SetDefault();
}

wxcDownloadDlg::~wxcDownloadDlg()
{
}
void wxcDownloadDlg::OnDownloadWxCrafterPlugin(wxCommandEvent& event)
{
    ::wxLaunchDefaultBrowser("http://wxcrafter.codelite.org");
    this->EndModal(wxID_OK);
}

void wxcDownloadDlg::OnIgnoreTheError(wxCommandEvent& event)
{
    this->EndModal(wxID_OK);
}
