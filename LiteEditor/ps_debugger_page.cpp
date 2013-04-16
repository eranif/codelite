#include "ps_debugger_page.h"
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <project.h>
#include <workspace.h>
#include <wx/filedlg.h>

PSDebuggerPage::PSDebuggerPage( wxWindow* parent, ProjectSettingsDlg* dlg )
    : PSDebuggerPageBase( parent )
    , m_dlg(dlg)
{
}

void PSDebuggerPage::OnCmdEvtVModified( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}

void PSDebuggerPage::OnRemoteDebugUI( wxUpdateUIEvent& event )
{
    event.Enable(m_checkBoxDbgRemote->IsChecked());
}

void PSDebuggerPage::Load(BuildConfigPtr buildConf)
{
    m_textCtrlDbgCmds->ChangeValue(buildConf->GetDebuggerStartupCmds());
    m_textCtrlDbgPostConnectCmds->ChangeValue(buildConf->GetDebuggerPostRemoteConnectCmds());
    m_checkBoxDbgRemote->SetValue(buildConf->GetIsDbgRemoteTarget());
    m_textCtrl1DbgHost->ChangeValue(buildConf->GetDbgHostName());
    m_textCtrlDbgPort->ChangeValue(buildConf->GetDbgHostPort());
    m_textCtrlDebuggerPath->ChangeValue(buildConf->GetDebuggerPath());
}

void PSDebuggerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetDebuggerStartupCmds(m_textCtrlDbgCmds->GetValue());
    buildConf->SetDebuggerPostRemoteConnectCmds(m_textCtrlDbgPostConnectCmds->GetValue());
    buildConf->SetIsDbgRemoteTarget(m_checkBoxDbgRemote->IsChecked());
    buildConf->SetDbgHostName(m_textCtrl1DbgHost->GetValue());
    buildConf->SetDbgHostPort(m_textCtrlDbgPort->GetValue());
    buildConf->SetDebuggerPath(m_textCtrlDebuggerPath->GetValue());
}

void PSDebuggerPage::Clear()
{
    m_textCtrlDebuggerPath->Clear();
    m_textCtrl1DbgHost->Clear();
    m_textCtrlDbgCmds->Clear();
    m_textCtrlDbgPort->Clear();
    m_textCtrlDbgPostConnectCmds->Clear();
    m_checkBoxDbgRemote->SetValue(false);
}

void PSDebuggerPage::OnBrowseForDebuggerPath(wxCommandEvent& event)
{
    wxString debugger_path = ::wxFileSelector(_("Select debugger:"));
    
    if ( !debugger_path.IsEmpty() ) {
        wxString errMsg;
        ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_dlg->GetProjectName(), errMsg);
        if ( proj ) {
            wxFileName fnDebuggerPath( debugger_path );
            wxString project_path = proj->GetFileName().GetPath();
            if ( fnDebuggerPath.MakeRelativeTo( project_path ) ) {
                debugger_path = fnDebuggerPath.GetFullPath();
            }
        }
        m_textCtrlDebuggerPath->ChangeValue( debugger_path );
        m_dlg->SetIsDirty(true);
    }
}
