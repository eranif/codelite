#include "php_settings_dlg.h"
#include "php_configuration_data.h"
#include <wx/dirdlg.h>
#include <windowattrmanager.h>
#include <event_notifier.h>

PHPSettingsDlg::PHPSettingsDlg( wxWindow* parent )
    : PHPSettingsBaseDlg( parent )
{
    clConfig conf("php.conf");
    PHPConfigurationData data;
    conf.ReadItem(&data);

    m_textCtrlErrorReporting->ChangeValue(data.GetErrorReporting());
    m_filePickerPHPPath->SetPath(data.GetPhpExe());
    m_textCtrlIncludePath->ChangeValue(data.GetIncludePathsAsString());
    m_textCtrCClIncludePath->ChangeValue(data.GetCCIncludePathsAsString());
    m_textCtrlIdeKey->ChangeValue( data.GetXdebugIdeKey() );
    
    wxString strPort;
    strPort << data.GetXdebugPort();
    m_textCtrlXDebugPort->ChangeValue(strPort);

    WindowAttrManager::Load(this, wxT("PHPSettingsDlg"), NULL);
}

PHPSettingsDlg::~PHPSettingsDlg()
{
    WindowAttrManager::Save(this, wxT("PHPSettingsDlg"), NULL);
}

void PHPSettingsDlg::OnBrowseForIncludePath( wxCommandEvent& event )
{
    wxString path = wxDirSelector();
    if(path.IsEmpty() == false) {
        wxString curpath = m_textCtrlIncludePath->GetValue();
        curpath.Trim().Trim(false);
        if(curpath.IsEmpty() == false) {
            curpath << wxT("\n");
        }
        curpath << path;
        m_textCtrlIncludePath->SetValue(curpath);
    }
}

void PHPSettingsDlg::OnOK(wxCommandEvent& event)
{
    PHPConfigurationData data;
    data.SetErrorReporting(m_textCtrlErrorReporting->GetValue());
    data.SetIncludePaths(wxStringTokenize(m_textCtrlIncludePath->GetValue(), wxT("\n\r"), wxTOKEN_STRTOK));
    data.SetCcIncludePath(wxStringTokenize(m_textCtrCClIncludePath->GetValue(), wxT("\n\n"), wxTOKEN_STRTOK));
    data.SetPhpExe(m_filePickerPHPPath->GetPath());
    data.SetXdebugIdeKey( m_textCtrlIdeKey->GetValue() );
    wxString xdebugPort = m_textCtrlXDebugPort->GetValue();
    long port(0);
    if(xdebugPort.ToLong((long*)&port)) {
        data.SetXdebugPort(port);
    }

    clConfig conf("php.conf");
    conf.WriteItem(&data);

    // Send an event to trigger a retag
    wxCommandEvent evtRetag( wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace") );
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent( evtRetag );

    EndModal(wxID_OK);
}

void PHPSettingsDlg::OnAddCCPath(wxCommandEvent& event)
{
    wxString path = wxDirSelector();
    if(path.IsEmpty() == false) {

        wxString curpath = m_textCtrCClIncludePath->GetValue();
        curpath.Trim().Trim(false);

        if(!curpath.IsEmpty()) {
            curpath << wxT("\n");
        }
        curpath << path;
        m_textCtrCClIncludePath->SetValue(curpath);
    }
}

void PHPSettingsDlg::OnUpdateApplyUI(wxCommandEvent& event)
{
    event.Skip();
}
