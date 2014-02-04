#include "ps_resources_page.h"
#include "macros.h"

#ifdef __WXMSW__
#    define IS_WINDOWS true
#else
#    define IS_WINDOWS false
#endif

PSResourcesPage::PSResourcesPage( wxWindow* parent, ProjectSettingsDlg *dlg )
    : PSResourcesPageBase( parent )
    , m_dlg(dlg)
{
}

void PSResourcesPage::Load(BuildConfigPtr buildConf)
{
    m_pgPropResCmpOptions->SetValue(buildConf->GetResCompileOptions());
    m_pgPropResCmpSearchPath->SetValue(buildConf->GetResCmpIncludePath());
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildResWithGlobalSettings());
}

void PSResourcesPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetResCmpIncludePath( m_pgPropResCmpSearchPath->GetValueAsString() );
    buildConf->SetResCmpOptions( m_pgPropResCmpOptions->GetValueAsString() );
    buildConf->SetBuildResWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings->GetValueAsString());
}

void PSResourcesPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for( ; !iter.AtEnd(); ++iter ) {
        if ( iter.GetProperty() && !iter.GetProperty()->IsCategory() ) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
}

void PSResourcesPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    m_dlg->SetIsDirty(true);
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);

    if ( prop == m_pgPropResCmpOptions || prop == m_pgPropResCmpSearchPath ) {
        wxString v = prop->GetValueAsString();
        if ( PopupAddOptionDlg( v ) ) {
            prop->SetValue( v );
        }
    }
}

void PSResourcesPage::OnValueChanged(wxPropertyGridEvent& event)
{
    m_dlg->SetIsDirty(true);
}

void PSResourcesPage::OnResourcesEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dlg->IsCustomBuildEnabled() );
}

void PSResourcesPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dlg->IsProjectEnabled() );
}
