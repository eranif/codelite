#include "ps_debugger_page.h"
#include <wx/filename.h>
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

void PSDebuggerPage::OnSelectDebuggerPath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString path(m_textCtrlDebuggerPath->GetValue());
	if (wxFileName::FileExists(path)) {
		m_textCtrlDebuggerPath->SetValue(wxFileSelector(_("Select file:"), path.c_str()));
	} else {
		m_textCtrlDebuggerPath->SetValue(wxFileSelector(_("Select file:")));
	}
}

void PSDebuggerPage::OnRemoteDebugUI( wxUpdateUIEvent& event )
{
	event.Enable(m_checkBoxDbgRemote->IsChecked());
}

void PSDebuggerPage::Load(BuildConfigPtr buildConf)
{
	m_textCtrlDbgCmds->SetValue(buildConf->GetDebuggerStartupCmds());
	m_textCtrlDbgPostConnectCmds->SetValue(buildConf->GetDebuggerPostRemoteConnectCmds());
	m_checkBoxDbgRemote->SetValue(buildConf->GetIsDbgRemoteTarget());
	m_textCtrl1DbgHost->SetValue(buildConf->GetDbgHostName());
	m_textCtrlDbgPort->SetValue(buildConf->GetDbgHostPort());
	m_textCtrlDebuggerPath->SetValue(buildConf->GetDebuggerPath());
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
