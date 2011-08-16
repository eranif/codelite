#include "ps_completion_page.h"

PSCompletionPage::PSCompletionPage( wxWindow* parent, ProjectSettingsDlg* dlg )
	: PSCompletionBase( parent )
	, m_dlg(dlg)
{
}

void PSCompletionPage::OnCmdEvtVModified( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_dlg->SetIsDirty(true);
}

void PSCompletionPage::Clear()
{
	m_textCtrlSearchPaths->Clear();
	m_textCtrlMacros->Clear();
	m_textCtrlSearchPaths->Clear();
}

void PSCompletionPage::Load(BuildConfigPtr buildConf)
{
	m_textCtrlCmpOptions->SetValue(buildConf->GetClangCmpFlags());
	m_textCtrlMacros->SetValue(buildConf->GetClangPPFlags());
	m_textCtrlSearchPaths->SetValue(buildConf->GetCcSearchPaths());
}

void PSCompletionPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
	buildConf->SetClangCmpFlags(m_textCtrlCmpOptions->GetValue());
	buildConf->SetClangPPFlags(m_textCtrlMacros->GetValue());
	buildConf->SetCcSearchPaths(m_textCtrlSearchPaths->GetValue());
}
