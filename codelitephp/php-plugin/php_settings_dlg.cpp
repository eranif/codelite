#include "ColoursAndFontsManager.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "php_configuration_data.h"
#include "php_settings_dlg.h"
#include "php_workspace.h"
#include <event_notifier.h>
#include <windowattrmanager.h>
#include <wx/dirdlg.h>

PHPSettingsDlg::PHPSettingsDlg(wxWindow* parent)
    : PHPSettingsBaseDlg(parent)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("Default");
    if(lexer) {
        lexer->Apply(m_stcIncludePaths);
    }

    PHPConfigurationData data;
    data.Load();

    m_textCtrlErrorReporting->ChangeValue(data.GetErrorReporting());
    m_filePickerPHPPath->SetPath(data.GetPhpExe());
    m_stcIncludePaths->SetText(data.GetIncludePathsAsString());
    m_textCtrCClIncludePath->ChangeValue(data.GetCCIncludePathsAsString());
    m_textCtrlIdeKey->ChangeValue(data.GetXdebugIdeKey());
    wxString strPort;
    strPort << data.GetXdebugPort();
    m_textCtrlXDebugPort->ChangeValue(strPort);
    m_textCtrlHost->ChangeValue(data.GetXdebugHost());
    ::clSetDialogBestSizeAndPosition(this);
}

PHPSettingsDlg::~PHPSettingsDlg()
{
}

void PHPSettingsDlg::OnBrowseForIncludePath(wxCommandEvent& event)
{
    wxString path = wxDirSelector();
    if(path.IsEmpty() == false) {
        wxString curpath = m_stcIncludePaths->GetText();
        curpath.Trim().Trim(false);
        if(curpath.IsEmpty() == false) {
            curpath << wxT("\n");
        }
        curpath << path;
        m_stcIncludePaths->SetText(curpath);
    }
}

void PHPSettingsDlg::OnOK(wxCommandEvent& event)
{
    PHPConfigurationData data;
    data.SetErrorReporting(m_textCtrlErrorReporting->GetValue());
    data.SetIncludePaths(wxStringTokenize(m_stcIncludePaths->GetText(), wxT("\n\r"), wxTOKEN_STRTOK));
    data.SetCcIncludePath(wxStringTokenize(m_textCtrCClIncludePath->GetValue(), wxT("\n\n"), wxTOKEN_STRTOK));
    data.SetPhpExe(m_filePickerPHPPath->GetPath());
    data.SetXdebugIdeKey(m_textCtrlIdeKey->GetValue());
    data.SetXdebugHost(m_textCtrlHost->GetValue());
    wxString xdebugPort = m_textCtrlXDebugPort->GetValue();
    long port(0);
    if(xdebugPort.ToLong((long*)&port)) {
        data.SetXdebugPort(port);
    }

    data.Save();

    // Send an event to trigger a retag, but only if we got a PHP workspace opened
    if(PHPWorkspace::Get()->IsOpen()) {
        wxCommandEvent evtRetag(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evtRetag);
    }
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
