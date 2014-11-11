#include "XDebugDiagDlg.h"
#include "windowattrmanager.h"
#include "globals.h"
#include <wx/msgdlg.h>
#include "php_configuration_data.h"

XDebugDiagDlg::XDebugDiagDlg(wxWindow* parent)
    : XDebugDiagDlgBase(parent)
{
    WindowAttrManager::Load(this, "XDebugDiagDlg");
}

XDebugDiagDlg::~XDebugDiagDlg()
{
    WindowAttrManager::Save(this, "XDebugDiagDlg");
}

void XDebugDiagDlg::Load(const wxString& html)
{
    m_htmlWin289->SetPage(html);
}

void XDebugDiagDlg::OnRecommend(wxCommandEvent& event)
{
    wxString endOfLine;
#ifdef __WXMSW__
    endOfLine = "\r\n";
#else
    endOfLine = "\n";
#endif
    PHPConfigurationData config;
    config.Load();
    wxString recommendation;
    recommendation << "xdebug.remote_enable=1" << endOfLine
                   << "xdebug.remote_connect_back=1" << endOfLine
                   << "xdebug.ide_key=\"" << config.GetXdebugIdeKey() << "\"" <<endOfLine;
    ::CopyToClipboard(recommendation);
    ::wxMessageBox(_("Recommended settings have been copied to your clipboard\nOpen your php.ini file and paste them"));
    
}
