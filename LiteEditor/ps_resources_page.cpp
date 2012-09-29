#include "ps_resources_page.h"

#ifdef __WXMSW__
#    define IS_WINDOWS true
#else
#    define IS_WINDOWS false
#endif

PSResourcesPage::PSResourcesPage( wxWindow* parent, ProjectSettingsDlg *dlg )
    : PSResourcesPageBase( parent )
    , m_dlg(dlg)
{
    m_choiceResUseWithGlobalSettings->AppendString(APPEND_TO_GLOBAL_SETTINGS);
    m_choiceResUseWithGlobalSettings->AppendString(OVERWRITE_GLOBAL_SETTINGS);
    m_choiceResUseWithGlobalSettings->AppendString(PREPEND_GLOBAL_SETTINGS);
    m_choiceResUseWithGlobalSettings->Select(0);
    // We do this here rather than in wxFB to avoid failure and an assert in >wx2.8
    m_gbSizer4->AddGrowableCol(1);
}

void PSResourcesPage::OnResourceCmpNeeded( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}

void PSResourcesPage::OnrResourceCompilerNotNeededUI( wxUpdateUIEvent& event )
{
    event.Enable( IS_WINDOWS && !m_dlg->IsCustomBuildEnabled());
}

void PSResourcesPage::OnCmdEvtVModified( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}

void PSResourcesPage::OnResourceCmpAddOption( wxCommandEvent& event )
{
    if (PopupAddOptionDlg(m_textAddResCmpOptions)) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void PSResourcesPage::OnResourceCmpAddPath( wxCommandEvent& event )
{
    if (PopupAddOptionDlg(m_textAddResCmpPath)) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void PSResourcesPage::Load(BuildConfigPtr buildConf)
{
    m_textAddResCmpOptions->SetValue(buildConf->GetResCompileOptions());
    m_textAddResCmpPath->SetValue(buildConf->GetResCmpIncludePath());
    SelectChoiceWithGlobalSettings(m_choiceResUseWithGlobalSettings, buildConf->GetBuildResWithGlobalSettings());
}

void PSResourcesPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetResCmpIncludePath(m_textAddResCmpPath->GetValue());
    buildConf->SetResCmpOptions(m_textAddResCmpOptions->GetValue());

    wxString useWithGlobalSettings = m_choiceResUseWithGlobalSettings->GetStringSelection();
    if (useWithGlobalSettings == APPEND_TO_GLOBAL_SETTINGS) {
        buildConf->SetBuildResWithGlobalSettings(BuildConfig::APPEND_TO_GLOBAL_SETTINGS);
    } else if (useWithGlobalSettings == OVERWRITE_GLOBAL_SETTINGS) {
        buildConf->SetBuildResWithGlobalSettings(BuildConfig::OVERWRITE_GLOBAL_SETTINGS);
    } else if (useWithGlobalSettings == PREPEND_GLOBAL_SETTINGS) {
        buildConf->SetBuildResWithGlobalSettings(BuildConfig::PREPEND_GLOBAL_SETTINGS);
    }
}

void PSResourcesPage::Clear()
{
    m_textAddResCmpOptions->Clear();
    m_textAddResCmpPath->Clear();
}

void PSResourcesPage::OnProjectCustumBuildUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dlg->IsCustomBuildEnabled() );
}
