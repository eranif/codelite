#include "ps_linker_page.h"
#include "compiler.h"
#include "build_settings_config.h"
#include "ps_general_page.h"

PSLinkerPage::PSLinkerPage( wxWindow* parent, ProjectSettingsDlg *dlg, PSGeneralPage *gp )
	: PSLinkPageBase( parent )
	, m_dlg(dlg)
	, m_gp(gp)
{
	m_choiceLnkUseWithGlobalSettings->AppendString(APPEND_TO_GLOBAL_SETTINGS);
	m_choiceLnkUseWithGlobalSettings->AppendString(OVERWRITE_GLOBAL_SETTINGS);
	m_choiceLnkUseWithGlobalSettings->AppendString(PREPEND_GLOBAL_SETTINGS);
	m_choiceLnkUseWithGlobalSettings->Select(0);
	// We do this here rather than in wxFB to avoid failure and an assert in >wx2.8
	m_gbSizer3->AddGrowableCol(1);
}

void PSLinkerPage::OnCheckLinkerNeeded( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_dlg->SetIsDirty(true);
}

void PSLinkerPage::OnLinkerNotNeededUI( wxUpdateUIEvent& event )
{
	event.Enable( !m_checkLinkerNeeded->IsChecked() && !m_dlg->IsCustomBuildEnabled() );
}

void PSLinkerPage::OnCmdEvtVModified( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_dlg->SetIsDirty(true);
}

void PSLinkerPage::OnButtonAddLinkerOptions( wxCommandEvent& event )
{
	wxString cmpName = m_gp->GetCompiler();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
	if (PopupAddOptionCheckDlg(m_textLinkerOptions, _("Linker options"), cmp->GetLinkerOptions())) {
		m_dlg->SetIsDirty(true);
	}
	event.Skip();
}

void PSLinkerPage::OnAddLibraryPath( wxCommandEvent& event )
{
	if (PopupAddOptionDlg(m_textLibraryPath)) {
		m_dlg->SetIsDirty(true);
	}
	event.Skip();

}

void PSLinkerPage::OnAddLibrary( wxCommandEvent& event )
{
	if (PopupAddOptionDlg(m_textLibraries)) {
		m_dlg->SetIsDirty(true);
	}
	event.Skip();
}

void PSLinkerPage::Load(BuildConfigPtr buildConf)
{
	m_checkLinkerNeeded->SetValue( !buildConf->IsLinkerRequired() );
	m_textLinkerOptions->SetValue(buildConf->GetLinkOptions());
	m_textLibraries->SetValue(buildConf->GetLibraries());
	m_textLibraryPath->SetValue(buildConf->GetLibPath());
	SelectChoiceWithGlobalSettings(m_choiceLnkUseWithGlobalSettings, buildConf->GetBuildLnkWithGlobalSettings());
}

void PSLinkerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
	buildConf->SetLibPath(m_textLibraryPath->GetValue());
	buildConf->SetLibraries(m_textLibraries->GetValue());
	buildConf->SetLinkerRequired(!m_checkLinkerNeeded->IsChecked());
	buildConf->SetLinkOptions(m_textLinkerOptions->GetValue());

	wxString useWithGlobalSettings = m_choiceLnkUseWithGlobalSettings->GetStringSelection();
	if (useWithGlobalSettings == APPEND_TO_GLOBAL_SETTINGS) {
		buildConf->SetBuildLnkWithGlobalSettings(BuildConfig::APPEND_TO_GLOBAL_SETTINGS);

	} else if (useWithGlobalSettings == OVERWRITE_GLOBAL_SETTINGS) {
		buildConf->SetBuildLnkWithGlobalSettings(BuildConfig::OVERWRITE_GLOBAL_SETTINGS);

	} else if (useWithGlobalSettings == PREPEND_GLOBAL_SETTINGS) {
		buildConf->SetBuildLnkWithGlobalSettings(BuildConfig::PREPEND_GLOBAL_SETTINGS);
	}
}

void PSLinkerPage::Clear()
{
	m_checkLinkerNeeded->SetValue(false);
	m_textLinkerOptions->SetValue(wxEmptyString);
	m_textLibraries->SetValue(wxEmptyString);
	m_textLibraryPath->SetValue(wxEmptyString);
}

void PSLinkerPage::OnProjectCustumBuildUI(wxUpdateUIEvent& event)
{
	event.Enable( !m_dlg->IsCustomBuildEnabled() );
}
