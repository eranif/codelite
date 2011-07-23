#include "ps_compiler_page.h"
#include "compiler.h"
#include "addoptioncheckdlg.h"
#include "project_settings_dlg.h"
#include "build_settings_config.h"
#include "project.h"
#include "ps_general_page.h"
#include "manager.h"

PSCompilerPage::PSCompilerPage( wxWindow* parent, const wxString &projectName, ProjectSettingsDlg *dlg, PSGeneralPage *gp )
	: PSCompilerPageBase( parent )
	, m_dlg(dlg)
	, m_projectName(projectName)
	, m_gp(gp)
{
	m_choiceCmpUseWithGlobalSettings->AppendString(APPEND_TO_GLOBAL_SETTINGS);
	m_choiceCmpUseWithGlobalSettings->AppendString(OVERWRITE_GLOBAL_SETTINGS);
	m_choiceCmpUseWithGlobalSettings->AppendString(PREPEND_GLOBAL_SETTINGS);
	m_choiceCmpUseWithGlobalSettings->Select(0);
	// We do this here rather than in wxFB to avoid failure and an assert in >wx2.8
	m_gbSizer2->AddGrowableCol(1);

}

void PSCompilerPage::OnCheckCompilerNeeded( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_dlg->SetIsDirty(true);
}

void PSCompilerPage::OnCompiledNotNeededUI( wxUpdateUIEvent& event )
{
	event.Enable(!m_checkCompilerNeeded->IsChecked() && !m_dlg->IsCustomBuildEnabled());
}

void PSCompilerPage::OnCmdEvtVModified( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_dlg->SetIsDirty(true);
}

void PSCompilerPage::OnButtonAddCompilerOptions( wxCommandEvent& event )
{
	wxString cmpName = m_gp->GetCompiler();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
	if (PopupAddOptionCheckDlg(m_textCompilerOptions, _("Compiler options"), cmp->GetCompilerOptions())) {
		m_dlg->SetIsDirty(true);
	}
	event.Skip();
}

void PSCompilerPage::OnAddSearchPath( wxCommandEvent& event )
{
	if (PopupAddOptionDlg(m_textAdditionalSearchPath)) {
		m_dlg->SetIsDirty(true);
	}
	event.Skip();
}

void PSCompilerPage::OnButtonAddPreprocessor( wxCommandEvent& event )
{
	if (PopupAddOptionDlg(m_textPreprocessor)) {
		m_dlg->SetIsDirty(true);
	}
	event.Skip();
}

void PSCompilerPage::OnBrowsePreCmpHeader( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString projectPath;
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if (p) {
		projectPath = p->GetFileName().GetPath();
	}
	wxString preCmpHeader = wxFileSelector(_("Select file:"), projectPath, wxT(""));
	if (preCmpHeader.IsEmpty() == false) {
		m_textCtrlPreCompiledHeader->SetValue( preCmpHeader );
	}
}

void PSCompilerPage::Load(BuildConfigPtr buildConf)
{
	m_checkCompilerNeeded->SetValue(!buildConf->IsCompilerRequired());
	m_textCompilerOptions->SetValue(buildConf->GetCompileOptions());
	m_textAdditionalSearchPath->SetValue(buildConf->GetIncludePath());
	m_textPreprocessor->SetValue(buildConf->GetPreprocessor());
	m_textCtrlPreCompiledHeader->SetValue(buildConf->GetPrecompiledHeader());
	m_textCtrlCCompilerOptions->SetValue(buildConf->GetCCompileOptions());
	SelectChoiceWithGlobalSettings(m_choiceCmpUseWithGlobalSettings, buildConf->GetBuildCmpWithGlobalSettings());
	m_checkBoxPCHInCommandLine->SetValue(buildConf->GetPchInCommandLine());
}

void PSCompilerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
	buildConf->SetCompilerRequired(!m_checkCompilerNeeded->IsChecked());
	buildConf->SetCompileOptions(m_textCompilerOptions->GetValue());
	buildConf->SetIncludePath(m_textAdditionalSearchPath->GetValue());
	buildConf->SetPreprocessor(m_textPreprocessor->GetValue());
	buildConf->SetPrecompiledHeader(m_textCtrlPreCompiledHeader->GetValue());
	buildConf->SetCCompileOptions(m_textCtrlCCompilerOptions->GetValue());
	buildConf->SetPchInCommandLine(m_checkBoxPCHInCommandLine->IsChecked());
	
	wxString useWithGlobalSettings = m_choiceCmpUseWithGlobalSettings->GetStringSelection();
	if (useWithGlobalSettings == APPEND_TO_GLOBAL_SETTINGS) {
		buildConf->SetBuildCmpWithGlobalSettings(BuildConfig::APPEND_TO_GLOBAL_SETTINGS);
	} else if (useWithGlobalSettings == OVERWRITE_GLOBAL_SETTINGS) {
		buildConf->SetBuildCmpWithGlobalSettings(BuildConfig::OVERWRITE_GLOBAL_SETTINGS);
	} else if (useWithGlobalSettings == PREPEND_GLOBAL_SETTINGS) {
		buildConf->SetBuildCmpWithGlobalSettings(BuildConfig::PREPEND_GLOBAL_SETTINGS);
	}
}

void PSCompilerPage::Clear()
{
	m_checkCompilerNeeded->SetValue(false);
	m_textCompilerOptions->Clear();
	m_textAdditionalSearchPath->Clear();
	m_textPreprocessor->Clear();
	m_textCtrlPreCompiledHeader->Clear();
	m_textCtrlCCompilerOptions->Clear();
}

void PSCompilerPage::OnProjectCustumBuildUI(wxUpdateUIEvent& event)
{
	event.Enable( !m_dlg->IsCustomBuildEnabled() );
}

void PSCompilerPage::OnButtonAddCCompilerOptions(wxCommandEvent& event)
{
	wxString cmpName = m_gp->GetCompiler();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
	if (PopupAddOptionCheckDlg(m_textCtrlCCompilerOptions, _("C Compiler options"), cmp->GetCompilerOptions())) {
		m_dlg->SetIsDirty(true);
	}
	event.Skip();
}
