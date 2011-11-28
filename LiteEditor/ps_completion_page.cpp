#include "ps_completion_page.h"
#include "clang_code_completion.h"

PSCompletionPage::PSCompletionPage( wxWindow* parent, ProjectSettingsDlg* dlg )
	: PSCompletionBase( parent )
	, m_dlg(dlg)
	, m_ccSettingsModified(false)
{
}

void PSCompletionPage::OnCmdEvtVModified( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_dlg->SetIsDirty(true);
	m_ccSettingsModified = true;
}

void PSCompletionPage::Clear()
{
	m_textCtrlSearchPaths->Clear();
	m_textCtrlMacros->Clear();
	m_textCtrlSearchPaths->Clear();
	m_ccSettingsModified = false;
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
	// Save was requested
	if(m_ccSettingsModified) {
		m_ccSettingsModified = false;
		
#if HAS_LIBCLANG
		ClangCodeCompletion::Instance()->ClearCache();
#endif

	}
}
