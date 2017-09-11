#include "HelpPluginMessageDlg.h"
#include "windowattrmanager.h"

HelpPluginMessageDlg::HelpPluginMessageDlg(wxWindow* parent)
    : HelpPluginMessageBaseDlg(parent)
{
#ifdef __WXOSX__
    m_staticText->SetLabel(
        _("The Help plugin uses 'Dash' for displaying the offline documentation\nPlease click the link below to "
          "download and install Dash"));
    m_hyperLink->SetLabel(_("Go to Dash website"));
    m_hyperLink->SetURL("https://kapeli.com/dash");
#else
    m_staticText->SetLabel(
        _("The Help plugin uses 'Zeal' for displaying the offline documentation\nPlease click the link below to "
          "download and install Zeal"));
    m_hyperLink->SetLabel(_("Go to Zeal website"));
    m_hyperLink->SetURL("http://zealdocs.org/");
#endif

    GetSizer()->Fit(this);
    CentreOnParent();
}

HelpPluginMessageDlg::~HelpPluginMessageDlg() {}
